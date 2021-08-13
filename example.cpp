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

  if (auto const obj(c.create()); obj)
  {
    return app.exec();
  }
  else
  {
    qDebug() << c.errors();
  }

  return EXIT_FAILURE;
}
