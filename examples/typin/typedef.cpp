#include "stdafx.h"
#include "typedef.h"
#include <string>
#include <windows.h>

std::wstring LastErrorStr(DWORD error_code) {
  WCHAR *win_msg_ptr;
  std::wstring msg;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&win_msg_ptr, 0, NULL);
  if (win_msg_ptr != NULL) {
    msg = win_msg_ptr;
    LocalFree(win_msg_ptr);
  }
  return msg;
}

