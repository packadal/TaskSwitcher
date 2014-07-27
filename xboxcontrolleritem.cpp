#include "xboxcontrolleritem.h"

#define _USE_MATH_DEFINES
#include <math.h>

XBoxControllerItem::XBoxControllerItem(QQuickItem *parent) :
  QQuickItem(parent),
  m_a(false),
  m_b(false),
  m_x(false),
  m_y(false),
  m_rb(false),
  m_rt(false),
  m_lb(false),
  m_lt(false),
  m_back(false),
  m_start(false),
  m_guide(false)
{
  startTimer(20);
}

//this function is not documented nor exported publicly, this hack allows us to use it anyways.
//because fuck private APIs that expose a controller button.
typedef DWORD (WINAPI* XInputGetStateEx_t)(DWORD dwUserIndex, XINPUT_STATE *pState); // 100

#define XINPUT_GAMEPAD_GUIDE 0x0400
void XBoxControllerItem::timerEvent(QTimerEvent *)
{
  HMODULE m = LoadLibraryA("xinput1_3");
  FARPROC f = GetProcAddress(m, (LPCSTR)100);
  XInputGetStateEx_t getStateEx = (XInputGetStateEx_t)f;
  /*DWORD res = */getStateEx(0, &m_state);

  seta(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
  setb(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
  setx(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
  sety(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);

  setrt(m_state.Gamepad.bRightTrigger);
  setlt(m_state.Gamepad.bLeftTrigger);

  setrb(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
  setlb(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);

  setstart(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
  setback(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);

  setleftStickPressed(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
  setrightStickPressed(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);

  short rx = m_state.Gamepad.sThumbRX;
  rx = std::abs(rx) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? 0 : rx;

  short lx = m_state.Gamepad.sThumbLX;
  lx = std::abs(lx) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? 0 : lx;

  short ry = m_state.Gamepad.sThumbRY;
  ry = std::abs(ry) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? 0 : ry;

  short ly = m_state.Gamepad.sThumbLY;
  ly = std::abs(ly) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? 0 : ly;

  setrightStickX(rx);
  setrightStickY(ry);

  setleftStickX(lx);
  setleftStickY(ly);

  double leftStickAngle = std::atan2(m_leftStickY, m_leftStickX) * 180. / M_PI;
  leftStickAngle = std::fmod(leftStickAngle + 360., 360.);
  double rightStickAngle = std::atan2(m_rightStickY, m_rightStickX) * 180. / M_PI;
  rightStickAngle = std::fmod(rightStickAngle + 360., 360.);
  leftStickAngle = lx == 0 && ly == 0 ? -1 : leftStickAngle;
  setleftStickAngle(leftStickAngle);
  rightStickAngle = (rx == 0 && ry == 0) ? -1 : rightStickAngle;
  setrightStickAngle(rightStickAngle);

//  qWarning() << "guide: " << (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_GUIDE) << "; " << m_state.Gamepad.wButtons;
  setguide(m_state.Gamepad.wButtons & XINPUT_GAMEPAD_GUIDE);
}
