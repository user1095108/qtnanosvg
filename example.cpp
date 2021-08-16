#include <QtQml>
#include <QtQuick>

#include "svgimageprovider.hpp"

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);

  //
  QScopedPointer<QQmlEngine> engine(new QQmlEngine);

  engine->addImageProvider(QStringLiteral("svg"), new SVGImageProvider);

  QQmlComponent c(engine.get(), QStringLiteral("qrc:///Main.qml"));

  return c.create() ? app.exec() : (qDebug() << c.errors(), EXIT_FAILURE);
}
