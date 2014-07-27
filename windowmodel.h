#ifndef WINDOWMODEL_H
#define WINDOWMODEL_H

#include <functional>

#include <QAbstractListModel>
#include <QQuickImageProvider>
#include <QtWinExtras/QtWin>

#include <windows.h>
#include <dwmapi.h>
#include <commoncontrols.h>

#include "window.h"

class WindowIconImageProvider;
class WindowComparator;

class WindowModel : public QAbstractListModel {
  friend class WindowIconImageProvider;
  friend class WindowComparator;
  Q_OBJECT
public:

  QList<Window*> pinnedWindows();
  static QString resolveLink(const QString linkPath);

  WindowModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  virtual QHash<int,QByteArray> roleNames() const;

  enum WindowRoles {
    TitleRole = Qt::UserRole + 1,
    HandleRole,
    IconRole,
    IconPathRole,
    IdentifierRole
  };

  Window* WindowModel::getWindowByExecutableName(const QString executableName);
  static QString executableName(HWND handle);

  int indexOfWindow(const QString& executableName);
  Window* setActiveWindow(HWND handle);

  Window* addWindow(HWND handle);
  Window* addWindow(Window* w);
  void removeWindow(HWND handle);

  static void focusWindow(HWND);

public slots:
  void focusAndUpdate();

private:
  QList<Window*> m_windows;

  static const std::function<bool(Window *first, Window *second)> WindowComparator;
};

class WindowIconImageProvider : public QQuickImageProvider
{
public:
  WindowIconImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
  {
  }

  QPixmap requestPixmap(const QString &id, QSize *size, const QSize &)
  {
    if (size)
      *size = QSize(50, 50);


    SHFILEINFO info;
    DWORD_PTR p = SHGetFileInfoW(QUrl::fromPercentEncoding(id.toUtf8()).toStdWString().c_str(),
                                 0,
                                 &info,
                                 sizeof(SHFILEINFO),
                                 SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_LARGEICON);

    // Get the jumbo image list
    IImageList *imageList;
    if (p && SUCCEEDED(SHGetImageList(SHIL_JUMBO, IID_PPV_ARGS(&imageList))))
    {
      // Extract an icon
      HICON hico;
      imageList->GetIcon(info.iIcon, ILD_TRANSPARENT, &hico);

      // Clean up
      imageList->Release();

      // Return the icon
      QPixmap icon = QtWin::fromHICON(hico);
      return icon;
    }
    else
    {
      QPixmap icon = QtWin::fromHICON(info.hIcon);
      return icon;
    }
  }
};

Q_DECLARE_METATYPE(WindowModel*)
Q_DECLARE_METATYPE(QList<Window*>)

#endif // WINDOWMODEL_H
