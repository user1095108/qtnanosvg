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
  QByteArray ba;

  if (QFile f(id); !rs.isEmpty() && f.open(QIODevice::ReadOnly))
  {
    if (auto const sz(f.size()); sz)
    {
      ba.resize(sz + 1);

      if (sz != f.read(ba.data(), sz))
      {
        ba.clear();
      }
    }
  }

  //
  if (ba.size())
  {
    ba.back() = {};

    if (auto const nsi(nsvgParse(ba.data(), "dpi", 96)); nsi)
    {
      pixmap.fill(Qt::transparent);

      QPainter p(&pixmap);
      p.setRenderHint(QPainter::Antialiasing, true);

      drawSVGImage(&p, nsi, 0, 0, rs.width(), rs.height());

      nsvgDelete(nsi);
    }
  }

  return pixmap;
}
