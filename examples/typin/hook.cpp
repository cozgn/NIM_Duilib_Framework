#include "stdafx.h"
#include "Hook.h"

//全局变量
static KeyboardHookHelper* helper;

//静态成员
std::set<KeyboardHookHelper::KeyboardEventListener*>
    KeyboardHookHelper::mListenerList;

KeyboardHookHelper::KeyboardHookHelper() : mHook(NULL) {
  helper = this;
  setWindowsHook();
}

LRESULT KeyboardHookHelper::keyboardHookCallback(int nCode, WPARAM wParam,
                                         LPARAM lParam) {
  if (nCode >= HC_ACTION) {
    int r = 0;
    for (auto listener : helper->mListenerList) {
      if (listener->keyboardEvent(nCode, wParam, lParam)) {
        ++r;
      }
    }
    if (r > 0) {
      return true;
    }
  }
  return (CallNextHookEx(helper->mHook, nCode, wParam, lParam));
}

bool KeyboardHookHelper::AddKeyboardEventListener(
    KeyboardEventListener* listener) {
  if (listener == NULL) {
    return false;
  }
  mListenerList.insert(listener);
  return true;
}

KeyboardHookHelper::~KeyboardHookHelper() {
  if (mHook) {
    UnhookWindowsHookEx(mHook);
    mHook = NULL;  //   卸掉钩子
  }
  mListenerList.clear();
  helper = NULL;
}

bool KeyboardHookHelper::setWindowsHook() {
  if (mHook == NULL) {
    //安装新钩子
    mHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)keyboardHookCallback,
                             GetModuleHandle(NULL), 0);
  }
  return mHook != NULL;
}
