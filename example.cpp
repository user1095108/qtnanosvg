#include <QtQml>
#include <QtQuick>

#include "svgimageprovider.hpp"

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);

  //
  QScopedPointer<QQmlEngine> engine(new QQmlEngine);
  QObject::connect(engine.get(), &QQmlEngine::quit, &app,
    &QCoreApplication::quit);

  engine->addImageProvider(QStringLiteral("svg"), new SVGImageProvider);

  QQmlComponent c(engine.get(), QStringLiteral("qrc:///Main.qml"));

  auto const obj(c.create());

  return obj ? app.exec() : (qDebug() << c.errors(), EXIT_FAILURE);
}
