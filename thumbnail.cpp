#include "thumbnail.h"

#include <qpa/qplatformnativeinterface.h>
#include <windows.h>
#include <winuser.h>
#include <QGuiApplication>
#include <QtWinExtras/QtWin>
#include <QDebug>
#include <QPainter>

#include <qwindow.h>

QWindow* Thumbnail::m_qwindow = nullptr;

Thumbnail::Thumbnail(QQuickItem *parent) :
  QQuickPaintedItem(parent),
  m_window(nullptr)
{
//  connect(this, &QQuickItem::xChanged, this, &Thumbnail::updatePosition);
//  connect(this, &QQuickItem::yChanged, this, &Thumbnail::updatePosition);
//  connect(this, &Thumbnail::windowChanged, this, &Thumbnail::updatePosition);

  setFlag(QQuickItem::ItemHasContents);
}

//void Thumbnail::updatePosition()
//{
//  HRESULT res = S_OK;
//  if(m_window->thumbnailId == 0 && m_qwindow) {
//    HWND hwnd = (HWND)QGuiApplication::platformNativeInterface()->nativeResourceForWindow(QByteArrayLiteral("handle"), m_qwindow);
//    res = DwmRegisterThumbnail(hwnd, m_window->handle(), &m_window->thumbnailId);
//    //    qWarning() << "registering" << m_window->m_title;
//  }
//  if(res == S_OK) {
//    SIZE size;
//    DwmQueryThumbnailSourceSize(m_window->thumbnailId, &size);

//    DWM_THUMBNAIL_PROPERTIES properties;
//    properties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION | DWM_TNP_OPACITY;
//    properties.fVisible = true;
//    properties.opacity = 0xFF;//(byte)opacity.Value;

//    QPointF p = mapToScene(QPointF(0, 0));
//    //    qWarning() << pos() << " => " << p;
//    properties.rcDestination.left = p.x();
//    properties.rcDestination.top = p.y();
//    properties.rcDestination.right = p.x() + boundingRect().width();
//    properties.rcDestination.bottom = p.y() + boundingRect().height();
//    if (size.cx < boundingRect().width())
//      properties.rcDestination.right = properties.rcDestination.left + size.cx;
//    if (size.cy < boundingRect().height())
//      properties.rcDestination.bottom = properties.rcDestination.top + size.cy;
//    DwmUpdateThumbnailProperties(m_window->thumbnailId, &properties);
//  }
//  else {
//    qWarning() << QtWin::errorStringFromHresult(res);
//  }
//}

void Thumbnail::paint(QPainter *painter)
{
  if(m_window->handle())
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

    QImage texture = QtWin::imageFromHBITMAP(hdcMem, hbitmap, width, height);
    QImage scaledTexture = texture.scaled(this->width(), this->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    painter->drawImage(0, 0, scaledTexture);

    DeleteObject(hbitmap);

    DeleteObject(hdcMem);
    ReleaseDC(m_window->handle(), hdc);
  }
}

//QSGNode *Thumbnail::updatePaintNode(QSGNode *parent, QQuickItem::UpdatePaintNodeData *)
//{
//  return parent;
//}
