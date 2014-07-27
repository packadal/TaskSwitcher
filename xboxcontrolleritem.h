#ifndef XBOXCONTROLLERITEM_H
#define XBOXCONTROLLERITEM_H

#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "XINPUT9_1_0.LIB")

#include <QQuickItem>

class XBoxControllerItem : public QQuickItem
{
  Q_OBJECT
public:
  explicit XBoxControllerItem(QQuickItem *parent = 0);

  void timerEvent(QTimerEvent* event);

private:
  bool m_a;
  Q_PROPERTY(bool a READ a WRITE seta NOTIFY aChanged)
public:
  bool a() const { return m_a; }
  void seta(const bool newValue) {
    if(m_a != newValue) {
      m_a = newValue;
      aChanged();
    }
  }
signals:
  void aChanged();

private:
  bool m_b;
  Q_PROPERTY(bool b READ b WRITE setb NOTIFY bChanged)
public:
  bool b() const { return m_b; }
  void setb(const bool newValue) {
    if(m_b != newValue) {
      m_b = newValue;
      bChanged();
    }
  }
signals:
  void bChanged();

private:
  bool m_x;
  Q_PROPERTY(bool x READ x WRITE setx NOTIFY xChanged)
public:
  bool x() const { return m_x; }
  void setx(const bool newValue) {
    if(m_x != newValue) {
      m_x = newValue;
      xChanged();
    }
  }
signals:
  void xChanged();

private:
  bool m_y;
  Q_PROPERTY(bool y READ y WRITE sety NOTIFY yChanged)
public:
  bool y() const { return m_y; }
  void sety(const bool newValue) {
    if(m_y != newValue) {
      m_y = newValue;
      yChanged();
    }
  }
signals:
  void yChanged();

private:
  bool m_lb;
  Q_PROPERTY(bool lb READ lb WRITE setlb NOTIFY lbChanged)
public:
  bool lb() const { return m_lb; }
  void setlb(const bool newValue) {
    if(m_lb != newValue) {
      m_lb = newValue;
      lbChanged();
    }
  }
signals:
  void lbChanged();

private:
  short m_lt;
  Q_PROPERTY(short lt READ lt WRITE setlt NOTIFY ltChanged)
public:
  short lt() const { return m_lt; }
  void setlt(const short newValue) {
    if(m_lt != newValue) {
      m_lt = newValue;
      ltChanged();
    }
  }
signals:
  void ltChanged();

private:
  bool m_rb;
  Q_PROPERTY(bool rb READ rb WRITE setrb NOTIFY rbChanged)
public:
  bool rb() const { return m_rb; }
  void setrb(const bool newValue) {
    if(m_rb != newValue) {
      m_rb = newValue;
      rbChanged();
    }
  }
signals:
  void rbChanged();

private:
  short m_rt;
  Q_PROPERTY(short rt READ rt WRITE setrt NOTIFY rtChanged)
public:
  short rt() const { return m_rt; }
  void setrt(const short newValue) {
    if(m_rt != newValue) {
      m_rt = newValue;
      rtChanged();
    }
  }
signals:
  void rtChanged();

private:
  bool m_back;
  Q_PROPERTY(bool back READ back WRITE setback NOTIFY backChanged)
public:
  bool back() const { return m_back; }
  void setback(const bool newValue) {
    if(m_back != newValue) {
      m_back = newValue;
      backChanged();
    }
  }
signals:
  void backChanged();

private:
  bool m_start;
  Q_PROPERTY(bool start READ start WRITE setstart NOTIFY startChanged)
public:
  bool start() const { return m_start; }
  void setstart(const bool newValue) {
    if(m_start != newValue) {
      m_start = newValue;
      startChanged();
    }
  }
signals:
  void startChanged();

private:
  short m_leftStickX;
  Q_PROPERTY(short leftStickX READ leftStickX WRITE setleftStickX NOTIFY leftStickXChanged)
public:
  short leftStickX() const { return m_leftStickX; }
  void setleftStickX(const short newValue) {
    if(m_leftStickX != newValue) {
      m_leftStickX = newValue;
      leftStickXChanged();
    }
  }
signals:
  void leftStickXChanged();

private:
  short m_leftStickY;
  Q_PROPERTY(short leftStickY READ leftStickY WRITE setleftStickY NOTIFY leftStickYChanged)
public:
  short leftStickY() const { return m_leftStickY; }
  void setleftStickY(const short newValue) {
    if(m_leftStickY != newValue) {
      m_leftStickY = newValue;
      leftStickYChanged();
    }
  }
signals:
  void leftStickYChanged();

private:
  short m_rightStickX;
  Q_PROPERTY(short rightStickX READ rightStickX WRITE setrightStickX NOTIFY rightStickXChanged)
public:
  short rightStickX() const { return m_rightStickX; }
  void setrightStickX(const short newValue) {
    if(m_rightStickX != newValue) {
      m_rightStickX = newValue;
      rightStickXChanged();
    }
  }
signals:
  void rightStickXChanged();

private:
  short m_rightStickY;
  Q_PROPERTY(short rightStickY READ rightStickY WRITE setrightStickY NOTIFY rightStickYChanged)
public:
  short rightStickY() const { return m_rightStickY; }
  void setrightStickY(const short newValue) {
    if(m_rightStickY != newValue) {
      m_rightStickY = newValue;
      rightStickYChanged();
    }
  }
signals:
  void rightStickYChanged();

private:
  double m_leftStickAngle;
  Q_PROPERTY(double leftStickAngle READ leftStickAngle WRITE setleftStickAngle NOTIFY leftStickAngleChanged)
public:
  double leftStickAngle() const { return m_leftStickAngle; }
  void setleftStickAngle(const double newValue) {
    if(m_leftStickAngle != newValue) {
      m_leftStickAngle = newValue;
      leftStickAngleChanged();
    }
  }
signals:
  void leftStickAngleChanged();

private:
  double m_rightStickAngle;
  Q_PROPERTY(double rightStickAngle READ rightStickAngle WRITE setrightStickAngle NOTIFY rightStickAngleChanged)
public:
  double rightStickAngle() const { return m_rightStickAngle; }
  void setrightStickAngle(const double newValue) {
    if(m_rightStickAngle != newValue) {
      m_rightStickAngle = newValue;
      rightStickAngleChanged();
    }
  }
signals:
  void rightStickAngleChanged();

private:
  bool m_leftStickPressed;
  Q_PROPERTY(bool leftStickPressed READ leftStickPressed WRITE setleftStickPressed NOTIFY leftStickPressedChanged)
public:
  bool leftStickPressed() const { return m_leftStickPressed; }
  void setleftStickPressed(const bool newValue) {
    if(m_leftStickPressed != newValue) {
      m_leftStickPressed = newValue;
      leftStickPressedChanged();
    }
  }
signals:
  void leftStickPressedChanged();

private:
  bool m_rightStickPressed;
  Q_PROPERTY(bool rightStickPressed READ rightStickPressed WRITE setrightStickPressed NOTIFY rightStickPressedChanged)
public:
  bool rightStickPressed() const { return m_rightStickPressed; }
  void setrightStickPressed(const bool newValue) {
    if(m_rightStickPressed != newValue) {
      m_rightStickPressed = newValue;
      rightStickPressedChanged();
    }
  }
signals:
  void rightStickPressedChanged();

private:
  bool m_guide;
  Q_PROPERTY(bool guide READ guide WRITE setguide NOTIFY guideChanged)
public:
  bool guide() const { return m_guide; }
  void setguide(const bool newValue) {
    if(m_guide != newValue) {
      m_guide = newValue;
      guideChanged();
    }
  }
signals:
  void guideChanged();

private:

  XINPUT_STATE m_state;
};

#endif // XBOXCONTROLLERITEM_H
