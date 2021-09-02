TEMPLATE = app

CONFIG += no_lflags_merge exceptions_off stl thread warn_on c++latest strict_c++ qt qtquickcompiler
CONFIG -= exceptions

QT += qml quick

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES +=                 \
  QT_USE_FAST_CONCATENATION\
  QT_USE_FAST_OPERATOR_PLUS\
  QT_NO_CAST_FROM_ASCII    \
  QT_NO_CAST_TO_ASCII      \
  QT_NO_CAST_FROM_BYTEARRAY

RESOURCES = qml.qrc

HEADERS += qtnanosvg.hpp
SOURCES += qtnanosvg.cpp \
           svgimageprovider.cpp \
           example.cpp

*-g++* {
  QMAKE_CFLAGS = -pedantic -Wall -Wextra -fno-stack-protector -fno-plt

  unix:QMAKE_CXXFLAGS_DEBUG *= -fsanitize=address,undefined
  unix:QMAKE_LFLAGS_DEBUG *= -fsanitize=address,undefined

  QMAKE_CFLAGS_RELEASE *= -Ofast -DNDEBUG
  QMAKE_CXXFLAGS_RELEASE *= -Ofast -DNDEBUG -fno-stack-protector -fno-plt

  QMAKE_LFLAGS *= -fno-stack-protector -fuse-ld=gold
}
