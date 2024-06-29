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

  while (!pm.isNull())
  {
    QByteArray dat;

    if (QFile f(id);
      f.open(QIODevice::ReadOnly)) dat = f.readAll(); else break;

    if (auto const nsi(nsvgParse(dat.data(), "px", 96)); nsi)
    {
      pm.fill(Qt::transparent);

      {
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);

        drawSVGImage(&p, nsi, rs.width(), rs.height());
      }

      nsvgDelete(nsi);
    }

    break;
  }

  return pm;
}
