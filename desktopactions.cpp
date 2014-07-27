#include "desktopactions.h"

#include <windows.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")

DesktopActions::DesktopActions(QQuickItem *)
{
}


void DesktopActions::shutdown()
{
  HANDLE hToken;
  TOKEN_PRIVILEGES tkp;

  // Get a token for this process.
  if (!OpenProcessToken(GetCurrentProcess(),
                        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    return;

  // Get the LUID for the shutdown privilege.
  LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
                       &tkp.Privileges[0].Luid);

  tkp.PrivilegeCount = 1;  // one privilege to set
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  // Get the shutdown privilege for this process.
  AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                        (PTOKEN_PRIVILEGES)NULL, 0);

  if (GetLastError() != ERROR_SUCCESS)
    return;

  // Shut down the system and force all applications to close.
  ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,
                SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
                SHTDN_REASON_MINOR_UPGRADE |
                SHTDN_REASON_FLAG_PLANNED);
  //shutdown was successful
  return;
}

void DesktopActions::restart()
{
  HANDLE hToken;
  TOKEN_PRIVILEGES tkp;

  // Get a token for this process.
  if (!OpenProcessToken(GetCurrentProcess(),
                        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    return;

  // Get the LUID for the shutdown privilege.
  LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
                       &tkp.Privileges[0].Luid);

  tkp.PrivilegeCount = 1;  // one privilege to set
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  // Get the shutdown privilege for this process.
  AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                        (PTOKEN_PRIVILEGES)NULL, 0);

  if (GetLastError() != ERROR_SUCCESS)
    return;

  // Shut down the system and force all applications to close.
  ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
                SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
                SHTDN_REASON_MINOR_UPGRADE |
                SHTDN_REASON_FLAG_PLANNED);
  //shutdown was successful
  return;
}
