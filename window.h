#ifndef WINDOW_H
#define WINDOW_H

#include <QObject>

#include <dwmapi.h>

class Window : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString title READ title NOTIFY titleChanged)
  Q_PROPERTY(QString iconPath READ iconPath NOTIFY iconPathChanged)
  Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningChanged)
public:
  Window();

  Window(const QString& executablePath,
         const unsigned long executionCount,
         const unsigned int focusTime
         );

  ~Window() {
  }

  Window(const Window& other);

  Window(HWND handle);

  void setRunning(HWND handle);

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

  bool isRunning() const {
    return m_handle != nullptr;
  }

public slots:

  void start();

  void setFocus();

  void close();

signals:
  void titleChanged();
  void iconPathChanged();
  void runningChanged();

private:
  QString m_title;
  HWND m_handle;
  QString m_executablePath;
  unsigned long m_executionCount;
  unsigned int m_focusTime;
};

Q_DECLARE_METATYPE(Window)

#endif // WINDOW_H
