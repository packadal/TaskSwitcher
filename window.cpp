#include "window.h"

#include "windowmodel.h"

Window::Window()
  : thumbnailId(nullptr),
    m_handle(nullptr),
    m_title(QString()),
    m_executablePath(QString()),
    m_executionCount(0),
    m_focusTime(0)
{
}

Window::Window(const QString &executablePath, const unsigned long executionCount, const unsigned int focusTime)
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

Window::Window(const Window &other)
{
  m_handle = other.m_handle;
  thumbnailId = other.thumbnailId;
  m_title = other.m_title;
}

Window::Window(HWND handle)
  :thumbnailId(0),
    m_handle(handle)
{
  const int maxTitleLength = 50;
  wchar_t title[maxTitleLength];
  GetWindowText(handle, title, maxTitleLength);
  m_title = QString::fromWCharArray(title);

  m_executablePath = WindowModel::executableName(handle);
}

void Window::setRunning(HWND handle)
{
  m_handle = handle;
  m_executionCount += 1000;
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
