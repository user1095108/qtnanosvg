#if defined(_WIN32)
# include <malloc.h>
# define SIP_ALLOCA(x) _alloca(x)
#else
# include <stdlib.h>
# define SIP_ALLOCA(x) alloca(x)
#endif // SIP_ALLOCA

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

  do if(!pm.isNull())
  {
    char* dat;

    if (QFile f(id); f.open(QIODevice::ReadOnly))
    {
      if (auto const fsz(f.size());
        fsz == f.read(dat = static_cast<char*>(SIP_ALLOCA(fsz + 1)), fsz))
        dat[fsz] = {}; else break;
    }
    else break;

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
  } while (false);

  return pm;
}
