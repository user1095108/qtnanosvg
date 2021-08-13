#ifndef SVGIMAGEPROVIDER_HPP
# define SVGIMAGEPROVIDER_HPP
# pragma once

#include <QQuickImageProvider>

class SVGImageProvider final: public QQuickImageProvider
{
public:
  explicit SVGImageProvider();

  QPixmap requestPixmap(QString const&, QSize*, QSize const&) final;
};

#endif // SVGIMAGEPROVIDER_HPP
