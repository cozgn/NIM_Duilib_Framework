#include "stdafx.h"
#include "basic_form.h"
#include "keycap.h"
#include "chart.h"


/**
 * 热力图
 * 1.为每个数据点设置一个从中心向外灰度渐变的圆；
 * 2.利用灰度可以叠加的原理，计算每个像素点数据交叉叠加得到的灰度值；
 * 3.根据每个像素计算得到的灰度值，在一条彩色色带中进行颜色映射，最后对图像进行着色，得到热力图。
 */
const std::wstring BasicForm::kClassName = L"Basic";

BasicForm::BasicForm()
{
}


BasicForm::~BasicForm()
{
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

void BasicForm::InitWindow()
{

}

LRESULT BasicForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}
