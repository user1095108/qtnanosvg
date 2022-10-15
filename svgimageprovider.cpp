#include <QFile>
#include <QPainter>

#include "qtnanosvg.hpp"

#include "nanosvg/src/nanosvg.h"

#include "svgimageprovider.hpp"

//////////////////////////////////////////////////////////////////////////////
SVGImageProvider::SVGImageProvider():
  QQuickImageProvider(QQmlImageProviderBase::Pixmap)
{
}

//////////////////////////////////////////////////////////////////////////////
QPixmap SVGImageProvider::requestPixmap(QString const& id, QSize* const sz,
  QSize const& rs)
{
  QPixmap pixmap(*sz = rs);

  //
  if (QFile f(id); !rs.isEmpty() && f.open(QIODevice::ReadOnly))
  {
    if (auto const sz(f.size()); sz)
    {
      if (char tmp[sz + 1]; f.read(tmp, sz) == sz)
      {
        tmp[sz] = {};

        if (auto const nsi(nsvgParse(tmp, "px", 96)); nsi)
        {
          pixmap.fill(Qt::transparent);

          {
            QPainter p(&pixmap);
            p.setRenderHint(QPainter::Antialiasing, true);

            drawSVGImage(&p, nsi, rs.width(), rs.height());
          }

          nsvgDelete(nsi);
        }
      }
    }
  }

  return pixmap;
}
