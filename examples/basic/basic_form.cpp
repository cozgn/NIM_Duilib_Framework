#include "stdafx.h"
#include "basic_form.h"

const std::wstring BasicForm::kClassName = L"Basic";

BasicForm::BasicForm()
{
}


BasicForm::~BasicForm()
{
}

std::wstring BasicForm::GetSkinFolder()
{
	return L"basic";
}

std::wstring BasicForm::GetSkinFile()
{
	return L"basic.xml";
}

std::wstring BasicForm::GetWindowClassName() const
{
	return kClassName;
}

void BasicForm::InitWindow()
{
  ui::TabBox* tabbox = dynamic_cast<ui::TabBox*>(FindControl(L"tabbox"));

  tabbox->SetAttribute(L"fadeswitch", L"true");
  //dynamic_cast<ui::Box*>(FindControl(L"tab1"))->SetAttribute(L"fadeswitch", L"true");
  //dynamic_cast<ui::Box*>(FindControl(L"tab2"))->SetAttribute(L"fadeswitch", L"true");
  //dynamic_cast<ui::Box*>(FindControl(L"tab3"))->SetAttribute(L"fadeswitch", L"true");

  ui::Button* btn1 = dynamic_cast<ui::Button*>(FindControl(L"btn1"));
  btn1->AttachClick([&](ui::EventArgs* args)->bool {
    ui::TabBox* tabbox = dynamic_cast<ui::TabBox*>(FindControl(L"tabbox"));
    tabbox->SelectItem(L"tab1");
    return true;
  });
  ui::Button* btn2 = dynamic_cast<ui::Button*>(FindControl(L"btn2"));
  btn2->AttachClick([&](ui::EventArgs* args)->bool {
    ui::TabBox* tabbox = dynamic_cast<ui::TabBox*>(FindControl(L"tabbox"));
    tabbox->SelectItem(L"tab2");
    return true;
  });

  ui::Button* btn3 = dynamic_cast<ui::Button*>(FindControl(L"btn3"));
  btn3->AttachClick([&](ui::EventArgs* args)->bool {
    ui::TabBox* tabbox = dynamic_cast<ui::TabBox*>(FindControl(L"tabbox"));
    tabbox->SelectItem(L"tab3");
    return true;
  });
}

LRESULT BasicForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}
