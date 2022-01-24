#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>

#include <climits>

#include <array>
#include <utility>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include "nanosvg/src/nanosvg.h"

#include "qtnanosvg.hpp"

//////////////////////////////////////////////////////////////////////////////
inline auto toQColor(auto const c, auto const o) noexcept
{
  return [&]<auto ...I>(std::index_sequence<I...>) noexcept
    {
      return QColor(
        (
          I == 3 ?
            qRound(o * quint8(c >> CHAR_BIT * I)) :
            quint8(c >> CHAR_BIT * I)
        )...
      );
    }(std::make_index_sequence<4>());
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
              auto& stp(g.stops[i]);

              gr.setColorAt(stp.offset, toQColor(stp.color, shape->opacity));
            }
          }

          p->fillPath(qpath, gr);
        }
      );

      switch (type)
      {
        case NSVG_PAINT_COLOR:
          p->fillPath(qpath, toQColor(shape->fill.color, shape->opacity));

          break;

        case NSVG_PAINT_LINEAR_GRADIENT:
          {
            QLinearGradient lgr;

            auto const t(inverse(shape->fill.gradient->xform));

            lgr.setStart(t[4], t[5]);
            lgr.setFinalStop(t[2] + t[4], t[3] + t[5]);

            fillWithGradient(lgr);

            break;
          }

        case NSVG_PAINT_RADIAL_GRADIENT:
          {
            QRadialGradient rgr;

            auto& g(*shape->fill.gradient);

            auto const t(inverse(g.xform));
            auto const r(-t[0]);

            rgr.setCenter(g.fx * r, g.fy * r);
            rgr.setCenterRadius(0);

            rgr.setFocalPoint(t[4], t[5]);
            rgr.setFocalRadius(t[0]);

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
        QPen pen(toQColor(shape->stroke.color, shape->opacity));

        pen.setWidthF(shape->strokeWidth);

        if (auto const count(shape->strokeDashCount); count)
        {
          pen.setDashOffset(shape->strokeDashOffset);
          pen.setDashPattern(
            {
              shape->strokeDashArray,
              shape->strokeDashArray + count
            }
          );
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

  //
  p->restore();
}
