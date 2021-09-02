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
QPixmap SVGImageProvider::requestPixmap(QString const& id, QSize* sz,
  QSize const& rs)
{
  QPixmap pixmap(*sz = rs);

  //
  if (QFile f(id); !rs.isEmpty() && f.open(QIODevice::ReadOnly))
  {
    if (auto const sz(f.size()); sz)
    {
      QByteArray ba;
      ba.resize(sz + 1);

      if (f.read(ba.data(), sz) == sz)
      {
        ba.back() = {};

        if (auto const nsi(nsvgParse(ba.data(), "px", 96)); nsi)
        {
          pixmap.fill(Qt::transparent);

          QPainter p(&pixmap);
          p.setRenderHint(QPainter::Antialiasing, true);

          drawSVGImage(&p, nsi, rs.width(), rs.height());

          nsvgDelete(nsi);
        }
      }
    }
  }

  return pixmap;
}
