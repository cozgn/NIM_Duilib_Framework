#pragma once

enum ThreadId { kThreadUI, kThreadGlobalMisc };


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
  void StartDownloadTask(const std::string &url);
	void Upgrade(const std::wstring &old_exe, const std::wstring &new_exe);
	std::wstring CreateTempFile();
	void OnDownloadComplete();
	std::wstring GetArgv();

  ui::Progress* progress_bar_;
	ui::Label* label_progress_;
	ui::Label* label_title_;

	std::ofstream* tmp_fstream_;
	std::wstring tmp_file_name_;

};

