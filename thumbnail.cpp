#include "thumbnail.h"

#include <qpa/qplatformnativeinterface.h>
#include <windows.h>
#include <winuser.h>
#include <QGuiApplication>
#include <QtWinExtras/QtWin>
#include <QDebug>
#include <QPainter>

#include <qwindow.h>

Thumbnail::Thumbnail(QQuickItem *parent) :
  QQuickPaintedItem(parent),
  m_window(nullptr)
{
  setFlag(QQuickItem::ItemHasContents);
}

void Thumbnail::paint(QPainter *painter)
{
  if(m_window->handle())
  {
    if(m_image.isNull()) {
      updateThumbnail();
    }

    QImage scaledTexture = m_image.scaled(this->width(), this->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    painter->drawImage(0, 0, scaledTexture);
  }
}

void Thumbnail::updateThumbnail()
{
  RECT rc;
  GetWindowRect(m_window->handle(), &rc);
  HDC hdc = GetWindowDC(m_window->handle());

  HDC hdcMem = CreateCompatibleDC(hdc);

  const int width = rc.right - rc.left;
  const int height = rc.bottom - rc.top;
  HBITMAP hbitmap = CreateCompatibleBitmap(hdc, width, height);

  SelectObject(hdcMem, hbitmap);
  PrintWindow(m_window->handle(), hdcMem, 0);

  m_image = QtWin::imageFromHBITMAP(hdcMem, hbitmap, width, height);

  DeleteObject(hbitmap);

  DeleteObject(hdcMem);
  ReleaseDC(m_window->handle(), hdc);
}
