#ifndef DESKTOPACTIONS_H
#define DESKTOPACTIONS_H

#include <QQuickItem>

class DesktopActions : public QQuickItem
{
  Q_OBJECT
public:
  explicit DesktopActions(QQuickItem *parent = 0);

signals:

public slots:
  void shutdown();
  void restart();
};

#endif // DESKTOPACTIONS_H
