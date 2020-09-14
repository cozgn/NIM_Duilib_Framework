#pragma once
#include "stdafx.h"

namespace ui {

class Keycap : public Button {
public:
  Keycap();
  virtual ~Keycap();

  virtual void PaintText(IRenderContext* pRender) override;
  virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue) override;

  int64_t Count() {return count_;};
  void Increase() { count_++; Invalidate(); };
  void set_count(int64_t count) {count_ = count; Invalidate();};
  static std::wstring GetKeyName(int vkcode);
  static std::map<int, ui::Keycap*>* AllView() { return &all_keycaps_;};

  std::vector<std::wstring> Split(const std::wstring& str,
                                 const std::wstring& delim,
                                 const bool trim_empty = false);

private:
  int vk_code_;
  int64_t count_; 
  std::vector<std::wstring> letters;
  static std::map<int, ui::Keycap*> all_keycaps_;
};
}
