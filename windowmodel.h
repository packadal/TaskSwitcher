#ifndef WINDOWMODEL_H
#define WINDOWMODEL_H

#include <QDebug>
#include <QAbstractListModel>
#include <QQuickImageProvider>
#include <QQmlListProperty>
#include <QtWinExtras/QtWin>

#include <windows.h>
#include <winuser.h>
#include <dwmapi.h>
#include <commoncontrols.h>
#include <shellapi.h>

class WindowIconImageProvider;
class WindowComparator;
class Window;

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
};

class Window : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString title READ title NOTIFY titleChanged)
  Q_PROPERTY(QString iconPath READ iconPath NOTIFY iconPathChanged)
public:
  Window()
    : thumbnailId(nullptr),
      m_handle(nullptr),
      m_title(QString()),
      m_executablePath(QString()),
      m_executionCount(0),
      m_focusTime(0)
  {
  }

  Window(const QString& executablePath,
         const unsigned long executionCount,
         const unsigned int focusTime
         )
    : thumbnailId(nullptr),
      m_handle(nullptr),
      m_title(QString()),
      m_executablePath(executablePath),
      m_executionCount(executionCount),
      m_focusTime(focusTime)
  {
    int lastIndex = executablePath.lastIndexOf("\\") + 1;
    if(lastIndex >= 0)
      m_title = executablePath.mid(lastIndex, executablePath.length() - lastIndex - 4);
  }

  ~Window() {
  }

  Window(const Window& other) {
    m_handle = other.m_handle;
    thumbnailId = other.thumbnailId;
    m_title = other.m_title;
  }

  Window(HWND handle) :
    thumbnailId(0),
    m_handle(handle)
  {
    const int maxTitleLength = 50;
    wchar_t title[maxTitleLength];
    GetWindowText(handle, title, maxTitleLength);
    m_title = QString::fromWCharArray(title);

    m_executablePath = WindowModel::executableName(handle);
  }

  void setRunning(HWND handle)
  {
    m_handle = handle;
    m_executionCount += 1000;
  }

  HWND handle() const {
    return m_handle;
  }

  QString title() const {
    return m_title;
  }

  bool operator ==(const Window& other) {
    return other.m_handle == this->m_handle;
  }

  QString executableName() const
  {
    return m_executablePath;
  }

  unsigned long executionCount() const
  {
    return m_executionCount;
  }

  void setExecutionCount(unsigned long newValue)
  {
    m_executionCount = newValue;
  }

  QString iconPath() const
  {
    return "image://icons/" + m_executablePath;
  }

  HTHUMBNAIL thumbnailId;

  unsigned int focusTime() const {
    return m_focusTime;
  }

public slots:
  bool isRunning() const {
    return m_handle != nullptr;
  }

  void start();

  void setFocus();

  void close();

signals:
  void titleChanged();
  void iconPathChanged();

private:
  QString m_title;
  HWND m_handle;
  QString m_executablePath;
  unsigned long m_executionCount;
  unsigned int m_focusTime;
};

Q_DECLARE_METATYPE(Window)

class WindowComparator
{
public:
  WindowComparator(WindowModel* model)
    : m_model(model)
  {

  }
  bool operator ()(Window *first, Window *second)
  {
      return first->executionCount() > second->executionCount();
  }

private:
  WindowModel* m_model;
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
