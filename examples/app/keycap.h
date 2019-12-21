#pragma once
#include "stdafx.h"

namespace ui {

class Keycap : public Button {
public:
  Keycap();
  virtual ~Keycap();

  virtual void PaintText(IRenderContext* pRender) override;
  virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue) override;

  int Count() {return count_;};
  void Increase() { count_++; Invalidate(); };
  static std::wstring GetKeyName(int vkcode);
  static std::map<int, ui::Keycap*>* AllView() { return &all_keycaps_;};

private:
  int vk_code_;
  int count_; 
  static std::map<int, ui::Keycap*> all_keycaps_;
};
}
