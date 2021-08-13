#ifndef QTNANOSVG_HPP
# define QTNANOSVG_HPP
# pragma once

#include <QtGlobal>

class QPainter;
struct NSVGimage;

void drawSVGImage(QPainter*, struct NSVGimage*, qreal, qreal);

#endif // QTNANOSVG_HPP
