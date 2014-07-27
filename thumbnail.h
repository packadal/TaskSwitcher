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

public slots:
  void paint(QPainter *painter);

  void updateThumbnail();

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
  QImage m_image;
  Window* m_window;
};

#endif // THUMBNAIL_H
