#include "windowmodel.h"
#include <QDebug>
#include <QSettings>
#include <Windows.h>
#include <QPixmap>
#include <QSettings>
#include <QDir>
#include <QtWinExtras/QtWin>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#include <qdatetime.h>

#include "thumbnail.h"
#include "Psapi.h"
#include "Shlobj.h"

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")

static int indexIndirection[9] = {5, 3, 7, 1, 0, 2, 6, 4, 8};

BOOL IsAltTabWindow(HWND hwnd)
{
 // Start at the root owner
 HWND hwndWalk = GetAncestor(hwnd, GA_ROOTOWNER);

// const long TARGETWINDOW  = WS_BORDER | WS_VISIBLE;
 if(!IsWindow(hwnd) ||
    !IsWindowVisible(hwnd) ||
//    (GetWindowLongA(hwnd, GWL_STYLE) & TARGETWINDOW) != TARGETWINDOW ||
    (GetWindowLongA(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW) {
   return FALSE;
 }

 // See if we are the last active visible popup
 HWND hwndTry;
 while ((hwndTry = GetLastActivePopup(hwndWalk)) != hwndTry) {
  if (IsWindowVisible(hwndTry)) break;
  hwndWalk = hwndTry;
 }
 return hwndWalk == hwnd;
}

BOOL CALLBACK Callback(HWND hwnd, long int windowModelPointer)
{
//  const long TARGETWINDOW  = WS_BORDER | WS_VISIBLE;
//  if(IsWindow(hwnd) && (GetWindowLongA(hwnd, GWL_STYLE) & TARGETWINDOW) == TARGETWINDOW)
  if(IsAltTabWindow(hwnd))
  {
    WindowModel* model = (WindowModel*)windowModelPointer;
    model->addWindow(hwnd);
  }

  return true; //continue enumeration
}

//VOID CALLBACK WinEventProcCallback( HWINEVENTHOOK /*hWinEventHook*/,
//                                    DWORD event,
//                                    HWND hwnd,
//                                    LONG /*idObject*/,
//                                    LONG /*idChild*/,
//                                    DWORD /*dwEventThread*/,
//                                    DWORD /*dwmsEventTime*/ )
//{
//  if ( !hwnd || !IsWindow( hwnd ) ) {
//    return;
//  }
//  const long TARGETWINDOW  = WS_BORDER | WS_VISIBLE;
//  if ( event == EVENT_SYSTEM_FOREGROUND  && (GetWindowLongA(hwnd, GWL_STYLE) & TARGETWINDOW) == TARGETWINDOW)
//  {
////    windowModel->setActiveWindow(hwnd);
//  }
//}

QString rot13(const QString cypher)
{
  const ushort offset = 13;
  QString result;
  for(int i = 0; i < cypher.size(); ++i)
  {
    //uppercase
    char current = cypher.at(i).toLatin1();
    if(current >= 'A' && current <= 'Z' )
    {
      if(current - 'A' < offset)
        current += offset;
      else
        current -= offset;
    }
    //lowercase
    if(current >= 'a' && current <= 'z' )
    {
      if(current - 'a' < offset)
        current += offset;
      else
        current -= offset;

    }
    result.append(QChar(current));
  }

  return result;
}

QList<Window *> WindowModel::pinnedWindows()
{
  QList<Window*> result;

  IKnownFolder *folder = NULL;
  IKnownFolderManager* mgr = NULL;
  CoCreateInstance(CLSID_KnownFolderManager,
                   NULL,
                   CLSCTX_INPROC_SERVER,
                   IID_PPV_ARGS(&mgr));

  mgr->GetFolder( FOLDERID_UserPinned, &folder );

  wchar_t *outName = NULL;
  folder->GetPath(0, &outName);
  const QString path(QString::fromWCharArray(outName));

  QDir d(path + "/Taskbar");
  for(QFileInfo entry : d.entryInfoList(QDir::Files))
  {
    QString exePath = resolveLink(entry.absoluteFilePath());

    int index = indexOfWindow(exePath);
    Window* w = nullptr;
    if(index > 0 && index < m_windows.length())
      w = m_windows[index];
    else
      w = new Window(exePath, 0, 0);

    result << w;
  }

  return result;
}

QString WindowModel::resolveLink(const QString linkPath)
{
  IShellLink* psl = NULL;
  HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
  IPersistFile* ppf = NULL;
  hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
  hres = ppf->Load(linkPath.toStdWString().c_str(), STGM_READ);
  hres = psl->Resolve(NULL, 0);
  TCHAR szGotPath[MAX_PATH] = {0};
  hres = psl->GetPath(szGotPath, _countof(szGotPath), NULL, SLGP_SHORTPATH);

  TCHAR szLongPath[MAX_PATH] = {0};
  GetLongPathName(szGotPath, szLongPath, MAX_PATH);

  return QString::fromWCharArray(szLongPath);
}

WindowModel::WindowModel()
{
  QMap<QString, QString> knownFolders;

  KNOWNFOLDERID *folders = NULL;
  UINT pCount = 0;
  IKnownFolderManager* mgr = NULL;
  CoCreateInstance(CLSID_KnownFolderManager,
                   NULL,
                   CLSCTX_INPROC_SERVER,
                   IID_PPV_ARGS(&mgr));

  mgr->GetFolderIds(&folders, &pCount);
  for(unsigned int i = 0; i < pCount; ++i)
  {
    IKnownFolder *folder = NULL;
    mgr->GetFolder( folders[i], &folder );

    KF_CATEGORY cat;
    folder->GetCategory(&cat);
    if(cat == KF_CATEGORY_VIRTUAL) //skip some folders
      continue;

    wchar_t *outGUID = new wchar_t[40];
    StringFromGUID2(folders[i], outGUID, 40);
    QString guid(QString::fromWCharArray(outGUID));

    wchar_t *outName = NULL;
    folder->GetPath(0, &outName);
    const QString path(QString::fromWCharArray(outName));

    if(guid.isEmpty() || path.isEmpty())
    {
//      qWarning() << "ERROR: " << guid << "; " << path;
      continue;
    }

    knownFolders.insert(guid, path);
  }

  const QString executablesRegistryGroup = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\{CEBFF5CD-ACE2-4F4F-9178-9926F41749EA}\\Count";
  const QString shortcutsRegistryGroup = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\{F4E57C4B-2036-45F0-A9AB-443BCFE33D9F}\\Count";

  HKEY rootKey;
  RegOpenKeyEx(HKEY_CURRENT_USER,
               shortcutsRegistryGroup.toStdWString().c_str(),
               0,
               KEY_ALL_ACCESS ,
               &rootKey);
  int keyindex = 0;
  LRESULT subKeyError = ERROR_SUCCESS;
  while(subKeyError == ERROR_SUCCESS)
  {
    DWORD nameLength = 512;
    WCHAR nameBuffer[512];
    DWORD length = 4096;
    DWORD type = REG_BINARY;
    unsigned char value[4096];
    subKeyError = RegEnumValue(rootKey,
                               keyindex,
                               nameBuffer,
                               &nameLength,
                               NULL,
                               &type,
                               value,
                               &length);

    QString b = QString::fromWCharArray(nameBuffer);
    UINT32 executionCount = *((UINT32*)(value + 4));
    UINT32 focusTime = *((UINT32*)(value + 12));
//    UINT64 windate =*((UINT64*)(value + 60));
//    UINT64 no_nano = windate/10000000.;
//    UINT64 unixDate = no_nano - 11644473600;
//    QDateTime d = QDateTime::fromMSecsSinceEpoch(unixDate);
//    qWarning() <<windate << "; " << no_nano << "; " << unixDate << "; "<<  d;

    keyindex++;

    QString realName = rot13(b);
    if(realName.startsWith("{"))
    {
      QString folderId = realName.left(realName.indexOf("}")+1);
      realName = realName.replace(folderId, knownFolders[folderId]);
    }

    IShellLink* psl = NULL;
    HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    IPersistFile* ppf = NULL;
    hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
    hres = ppf->Load(realName.toStdWString().c_str(), STGM_READ);
    hres = psl->Resolve(NULL, 0);
    TCHAR szGotPath[MAX_PATH] = {0};
    hres = psl->GetPath(szGotPath, _countof(szGotPath), NULL, SLGP_SHORTPATH);
    QString target = QString::fromWCharArray(szGotPath);
    if(!target.isEmpty())
      realName = target;

    GetLongPathName(target.toStdWString().c_str(), szGotPath, MAX_PATH);
    realName = QString::fromStdWString(szGotPath);

    if(realName.isEmpty())
      continue;

    //avoid duplicates, or deleted files
    if(indexOfWindow(realName) < 0 && QFile::exists(realName)) {
      Window* w = new Window(realName, executionCount, focusTime);
      addWindow(w);
    }
  }

  EnumWindows(Callback, (long int)this);

  //no need to monitor new window creation for now
//  SetWinEventHook( EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL,
//                                                   WinEventProcCallback, 0, 0,
//                                                   WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS );
}

int WindowModel::rowCount(const QModelIndex &) const
{
  return 9;
}

int WindowModel::columnCount(const QModelIndex &) const
{
  return 1;
}

QVariant WindowModel::data(const QModelIndex &index, int role) const
{
  QVariant v;
  int currentIndex = index.row();
  while(indexIndirection[currentIndex] >= m_windows.length())
    currentIndex--;

  Window* w = m_windows[indexIndirection[currentIndex]];

  if(role == TitleRole)
    v.setValue(w->title());
  else if(role == IconPathRole)
  {
    v.setValue(w->iconPath());
  }
  else if(role == HandleRole)
    v.setValue(w);

  return v;
}

QHash<int, QByteArray> WindowModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[TitleRole] = "title";
  roles[HandleRole] = "handle";
  roles[IconPathRole] = "icon";

  return roles;
}

QString WindowModel::executableName(HWND handle)
{
  TCHAR fileName[MAX_PATH] = {0};
  DWORD pid;
  GetWindowThreadProcessId(handle, &pid);
  HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  GetModuleFileNameEx(processHandle, NULL, fileName, MAX_PATH);
  CloseHandle(processHandle);

  return QString::fromStdWString(fileName);
}

int WindowModel::indexOfWindow(const QString& executableName)
{
  for(int i = 0; i < m_windows.length(); ++i) {
    if(m_windows[i]->executableName() == executableName) {
      return i;
    }
  }
  return -1;
}

Window *WindowModel::setActiveWindow(HWND handle)
{
  addWindow(handle);
  int index = indexOfWindow(executableName(handle));

  Window* temp = m_windows[0];
  m_windows[0] = m_windows[index];
  m_windows[index] = temp;
  return m_windows[0];
}

Window* WindowModel::addWindow(Window* w)
{
  m_windows.append(w);
  WindowComparator c(this);
  qSort(m_windows.begin(), m_windows.end(), c);
  return w;
}

Window *WindowModel::addWindow(HWND handle)
{
  WindowComparator c(this);

  int index = indexOfWindow(executableName(handle));
  if(index >= 0 && index < m_windows.length())
  {
    if(!m_windows[index]->isRunning())
    {
      m_windows[index]->setRunning(handle);
    }
    return m_windows[index];
  }

  Window* w = new Window(handle);
  w->setRunning(handle);
  m_windows.append(w);
  qSort(m_windows.begin(), m_windows.end(), c);

  return w;
}

void WindowModel::removeWindow(HWND handle)
{
  for(Window* w : m_windows) {
    if(w->handle() == handle) {
      m_windows.removeOne(w);
      return;
    }
  }
}

void WindowModel::focusAndUpdate()
{
//  if(Thumbnail::m_qwindow)
//  {
//    HWND hwnd = (HWND)QGuiApplication::platformNativeInterface()->nativeResourceForWindow(QByteArrayLiteral("handle"), Thumbnail::m_qwindow);
//    focusWindow(hwnd);
//  }
  EnumWindows(Callback, (long int)this);
}

void WindowModel::focusWindow(HWND handle)
{
  HWND currentForegroundWindow = GetForegroundWindow();
  DWORD pid;
  GetWindowThreadProcessId(currentForegroundWindow, &pid);

  DWORD myPid;
  GetWindowThreadProcessId(handle, &myPid);

  if(pid != myPid)
  {
    AttachThreadInput(myPid, pid, TRUE);
    AllowSetForegroundWindow(ASFW_ANY);

    AttachThreadInput(myPid, pid, FALSE);
  }

  ShowWindow(handle, SW_SHOWMAXIMIZED );
  SetForegroundWindow(handle);
}

void Window::start()
{
  if(isRunning())
    return;

  QString operation("open");
  ShellExecuteW(NULL,
               operation.toStdWString().c_str(),
               m_executablePath.toStdWString().c_str(),
               NULL,
               NULL,
                SW_SHOWDEFAULT);
}

void Window::setFocus()
{
  WindowModel::focusWindow(m_handle);
}

void Window::close()
{
  SendMessage(m_handle, WM_CLOSE, 0, 0);
}
