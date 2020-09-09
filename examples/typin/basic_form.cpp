#include "stdafx.h"
#include "basic_form.h"
#include "keycap.h"
#include "chart.h"
#include "typedef.h"
#include <chrono>

#define WM_USER_KEY WM_USER + 1

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
  //ShowWindow(false);
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


std::wstring GetKeyName(unsigned int virtualKey) {
  unsigned int scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

  // because MapVirtualKey strips the extended bit for some keys
  switch (virtualKey) {
    case VK_LEFT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:  // arrow keys
    case VK_PRIOR:
    case VK_NEXT:  // page up and page down
    case VK_END:
    case VK_HOME:
    case VK_INSERT:
    case VK_DELETE:
    case VK_DIVIDE:  // numpad slash
    case VK_NUMLOCK: {
      scanCode |= 0x100;  // set extended bit
      break;
    }
  }

  WCHAR keyName[50];
  if (GetKeyNameText(scanCode, keyName, sizeof(keyName)) != 0) {
    return keyName;
  } else {
    return _T("[Error]");
  }
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

