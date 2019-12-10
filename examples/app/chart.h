#pragma once
#include "stdafx.h"

namespace ui {

class ThermodynamicChart : public Control {
public: 
  ThermodynamicChart();

	virtual void Paint(IRenderContext* pRender, const UiRect& rcPaint) override;
private:
  
};

}
