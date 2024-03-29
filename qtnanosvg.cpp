#include <QtGlobal>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>

#include <climits>

#include <algorithm>
#include <execution>

#include <utility>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include "nanosvg/src/nanosvg.h"

#include "qtnanosvg.hpp"

//////////////////////////////////////////////////////////////////////////////
inline auto inverse(float const* const f0) noexcept
{
  std::array<float, 6> f1{
    f0[3], -f0[1],
    -f0[2], f0[0],
    f0[2] * f0[5] - f0[3] * f0[4],
    f0[1] * f0[4] - f0[0] * f0[5]
  };

  std::transform(
    std::execution::unseq,
    f1.cbegin(),
    f1.cend(),
    f1.begin(),
    [invdet(1.f / (f0[0] * f0[3] - f0[2] * f0[1]))](auto const f) noexcept
    {
      return f * invdet;
    }
  );

  return f1;
}

inline auto toQColor(quint32 const c, float const o) noexcept
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
  return QColor(quint8(c), quint8(c >> 8), quint8(c >> 16),
    qRound(quint8(c >> 24) * o));
#else
  return QColor(quint8(c >> 24), quint8(c >> 16), quint8(c >> 8),
    qRound(quint8(c) * o));
#endif // Q_BYTE_ORDER
}

//////////////////////////////////////////////////////////////////////////////
inline void drawSVGShape(QPainter* const p,
  struct NSVGshape const* const shape)
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

    if (path->closed) qpath.closeSubpath();
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
          auto const& g(*shape->fill.gradient);

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
              auto const& stp(g.stops[i]);

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

            auto const& g(*shape->fill.gradient);

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
void drawSVGImage(QPainter* const p, struct NSVGimage const* const image,
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
void drawSVGImage(QPainter* const p, struct NSVGimage const* const image,
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
    if (NSVG_FLAGS_VISIBLE & shape->flags) drawSVGShape(p, shape);
  }

  //
  p->restore();
}
