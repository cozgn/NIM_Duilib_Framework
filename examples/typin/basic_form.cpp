#include "stdafx.h"
#include "basic_form.h"
#include "keycap.h"
#include "chart.h"
#include "typedef.h"
#include <chrono>
#include "resource.h"


#define WM_USER_KEY     WM_USER + 1
#define WM_APP_TRAY     WM_USER + 2

/**
 * 热力图
 * 1.为每个数据点设置一个从中心向外灰度渐变的圆；
 * 2.利用灰度可以叠加的原理，计算每个像素点数据交叉叠加得到的灰度值；
 * 3.根据每个像素计算得到的灰度值，在一条彩色色带中进行颜色映射，最后对图像进行着色，得到热力图。
 */
const std::wstring BasicForm::kClassName = L"Basic";

BasicForm::BasicForm():exit_(false), check_thread_(&BasicForm::CheckThread, this)
{
}


BasicForm::~BasicForm()
{
  exit_ = true;
  check_thread_.join();
}

std::wstring BasicForm::GetSkinFolder()
{
	return L"app";
}

std::wstring BasicForm::GetSkinFile()
{
	return L"app.xml";
}

std::wstring BasicForm::GetWindowClassName() const
{
	return kClassName; }

ui::Control* BasicForm::CreateControl(const std::wstring& pstrClass) {
  if (pstrClass.compare(_T("Keycap")) == 0) {
	  return new ui::Keycap();
	} else if (pstrClass.compare(_T("Chart")) == 0) {
    return new ui::ThermodynamicChart();
  }
  return nullptr;
}

LRESULT BasicForm::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam,
                             BOOL& bHandled) {
  if (wParam == VK_ESCAPE) {
	  Close();
	}
  return false;
}

void BasicForm::InitWindow() {
  keyboard_hook_helper_.AddKeyboardEventListener(this);
  InitTray();
  WM_TASKBAR_CREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
}

LRESULT BasicForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  SqliteDatabase db(L"");
  db.Insert(records_);
  records_.clear();

	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

LRESULT BasicForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

  switch (uMsg) {
    case WM_SIZE: {
      if (wParam == SIZE_MINIMIZED) {
        ShowWindow(false);
        ShowTray();
      }
    } break;
    case WM_APP_TRAY:
      if (lParam == WM_LBUTTONDBLCLK) {
        ShowWindow(true);
        ToTopMost(false);
        DeleteTray();
      }
      break;
    default:
      if (WM_TASKBAR_CREATED == uMsg) {
        //是否重建托盘
      }
      break;
  }


  if (uMsg == WM_USER_KEY) {
    auto i = repo_.statistics.find(wParam);
    if (i != repo_.statistics.end()) {
      repo_.statistics[wParam] = i->second + 1;
    } else {
      repo_.statistics[wParam] = 1;
    }
    LOGI(_T("vk %d count = %d"), wParam, repo_.statistics[wParam]);
    auto view = ui::Keycap::AllView()->find(wParam);
    if (view != ui::Keycap::AllView()->end()) {
      view->second->Increase();
    }
    return 0;
  }
  return __super::HandleMessage(uMsg, wParam, lParam);
}

void BasicForm::InitTray() {
  notifyicondata_.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
  notifyicondata_.hWnd = GetHWND();
  notifyicondata_.uID = IDR_MAINFRAME;
  notifyicondata_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  notifyicondata_.uCallbackMessage = WM_APP_TRAY;
  notifyicondata_.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_SMALL));
  wcscpy_s(notifyicondata_.szTip, _T("typin")); 
}

void BasicForm::ShowTray() {
  Shell_NotifyIcon(NIM_ADD, &notifyicondata_);
}

void BasicForm::DeleteTray() {
  Shell_NotifyIcon(NIM_DELETE, &notifyicondata_);  //在托盘中删除图标
}

bool BasicForm::keyboardEvent(int nCode, WPARAM wParam, LPARAM lParam) {
  KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
  using namespace std::chrono;
  static int64_t last_vk_code = -1;
  if (!(kbd->flags & LLKHF_UP)) {
    if (kbd->vkCode == last_vk_code) {
      return false;
    }
    last_vk_code = kbd->vkCode;
    uint64_t now = time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count();
    PostMessageW(WM_USER_KEY, (WPARAM)kbd->vkCode, 0);
    std::lock_guard<std::mutex> lock(mutex_);
    records_.push_back({kbd->vkCode, now});
  } else {
    last_vk_code = -1;
  }
  return false;
}

void BasicForm::CheckThread() {
  while (!exit_) {
    if (records_.size() > 0) {
        SqliteDatabase db(L"");
        mutex_.lock();
        std::vector<KeyboardRecord> tmp = records_;
        records_.clear();
        mutex_.unlock();
        db.Insert(tmp);
    }
    Sleep(3000);
  }
}

