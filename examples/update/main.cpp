﻿// basic.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "main.h"
#include "main_form.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	std::wstring s = MainForm::GetArgv();
	if (MainForm::GetArgv().length() < 10) {
	  return 0;
	}

	// 创建主线程
	MainThread thread;

	// 执行主线程循环
	thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

	return 0;
}

void MiscThread::Init() { nbase::ThreadManager::RegisterThread(thread_id_); }

void MiscThread::Cleanup() { nbase::ThreadManager::UnregisterThread(); }


void MainThread::Init()
{
	nbase::ThreadManager::RegisterThread(kThreadUI);

		// 启动杂事处理线程
  misc_thread_.reset(new MiscThread(kThreadGlobalMisc, "Global Misc Thread"));
  misc_thread_->Start();

	// 获取资源路径，初始化全局参数
	std::wstring theme_dir = nbase::win32::GetCurrentModuleDirectory();
#ifdef _DEBUG
	// Debug 模式下使用本地文件夹作为资源
	// 默认皮肤使用 resources\\themes\\default
	// 默认语言使用 resources\\lang\\zh_CN
	// 如需修改请指定 Startup 最后两个参数
	ui::GlobalManager::Startup(theme_dir + L"resources\\", ui::CreateControlCallback(), false);
#else
	// Release 模式下使用资源中的压缩包作为资源
	// 资源被导入到资源列表分类为 THEME，资源名称为 IDR_THEME
	// 如果资源使用的是本地的 zip 文件而非资源中的 zip 压缩包
	// 可以使用 OpenResZip 另一个重载函数打开本地的资源压缩包
	ui::GlobalManager::OpenResZip(MAKEINTRESOURCE(IDR_THEME), L"THEME", "");
	// ui::GlobalManager::OpenResZip(L"resources.zip", "");
	ui::GlobalManager::Startup(L"resources\\", ui::CreateControlCallback(), false);
#endif

	// 创建一个默认带有阴影的居中窗口
	MainForm* window = new MainForm();
	window->Create(NULL, MainForm::kClassName.c_str(), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX , 0);
	window->CenterWindow();
	window->ShowWindow();
}

void MainThread::Cleanup()
{
	ui::GlobalManager::Shutdown();
	misc_thread_->Stop();
  misc_thread_.reset(nullptr);
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();
}
