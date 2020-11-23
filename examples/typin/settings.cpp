#include "stdafx.h"
#include "settings.h"
#include "registry.hpp"
#include "typedef.h"

//const std::wstring BootStart::REG_RUN =L"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const std::wstring BootStart::REG_RUN =L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

BootStart::BootStart() {}

bool BootStart::setEnable(bool enable) {
  using namespace m4x1m1l14n;

  try {
    auto access = Registry::DesiredAccess::Read | Registry::DesiredAccess::Write;
    auto key = Registry::CurrentUser->Create(REG_RUN, access);
    if (enable) {
      TCHAR exe[MAX_PATH] = {0};
      GetModuleFileName(NULL, exe, MAX_PATH);
      key->SetString(L"Typin", exe);
    } else {
      key->Delete(L"Typin");
    }
  }
  catch (const std::exception& e) {
    LOGI(L"%s", e.what())
  }

    //QString applicationName = QApplication::applicationName();
    //QSettings* settings = new QSettings(REG_RUN, QSettings::NativeFormat);
    //if (enable) {
    //    QString applicationPath = QApplication::applicationFilePath();
    //    settings->setValue(applicationName, applicationPath.replace("/", "\\"));
    //} else {
    //    settings->remove(applicationName);
    //}
    //delete settings;
    return true;
}

bool BootStart::isEnable() {
  using namespace m4x1m1l14n;
  try {
    auto key = Registry::CurrentUser->Create(REG_RUN);
    return key->HasValue(L"Typin");
  }
  catch (const std::exception& e) {
    LOGI(L"%s", e.what())
  }
  return false;
}

BootStart::~BootStart() {}
