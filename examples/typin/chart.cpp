#include "stdafx.h"
#include "chart.h"

ui::ThermodynamicChart::ThermodynamicChart() {
}

void ui::ThermodynamicChart::Paint(IRenderContext* pRender, const UiRect& rcPaint) {
  __super::Paint(pRender, rcPaint);

  if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

  Gdiplus::Graphics graphics(pRender->GetDC());
  graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
  Gdiplus::Color centerColor(0, 0, 0);
  Gdiplus::Color edgeColor(255, 255, 255);
  Gdiplus::GraphicsPath path;
  Gdiplus::RectF r(rcPaint.left + 25, rcPaint.top + 25, 20, 20);
  path.AddEllipse(rcPaint.left, rcPaint.top, 50, 50);
  Gdiplus::PathGradientBrush brush(&path);
  brush.SetCenterColor(centerColor);
  int count = 1;
  brush.SetSurroundColors(&edgeColor, &count);
  Gdiplus::RectF rectf(rcPaint.left, rcPaint.top, 50, 50);
  graphics.FillEllipse(&brush, rectf);
}
