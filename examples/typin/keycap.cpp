#include "stdafx.h"
#include "keycap.h"
#include "typedef.h"

std::map<int, ui::Keycap*> ui::Keycap::all_keycaps_;

ui::Keycap::Keycap() {
  //SetAttribute(_T("class"), _T("keycap_global_win"));
  SetAttribute(_T("width"), _T("stretch"));
	SetAttribute(_T("font"), _T("app_consolas_bold_14"));
	SetAttribute(_T("bordersize"), _T("1"));
	SetAttribute(_T("singleline"), _T("false"));
	SetAttribute(_T("textpadding"), _T("8,5,8,5"));
	SetAttribute(_T("align"), _T("left"));
	SetAttribute(_T("bordercolor"), _T("app_vk_border_color"));
	//SetAttribute(_T("hotbordercolor"), _T("color_border_hot"));
	SetAttribute(_T("normalcolor"), _T("app_vk_bg_normal_color"));
	SetAttribute(_T("hotcolor"), _T("app_vk_bg_hot_color"));
	SetAttribute(_T("pushedcolor"), _T("app_vk_bg_push_color"));
	SetAttribute(_T("pushedtextcolor"), _T("app_vk_bg_normal_color"));
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

	DWORD hint_text_color = GlobalManager::GetTextColor(L"app_vk_hint_text_color");
	pRender->DrawText(rc, format(L"%d", count_), 
	                  hint_text_color , 
										L"system_12", 
										DT_BOTTOM | DT_CENTER,
										100,
										m_bLineLimit);

	if (letters.size() > 1) {
  	pRender->DrawText(rc, letters[0], dwClrColor, m_sFontId, m_uTextStyle, 255, m_bLineLimit);
		rc.Offset(0, 22);
  	pRender->DrawText(rc, letters[1], dwClrColor, m_sFontId, m_uTextStyle, 255, m_bLineLimit);
	} else {
  	pRender->DrawText(rc, letters[0], dwClrColor, m_sFontId, m_uTextStyle, 255, m_bLineLimit);
	}
}

void ui::Keycap::SetAttribute(const std::wstring& strName, const std::wstring& strValue) {
  if (strName == _T("vk")) {
	  vk_code_ = _wtoi(strValue.c_str());
		all_keycaps_[vk_code_] = this;
	} else if (strName == _T("text")) {
	  letters = Split(strValue, L" ");
	  __super::SetAttribute(strName, strValue);
	} else {
	  __super::SetAttribute(strName, strValue);
	}
}

std::wstring ui::Keycap::GetKeyName(int vkcode) { 
  
	return std::wstring(); 
}

std::vector<std::wstring> ui::Keycap::Split(const std::wstring& str,
                                            const std::wstring& delim,
                                            const bool trim_empty) {
  size_t pos, last_pos = 0, len;
  std::vector<std::wstring> tokens;

  while (true) {
    pos = str.find(delim, last_pos);
    if (pos == std::wstring::npos) {
      pos = str.size();
    }

    len = pos - last_pos;
    if (!trim_empty || len != 0) {
      tokens.push_back(str.substr(last_pos, len));
    }

    if (pos == str.size()) {
      break;
    } else {
      last_pos = pos + delim.size();
    }
  }

  return tokens;
}
