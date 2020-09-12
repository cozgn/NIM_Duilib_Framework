#pragma once
#include "stdafx.h"

#include <shellapi.h>
#include "hook.h"
#include "repo.h"
#include <mutex>

class BasicForm : public ui::WindowImplBase, public KeyboardHookHelper::KeyboardEventListener
{
public:
	BasicForm();
	~BasicForm();

	/**
	 * 以下三个接口是必须要覆写的接口，父类会调用这三个接口来构建窗口
	 * GetSkinFolder		接口设置你要绘制的窗口皮肤资源路径
	 * GetSkinFile			接口设置你要绘制的窗口的 xml 描述文件
	 * GetWindowClassName	接口设置窗口唯一的类名称
	 */
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;

	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;

	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	/**
	 * 收到 WM_CREATE 消息时该函数会被调用，通常做一些控件初始化的操作
	 */
	virtual void InitWindow() override;

	/**
	 * 收到 WM_CLOSE 消息时该函数会被调用
	 */
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;



	
	void InitTray();
	void ShowTray();
	void DeleteTray();


  virtual bool keyboardEvent(int nCode, WPARAM wParam, LPARAM lParam) override;


  void CheckThread();

	static const std::wstring kClassName;
	KeyboardHookHelper keyboard_hook_helper_;
	Repo repo_;
  std::vector<KeyboardRecord> records_;
	std::mutex mutex_;

	std::thread check_thread_;
	bool exit_;
	uint64_t last_input_time_;

  NOTIFYICONDATA notifyicondata_;
	UINT WM_TASKBAR_CREATED;
};

