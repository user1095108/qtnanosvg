#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>

#include <array>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include "nanosvg/src/nanosvg.h"

#include "qtnanosvg.hpp"

//////////////////////////////////////////////////////////////////////////////
inline auto to_rgba(unsigned int const c) noexcept
{
  return std::array<quint8, 4>{
    quint8(c), quint8(c >> 8), quint8(c >> 16), quint8(c >> 24)
  };
}

inline auto inverse(float const* const t) noexcept
{
  auto const invdet(qreal(1) / (qreal(t[0]) * t[3] - qreal(t[2]) * t[1]));

  return std::array<qreal, 6>{
    t[3] * invdet, -t[1] * invdet,
    -t[2] * invdet, t[0] * invdet,
    (qreal(t[2]) * t[5] - qreal(t[3]) * t[4]) * invdet,
    (qreal(t[1]) * t[4] - qreal(t[0]) * t[5]) * invdet
  };
}

//////////////////////////////////////////////////////////////////////////////
inline void drawSVGShape(QPainter* const p, struct NSVGshape* const shape)
{
  QPainterPath qpath;

  for (auto path(shape->paths); path; path = path->next)
  {
    {
      auto p(path->pts);

      qpath.moveTo(p[0], p[1]);

      auto const end(p + 2 * path->npts);

      for (p += 2; end != p; p += 6)
      {
        qpath.cubicTo(p[0], p[1], p[2], p[3], p[4], p[5]);
      }
    }

    if (path->closed)
    {
      qpath.closeSubpath();
    }
  }

  // fill
  switch (auto const type(shape->fill.type); type)
  {
    case NSVG_PAINT_NONE:
      break;

    case NSVG_PAINT_COLOR:
    case NSVG_PAINT_LINEAR_GRADIENT:
    case NSVG_PAINT_RADIAL_GRADIENT:
    {
      switch (shape->fillRule)
      {
        case NSVG_FILLRULE_NONZERO:
          qpath.setFillRule(Qt::WindingFill);

          break;

        case NSVG_FILLRULE_EVENODD:
          qpath.setFillRule(Qt::OddEvenFill);

          break;

        default:
          Q_ASSERT(0);
      }

      auto const fillWithGradient([&](QGradient& gr)
        {
          auto& g(*shape->fill.gradient);

          switch (g.spread)
          {
            case NSVG_SPREAD_PAD:
              gr.setSpread(QGradient::PadSpread);

              break;

            case NSVG_SPREAD_REFLECT:
              gr.setSpread(QGradient::ReflectSpread);

              break;

            case NSVG_SPREAD_REPEAT:
              gr.setSpread(QGradient::RepeatSpread);

              break;

            default:
              Q_ASSERT(0);
          }

          {
            auto const ns(g.nstops);

            for (decltype(g.nstops) i{}; ns != i; ++i)
            {
              auto& stop(g.stops[i]);

              auto const c(to_rgba(stop.color));
              gr.setColorAt(stop.offset,
                QColor(c[0], c[1], c[2], qRound(shape->opacity * c[3])));
            }
          }

          p->fillPath(qpath, gr);
        }
      );

      switch (type)
      {
        case NSVG_PAINT_COLOR:
          {
            auto const c(to_rgba(shape->fill.color));
            p->fillPath(qpath,
              QColor(c[0], c[1], c[2], qRound(shape->opacity * c[3])));

            break;
          }

        case NSVG_PAINT_LINEAR_GRADIENT:
          {
            QLinearGradient lgr;

            auto const t(inverse(shape->fill.gradient->xform));

            lgr.setStart(t[4], t[5]);
            lgr.setFinalStop(t[4] + t[2], t[5] + t[3]);

            fillWithGradient(lgr);

            break;
          }

        case NSVG_PAINT_RADIAL_GRADIENT:
          {
            QRadialGradient rgr;

            auto& g(*shape->fill.gradient);

            auto const t(inverse(g.xform));
            auto const r(t[0]);

            rgr.setCenter(g.fx * r, g.fy * r);
            rgr.setCenterRadius(0);

            rgr.setFocalPoint(t[4], t[5]);
            rgr.setFocalRadius(r);

            fillWithGradient(rgr);

            break;
          }

        default:
          Q_ASSERT(0);
      }

      break;
    }

    default:
      Q_ASSERT(0);
  }

  // stroke
  switch (shape->stroke.type)
  {
    case NSVG_PAINT_NONE:
      break;

    case NSVG_PAINT_COLOR:
      {
        auto const c(to_rgba(shape->stroke.color));

        QPen pen(QColor(c[0], c[1], c[2], qRound(shape->opacity * c[3])));

        pen.setWidthF(shape->strokeWidth);

        if (auto const count(shape->strokeDashCount); count)
        {
          pen.setDashOffset(shape->strokeDashOffset);
          pen.setDashPattern(QVector<qreal>(shape->strokeDashArray,
            shape->strokeDashArray + count));
        }

        switch (shape->strokeLineCap)
        {
          case NSVG_CAP_BUTT:
            pen.setCapStyle(Qt::FlatCap);

            break;

          case NSVG_CAP_ROUND:
            pen.setCapStyle(Qt::RoundCap);

            break;

          case NSVG_CAP_SQUARE:
            pen.setCapStyle(Qt::SquareCap);

            break;

          default:
            Q_ASSERT(0);
        }

        switch (shape->strokeLineJoin)
        {
          case NSVG_JOIN_BEVEL:
            pen.setJoinStyle(Qt::BevelJoin);

            break;

          case NSVG_JOIN_MITER:
            pen.setJoinStyle(Qt::SvgMiterJoin);
            pen.setMiterLimit(shape->miterLimit);

            break;

          case NSVG_JOIN_ROUND:
            pen.setJoinStyle(Qt::RoundJoin);

            break;

          default:
            Q_ASSERT(0);
        }

        p->strokePath(qpath, pen);

        break;
      }

    default:
      Q_ASSERT(0);
  }
}

//////////////////////////////////////////////////////////////////////////////
void drawSVGImage(QPainter* const p, struct NSVGimage* const image,
  qreal const w, qreal const h)
{
  // preserve aspect ratio
  {
    auto const sm(qMin(w / image->width, h / image->height));

    p->translate(
      qreal(.5) * (w - sm * image->width),
      qreal(.5) * (h - sm * image->height)
    );
    p->scale(sm, sm);
  }

  // draw shapes
  for (auto shape(image->shapes); shape; shape = shape->next)
  {
    drawSVGShape(p, shape);
  }
}

//////////////////////////////////////////////////////////////////////////////
void drawSVGImage(QPainter* const p, struct NSVGimage* const image,
  qreal const x, qreal const y, qreal const w, qreal const h)
{
  p->save();

  // preserve aspect ratio
  {
    auto const sm(qMin(w / image->width, h / image->height));

    p->translate(
      x + qreal(.5) * (w - sm * image->width),
      y + qreal(.5) * (h - sm * image->height)
    );
    p->scale(sm, sm);
  }

  // draw shapes
  for (auto shape(image->shapes); shape; shape = shape->next)
  {
    if (NSVG_FLAGS_VISIBLE & shape->flags)
    {
      drawSVGShape(p, shape);
    }
  }

  p->restore();
}
