#include "stdafx.h"
#include "keycap.h"


#define LOG(...) ::OutputDebugString((nbase::StringPrintf(__VA_ARGS__).append(_T("\n")).c_str()));

std::map<int, ui::Keycap*> ui::Keycap::all_keycaps_;

ui::Keycap::Keycap() {
  //SetAttribute(_T("class"), _T("keycap_global_win"));
  SetAttribute(_T("width"), _T("stretch"));
	SetAttribute(_T("font"), _T("system_16"));
	SetAttribute(_T("bordersize"), _T("0"));
	//SetAttribute(_T("borderround"), _T("3,3,3,3"));
	//SetAttribute(_T("bordercolor"), _T("color_border"));
	//SetAttribute(_T("hotbordercolor"), _T("color_border_hot"));
	SetAttribute(_T("normalcolor"), _T("app_vk_bg_normal_color"));
	SetAttribute(_T("hotcolor"), _T("app_vk_bg_hot_color"));
	SetAttribute(_T("pushedcolor"), _T("app_vk_bg_push_color"));
	SetAttribute(_T("height"), _T("50"));
}

ui::Keycap::~Keycap() {
  auto i = all_keycaps_.find(vk_code_);
	if (i != all_keycaps_.end()) {
	  all_keycaps_.erase(i);
	}
}

void ui::Keycap::PaintText(IRenderContext* pRender) {
	if (GetText().empty()) return;
	UiRect rc = this->m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;

	ControlStateType stateType = this->m_uButtonState;
	if (stateType == kControlStatePushed && GetStateTextColor(kControlStatePushed).empty()) {
		stateType = kControlStateHot;
	}
	if (stateType == kControlStateHot && GetStateTextColor(kControlStateHot).empty()) {
		stateType = kControlStateNormal;
	}
	if (stateType == kControlStateDisabled && GetStateTextColor(kControlStateDisabled).empty()) {
		stateType = kControlStateNormal;
	}
	std::wstring clrColor = GetStateTextColor(stateType);
	DWORD dwClrColor = GlobalManager::GetTextColor(clrColor);

	if (m_bSingleLine)
		m_uTextStyle |= DT_SINGLELINE;
	else
		m_uTextStyle &= ~DT_SINGLELINE;

	if (this->m_animationManager.GetAnimationPlayer(kAnimationHot)) {
		if ((stateType == kControlStateNormal || stateType == kControlStateHot)
			&& !GetStateTextColor(kControlStateHot).empty()) {
			std::wstring clrColor = GetStateTextColor(kControlStateNormal);
			if (!clrColor.empty()) {
				DWORD dwClrColor = GlobalManager::GetTextColor(clrColor);
				pRender->DrawText(rc, GetText(), dwClrColor, m_sFontId, m_uTextStyle, 255, m_bLineLimit);
			}

			if (this->m_nHotAlpha > 0) {
				std::wstring clrColor = GetStateTextColor(kControlStateHot);
				if (!clrColor.empty()) {
					DWORD dwClrColor = GlobalManager::GetTextColor(clrColor);
					pRender->DrawText(rc, GetText(), dwClrColor, m_sFontId, m_uTextStyle, (BYTE)this->m_nHotAlpha, m_bLineLimit);
				}
			}

			return;
		}
	}

	pRender->DrawText(rc, GetText(), dwClrColor, m_sFontId, m_uTextStyle, 255, m_bLineLimit);
	rc.left -= 30;
	rc.top -= 30;
	pRender->DrawText(rc, nbase::StringPrintf(_T("%d"), count_), dwClrColor, m_sFontId, m_uTextStyle, 255, m_bLineLimit);
}

void ui::Keycap::SetAttribute(const std::wstring& strName, const std::wstring& strValue) {
  if (strName == _T("vk")) {
	  vk_code_ = _wtoi(strValue.c_str());
		all_keycaps_[vk_code_] = this;
	} else {
	  __super::SetAttribute(strName, strValue);
	}
}

std::wstring ui::Keycap::GetKeyName(int vkcode) { 
  
	return std::wstring(); 
}
