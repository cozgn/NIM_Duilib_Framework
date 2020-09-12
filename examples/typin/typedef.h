#pragma once
#include "stdafx.h"

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <Windows.h>
#include <chrono>


inline std::wstring format(const wchar_t* format, ...) {
  std::wstring tmp;
  va_list marker;
  va_start(marker, format);
  size_t num = vswprintf(0, 0, format, marker);
  if (num >= tmp.capacity()) tmp.reserve(num + sizeof(wchar_t));
  tmp.resize(num);
  vswprintf((wchar_t*)tmp.data(), tmp.capacity(), format, marker);
  va_end(marker);
  return tmp;
}

inline std::string format(const char* format, ...) {
  std::string tmp;
  va_list marker;
  va_start(marker, format);
  size_t num = vsnprintf(0, 0, format, marker);
  if (num >= tmp.capacity()) tmp.reserve(num + sizeof(char));
  tmp.resize(num);
  vsnprintf((char*)tmp.data(), tmp.capacity(), format, marker);
  va_end(marker);
  return tmp;
}


#define CONSOLE_APP 0
#define LOG_LEVEL   1

#if CONSOLE_APP 
#define LOGE(format,...) {if (DEBUG_LEVEL <=2) {fwprintf(stderr,format,##__VA_ARGS__);fwprintf(stderr,L"\n");}}
#define LOGI(format,...) {if (DEBUG_LEVEL <=1) {fwprintf(stdout,format,__VA_ARGS__);fwprintf(stdout,L"\n");}}
#define LOGV(format,...) {if (DEBUG_LEVEL <=0) {fwprintf(stdout,format,##__VA_ARGS__);fwprintf(stdout,L"\n");}}
#define PRINTLN(format,...) {{fwprintf(stdout,format,##__VA_ARGS__);fwprintf(stdout,L"\n");}}
#else
#define LOGE(fmt,...) {if (LOG_LEVEL <=2) OutputDebugString(format(fmt TEXT("\n"),##__VA_ARGS__).c_str());}
#define LOGI(fmt,...) {if (LOG_LEVEL <=1) OutputDebugString(format(fmt TEXT("\n"),##__VA_ARGS__).c_str());}
#define LOGV(fmt,...) {if (LOG_LEVEL <=0) OutputDebugString(format(fmt TEXT("\n"),##__VA_ARGS__).c_str());}
#define PRINTLN(fmt,...) {{OutputDebugString(format(fmt TEXT("\n"),##__VA_ARGS__).c_str());}
#endif



std::wstring LastErrorStr(DWORD error_code);

class progress_timer {
 public:
  explicit progress_timer(const TCHAR* label) : label_(label) {
    start_time_ = std::chrono::high_resolution_clock::now();
  }
  ~progress_timer() {
    uint64_t progress = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
    uint64_t second = progress / 1000;
    if (second > 0) {
      LOGI(L"%s took %llus.%llums", label_.c_str(),
           progress / 1000, progress % 1000);
    } else {
      LOGI(L"%s took %llums", label_.c_str(), progress % 1000);
    }
  }

 private:
  std::wstring label_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
};


class error {
public:
  error(const std::wstring& what):what_(what.c_str()) {
  }
  std::wstring what() const {
    return what_;
  }
  bool operator!=(const error& e) const {
    return e.what_.compare(what_) != 0;
  }
  bool operator==(const error& e) const {
    return e.what_.compare(what_) == 0;
  }
private:
  std::wstring what_;
};
#define nil error(L"nil")
