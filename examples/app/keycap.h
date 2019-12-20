#pragma once
#include "stdafx.h"

namespace ui {

class Keycap : public Button {
public:
  Keycap();

  virtual void PaintText(IRenderContext* pRender) override;
  virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue) override;

  static std::wstring GetKeyName(int vkcode);

private:
  int vk_code_;
};
}
