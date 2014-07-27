#include <QApplication>
#include <qwindow.h>
#include <QDebug>

#include <qmessagebox.h>
#include <QtQml>
#include <QQmlApplicationEngine>
#include "thumbnail.h"
#include "windowmodel.h"
#include "xboxcontrolleritem.h"
#include "desktopactions.h"

#include <iostream>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  qmlRegisterType<Thumbnail>("windowing", 1, 0, "Thumbnail");
  qmlRegisterType<Window>("windowing", 1, 0, "Window");
  qmlRegisterType<WindowModel>("windowing", 1, 0, "WindowList");
  qmlRegisterType<DesktopActions>("windowing", 1, 0, "DesktopActions");
  qmlRegisterType<XBoxControllerItem>("windowing", 1, 0, "Controller");

  WindowModel* windowModel = new WindowModel();

  QQmlApplicationEngine engine;
  QVariant v;
  v.setValue(windowModel);
  engine.rootContext()->setContextProperty("windowList", v);

  QVariantList v2;
  for(Window* w : windowModel->pinnedWindows())
  {
    QVariant temp;
    temp.setValue(w);
    v2 << temp;
  }
  engine.rootContext()->setContextProperty("pinnedApplications", v2);

  engine.addImageProvider(QLatin1String("icons"), new WindowIconImageProvider);

  engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

  QList<QObject*> objs = engine.rootObjects();
  if(objs.length() > 0)
  {
    Thumbnail::m_qwindow = qobject_cast<QWindow*>(objs[0]);
  }
  else
  {
    QMessageBox::information(NULL, "DAMN", "no root objects");
    qWarning() << "DAMN";
    return 0;
  }

  return app.exec();
}
