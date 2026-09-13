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
    QByteArray dat;

    if (QFile f(id); f.open(QIODevice::ReadOnly))
      { if ((dat = f.readAll()).isEmpty()) break; } else break;

    if (auto const nsi = nsvgParse(dat.data(), "px", 96))
    {
      pm.fill(Qt::transparent);

      {
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);

        drawSVGImage(&p, nsi, rs.width(), rs.height());
      }

      nsvgDelete(nsi);
    }
  } while (false);

  return pm;
}
