#include <QFile>
#include <QPainter>

#include "nanosvg/src/nanosvg.h"
#include "qtnanosvg.hpp"

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
  QPixmap pm(*sz = rs);

  do if (!pm.isNull())
  {
    if (QFile f(id); f.open(QIODevice::ReadOnly))
    {
      auto const fsz(f.size());

      if (0 >= fsz) break;

      if (char dat[fsz + 1]; fsz == f.read(dat, fsz))
      {
        dat[fsz] = {};

        if (auto const nsi(nsvgParse(dat, "px", 96)); nsi)
        {
          pm.fill(Qt::transparent);

          {
            QPainter p(&pm);
            p.setRenderHint(QPainter::Antialiasing);

            drawSVGImage(&p, nsi, rs.width(), rs.height());
          }

          nsvgDelete(nsi);
        }
      }
    }
  } while (false);

  return pm;
}
