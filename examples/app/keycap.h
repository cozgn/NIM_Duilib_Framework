#pragma once
#include "stdafx.h"

namespace ui {

class Keycap : public Button {
public:
  Keycap();
  virtual void PaintText(IRenderContext* pRender) override;
};
}
