#include "stdafx.h"
#include "main_form.h"

#include "httplib.h"
#include "json.hpp"

const std::wstring MainForm::kClassName = L"Update";

void Toast(const std::wstring &msg) {
  MessageBox(NULL, msg.c_str(), L"Error", 0);
}

std::string get_host_from_url(const std::string &url) {
  int pos1 = url.find("://");
  if (pos1 == std::string::npos) {
    return "";
  }
  pos1 += 3;
  int pos2 = url.find("/", pos1);
  return url.substr(pos1, pos2);
}

std::string get_path_from_url(const std::string &url) {
  int pos1 = url.find("://");
  if (pos1 == std::string::npos) {
    return "";
  }
  pos1 += 3;
  int pos2 = url.find("/", pos1);
  if (pos2 == std::string::npos) {
    return "/";
  }
  return url.substr(pos2);
}


void LaunchProgram(const std::wstring &program) {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  //隐藏掉可能出现的cmd命令窗口
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;
  ZeroMemory(&pi, sizeof(pi));

  // Start the child process.
  if (!CreateProcess(NULL,  // No module name (use command line)
                     (LPWSTR)program.c_str(),  // Command line
                     NULL,                     // Process handle not inheritable
                     NULL,                     // Thread handle not inheritable
                     FALSE,  // Set handle inheritance to FALSE
                     0,      // No creation flags
                     NULL,   // Use parent's environment block
                     NULL,   // Use parent's starting directory
                     &si,    // Pointer to STARTUPINFO structure
                     &pi)    // Pointer to PROCESS_INFORMATION structure
  ) {
    // printf("CreateProcess failed (%d).\n", GetLastError());
    return;
  }

  // Wait until child process exits.
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles.
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

MainForm::MainForm() {}

MainForm::~MainForm() {}

std::wstring MainForm::GetSkinFolder() { return L"update"; }

std::wstring MainForm::GetSkinFile() { return L"main.xml"; }

std::wstring MainForm::GetWindowClassName() const { return kClassName; }

void MainForm::InitWindow() {
  label_title_ = dynamic_cast<ui::Label*>(FindControl(L"title"));
  progress_bar_ = dynamic_cast<ui::Progress*>(FindControl(L"progress"));
  label_progress_ = dynamic_cast<ui::Label*>(FindControl(L"percent"));

  Toast(GetCommandLine());
  json::JSON obj = json::JSON::Load(nbase::UTF16ToUTF8(GetArgv()));
  if (obj.hasKey("title")) {
    label_title_->SetText(nbase::UTF8ToUTF16(obj["title"].ToString()));
  }
  if (!obj.hasKey("url")) {
    Toast(L"No URL");
    return;
  }
  StartDownloadTask(obj["url"].ToString());


  //std::wstring exe =
  //    L"D:\\pengzc\\Documents\\sources\\NIM_Duilib_Framework\\bin\\update_d_"
  //    L"slave.exe";
  //std::wstring json = L"{\"title\": 123, \"url\": \"http://www.baidu.com\"}";

  //json::JSON obj;
  //obj["title"] = "this is title";
  //obj["url"] = "http://www.baidu.com/sadfs_adfasd";

  //std::wstring argv = nbase::UTF8ToUTF16(obj.dump());

  //cmdProcess(nbase::StringPrintf(L"cmd /c start %s %s", exe.c_str(), argv.c_str()));

}

LRESULT MainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam,
                          BOOL& bHandled) {
  PostQuitMessage(0L);
  return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void MainForm::OnProgressValueChagned(float value) {}

void MainForm::StartDownloadTask(const std::string &url) {
  if (tmp_file_name_.empty()) {
    tmp_file_name_ = CreateTempFile();
  }
  if (tmp_file_name_.empty()) {
    MessageBox(NULL, L"无法创建临时文件", L"Error", 0);
    Close();
    return;
  }
  tmp_fstream_ =
      new std::ofstream(tmp_file_name_, std::ios::out | std::ios::binary);
  if (!tmp_fstream_->is_open()) {
    MessageBox(NULL, L"无法打开临时文件", L"Error", 0);
    Close();
    return;
  }

  StdClosure task = [this, url]() {
    httplib::Client cli(get_host_from_url(url));
    auto progress = [this](uint64_t len, uint64_t total) {
      std::wstring msg =
          nbase::StringPrintf(L"%lld / %lld bytes => %d%% complete\n", len,
                              total, (int)(len * 100 / total));

      static int last_percent = -1;
      int percent = len * 100 / total;
      if (percent != last_percent) {
        last_percent = percent;
        nbase::ThreadManager::PostTask(kThreadUI, [this, percent]() {
          progress_bar_->SetValue(percent);
          label_progress_->SetText(nbase::StringPrintf(L"%d%%", percent));
        });
      }
      return true;
    };

    auto content_receiver = [this](const char* data,
                                   size_t data_length) -> bool {
      tmp_fstream_->write(data, data_length);
      return true;
    };
    std::shared_ptr<httplib::Response> res =
        cli.Get(get_path_from_url(url).c_str(), content_receiver, progress);
    if (res->status == 200) {
      OnDownloadComplete();
    }
  };
  nbase::ThreadManager::PostTask(kThreadGlobalMisc, task);
}

void MainForm::Upgrade(const std::wstring& old_exe,
                       const std::wstring& new_exe) {}

std::wstring MainForm::CreateTempFile() {
  TCHAR szPathName[MAX_PATH];
  TCHAR szFileName[MAX_PATH];
  if (!::GetTempPath(MAX_PATH, szPathName)) {
    return L"";
  }
  //创建临时文件名并在目录中创建文件
  if (!::GetTempFileName(szPathName, _T("flythings_auth.tmp"), 0, szFileName)) {
    return L"";
  }
  return szFileName;
}

void MainForm::OnDownloadComplete() {
  // check crc32
  //json::JSON obj = json::JSON::Load(nbase::UTF16ToUTF8(GetArgv()));
  Toast(nbase::StringPrintf(L"download success %s", tmp_file_name_.c_str()).c_str());
}

std::wstring MainForm::GetArgv() {
  TCHAR exeFullPath[256] = {0};
  GetModuleFileName(NULL, exeFullPath, MAX_PATH);

  int len = lstrlenW(exeFullPath);
  if (len == 0) {
    return L"";
  }
  std::wstring command = GetCommandLine();
  return command.substr(len);
}
