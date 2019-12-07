#include "stdafx.h"
#include "basic_form.h"
#include "keycap.h"

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
