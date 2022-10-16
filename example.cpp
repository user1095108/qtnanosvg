#include <QtQml>
#include <QtQuick>

#include "svgimageprovider.hpp"

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);

  //
  QQmlApplicationEngine engine;
  engine.addImageProvider(QStringLiteral("svg"), new SVGImageProvider);
  engine.load(QStringLiteral("qrc:///Main.qml"));

  //
  return app.exec();
}
