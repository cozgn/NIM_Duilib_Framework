#pragma once

#include <map>

enum ThreadId { kThreadUI, kThreadGlobalMisc };

struct FileDescription {
  std::wstring url;
	std::wstring replace;
	std::wstring tmp;
};

struct UpdateDescription {
  std::wstring title;
  std::vector<FileDescription> files_desc;
	std::wstring launcher;
};

class MainForm : public ui::WindowImplBase
{
public:
	MainForm();
	~MainForm();

	/**
	 * 以下三个接口是必须要覆写的接口，父类会调用这三个接口来构建窗口
	 * GetSkinFolder		接口设置你要绘制的窗口皮肤资源路径
	 * GetSkinFile			接口设置你要绘制的窗口的 xml 描述文件
	 * GetWindowClassName	接口设置窗口唯一的类名称
	 */
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;

	/**
	 * 收到 WM_CREATE 消息时该函数会被调用，通常做一些控件初始化的操作
	 */
	virtual void InitWindow() override;

	/**
	 * 收到 WM_CLOSE 消息时该函数会被调用
	 */
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	/**
   * 动态更新进度条接口
   */
  void OnProgressValueChagned(float value);

	static const std::wstring kClassName;

private: 
  bool ParseCommandLine();
  void Download();
	std::wstring CreateTempFile();
	void OnDownloadComplete();
	std::wstring GetArgv();
	void SetInfo(const std::wstring &msg);

  ui::Progress* progress_bar_;
	ui::Label* label_info_;
	ui::Label* label_title_;

	UpdateDescription update_desc_;
	//std::ofstream* tmp_fs_;
	//std::wstring tmp_file_name_;

};

