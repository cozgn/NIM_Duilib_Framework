#pragma once
#include <Windows.h>
#include <set>

class KeyboardHookHelper {
 public:

  class KeyboardEventListener {
   public:
    virtual ~KeyboardEventListener(){};
    virtual bool keyboardEvent(int nCode, WPARAM wParam, LPARAM lParam) = 0;
  };

  KeyboardHookHelper();
  ~KeyboardHookHelper();
  bool AddKeyboardEventListener(KeyboardEventListener* listener);

  static LRESULT CALLBACK keyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
 private:
  bool setWindowsHook();

 private:
  HHOOK mHook;
  static std::set<KeyboardEventListener*> mListenerList;
};


