TEMPLATE = app

CONFIG += no_lflags_merge exceptions_off stl thread warn_on c++17 strict_c++ qt qtquickcompiler

QT += qml quick

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000\
           TBB_USE_EXCEPTIONS=0\
           _GLIBCXX_USE_TBB_PAR_BACKEND=0

DEFINES +=                      \
  QT_USE_FAST_CONCATENATION     \
  QT_USE_FAST_OPERATOR_PLUS     \
  QT_NO_CAST_FROM_ASCII         \
  QT_NO_CAST_TO_ASCII           \
  QT_NO_CAST_FROM_BYTEARRAY

RESOURCES = qml.qrc

HEADERS += qtnanosvg.hpp
SOURCES += qtnanosvg.cpp \
           svgimageprovider.cpp \
           example.cpp

*-g++* {
  QMAKE_CXXFLAGS_DEBUG *= -fsanitize=address,undefined
  QMAKE_LFLAGS_DEBUG *= -fsanitize=address,undefined

  QMAKE_CFLAGS_RELEASE *= -march=native -Ofast -DNDEBUG
  QMAKE_CXXFLAGS_RELEASE *= -march=native -Ofast -DNDEBUG -DQT_NO_DEBUG_OUTPUT -fno-plt -fno-stack-protector -g

  QMAKE_LFLAGS *= -fno-stack-protector
  QMAKE_LFLAGS_RELEASE -= -s
}
