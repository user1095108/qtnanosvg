#include <QtQml>
#include <QtQuick>

#include "svgimageprovider.hpp"

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);

  //
  QQmlApplicationEngine eng;
  eng.addImageProvider(QStringLiteral("svg"), new SVGImageProvider);
  eng.load(QStringLiteral("qrc:///Main.qml"));

  //
  return app.exec();
}
