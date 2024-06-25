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

  if (!pm.isNull())
  {
    uchar* dat;

    if (QFile f(id); f.open(QIODevice::ReadOnly) &&
      (dat = f.map({}, f.size(), QFileDevice::MapPrivateOption)))
    {
      dat[f.size() - 1] = {};

      if (auto const nsi(nsvgParse(
        reinterpret_cast<char*>(dat), "px", 96)); nsi)
      {
        pm.fill(Qt::transparent);

        {
          QPainter p(&pm);
          p.setRenderHint(QPainter::Antialiasing);

          drawSVGImage(&p, nsi, rs.width(), rs.height());
        }

        nsvgDelete(nsi);
        f.unmap(dat);
      }
    }
  }

  return pm;
}
