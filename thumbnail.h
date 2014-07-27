#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QGraphicsRectItem>
#include <QGraphicsItem>

#include <QQuickPaintedItem>

#include "windowmodel.h"

class Thumbnail : public QQuickPaintedItem
{
  Q_OBJECT
  Q_PROPERTY(Window* window READ window WRITE setWindow NOTIFY windowChanged)
public:
  explicit Thumbnail(QQuickItem *parent = 0);

  static QWindow* m_qwindow;

public slots:
  void paint(QPainter *painter);
//  QSGNode* updatePaintNode(QSGNode *parent, UpdatePaintNodeData *);

  Window* window() const {
    return m_window;
  }

  void setWindow(Window* const window) {
    if(m_window != window) {
      m_window = window;
      emit(windowChanged());
    }
  }

signals:
  void windowChanged();

private:
  Window* m_window;
};

#endif // THUMBNAIL_H
