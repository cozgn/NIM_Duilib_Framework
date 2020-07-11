#include "stdafx.h"
#include "main_form.h"
#include <fstream>

#include "httplib.h"
#include "json.hpp"

#define UI_TEST 0

const std::wstring MainForm::kClassName = L"Update";

void Toast(const std::wstring &msg) {
  MessageBox(NULL, msg.c_str(), L"", 0);
}

std::string get_host_from_url(const std::string &url) {
  int pos1 = url.find("://");
  if (pos1 == std::string::npos) {
    return "";
  }
  pos1 += 3;
  int pos2 = url.find("/", pos1);
  return url.substr(pos1, pos2 - pos1);
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
  ShowWindow(false);

  label_title_ = dynamic_cast<ui::Label*>(FindControl(L"title"));
  progress_bar_ = dynamic_cast<ui::Progress*>(FindControl(L"progress"));
  label_info_ = dynamic_cast<ui::Label*>(FindControl(L"info"));
  label_total_ = dynamic_cast<ui::Label*>(FindControl(L"total"));
  btn_restart_ = dynamic_cast<ui::Button*>(FindControl(L"btn_restart"));
  layout_progress_ = dynamic_cast<ui::Box*>(FindControl(L"layout_progress"));

#if UI_TEST
  return ;
#endif
  label_total_->SetText(L"");
  label_info_->SetText(L"");
  progress_bar_->SetValue(0);
  btn_restart_->SetVisible(false);

  btn_restart_->AttachClick([this](ui::EventArgs *args) -> bool {
    ShowWindow(false);
    LaunchProgram(update_desc_.launcher);
    Close();
    return true;
  });
  if (!ParseCommandLine()) {
    Close();
    return;
  }

  Download();
}

LRESULT MainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam,
                          BOOL& bHandled) {
  PostQuitMessage(0L);
  return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void MainForm::OnProgressValueChagned(float value) {}

bool MainForm::ParseCommandLine() {
  //Toast(GetArgv());
  json::JSON obj = json::JSON::Load(nbase::UTF16ToUTF8(GetArgv()));
  if (!obj.hasKey("title")) {
    return false;
  }
  label_title_->SetText(nbase::UTF8ToUTF16(obj["title"].ToString()));

  if (!obj.hasKey("launcher")) {
    return false;
  }
  update_desc_.launcher = nbase::UTF8ToUTF16(obj["launcher"].ToString());

  if (!obj.hasKey("description")) {
    return false;
  }
  for (auto des : obj["description"].ArrayRange()) {
    FileDescription f;
    f.url = nbase::UTF8ToUTF16(des["url"].ToString());
    f.replace = nbase::UTF8ToUTF16(des["replace"].ToString());
    //Toast(f.url);
    //Toast(f.replace);
    update_desc_.files_desc.push_back(f);
  }
  return update_desc_.files_desc.size() > 0; 
}

void MainForm::Download() {
  StdClosure task = [this]() {
    int retry_count = 0;
    for (size_t i = 0; i < update_desc_.files_desc.size(); ++i) {
      SetTotal(i + 1, update_desc_.files_desc.size());
      auto& desc = *(update_desc_.files_desc.begin() + i);
      std::wstring tmp_file = desc.tmp.empty() ? CreateTempFile() : desc.tmp;
      if (tmp_file.empty()) {
        SetInfo(L"无法创建临时文件");
        return;
      }
      desc.tmp = tmp_file;
      std::ofstream of(tmp_file, std::ios::out | std::ios::binary);
      if (!of.is_open()) {
        SetInfo(L"无法打开临时文件");
        return;
      }

      std::string url = nbase::UTF16ToUTF8(desc.url);
      std::string host = get_host_from_url(url);
      httplib::Client cli(host);
      cli.set_connection_timeout(3, 0);
      auto progress = [this](uint64_t len, uint64_t total) {
        static int last_percent = -1;
        int percent = (int)(len * 100 / total);
        if (percent != last_percent) {
          last_percent = percent;
          nbase::ThreadManager::PostTask(kThreadUI, [this, percent]() {
            progress_bar_->SetValue(percent);
            label_info_->SetText(nbase::StringPrintf(L"%d%%", percent));
          });
        }
        return true;
      };

      auto content_receiver = [&of](const char *data, size_t data_length) -> bool {
        of.write(data, data_length);
        return true;
      };
      std::shared_ptr<httplib::Response> res = 
        cli.Get(get_path_from_url(url).c_str(), content_receiver, progress);
      if (!res) {
        SetInfo(nbase::StringPrintf(L"无法连接Host: %s", nbase::UTF8ToUTF16(host).c_str()));
        return;
      }
      of.close();
      if (res->status != 200) {
        if (retry_count < 3) {
          --i;
          ++retry_count;
          continue;
        }
        SetInfo(nbase::StringPrintf(L"下载失败: %d", res->status));
        return;
      }
      retry_count = 0;
    }
    OnDownloadComplete();
    for (auto desc : update_desc_.files_desc) {
      nbase::DeleteFileW(desc.tmp);
    }
  };
  nbase::ThreadManager::PostTask(kThreadGlobalMisc, task);
}

std::wstring MainForm::CreateTempFile() {
  TCHAR szPathName[MAX_PATH];
  TCHAR szFileName[MAX_PATH];
  if (!::GetTempPath(MAX_PATH, szPathName)) {
    return L"";
  }
  //创建临时文件名并在目录中创建文件
  if (!::GetTempFileName(szPathName, L"fsauth.tmp", 0, szFileName)) {
    return L"";
  }
  return szFileName;
}

void MainForm::OnDownloadComplete() {
  // check crc32 
  nbase::ThreadManager::PostTask(kThreadUI, [this](){
     progress_bar_->SetVisible(false);
     layout_progress_->SetVisible(false);
     btn_restart_->SetVisible(true);
     label_title_->SetVisible(false);
  });

  SetInfo(nbase::StringPrintf(L"正在升级"));
  for (auto des : update_desc_.files_desc) {
    bool r = CopyFile(des.tmp.c_str(), des.replace.c_str(), false);
    if (!r) {
      SetInfo(nbase::StringPrintf(L"写入文件失败: %d", GetLastError()));
      return;
    }
  }
  SetInfo(L"升级完成");
}

std::wstring MainForm::GetArgv() {
  TCHAR exe[256] = {0};
  GetModuleFileName(NULL, exe, MAX_PATH);
  int len = lstrlenW(exe);
  if (len == 0) {
    return L"";
  }
  std::wstring command = GetCommandLine();
  //if (command[0] == L'\"') {
  //  command = command.substr(1);
  //}
  //if (command[command.length() - 2] == L'\"') {
  //  command = command.substr(0, command.length() - 1);
  //}
  return command.substr(command.find(exe) + len);
}

void MainForm::SetInfo(const std::wstring &msg) {
  nbase::ThreadManager::PostTask(kThreadUI, [this, msg](){
    label_info_->SetText(msg);
  });
}

void MainForm::SetTotal(int current, int total) {
  nbase::ThreadManager::PostTask(kThreadUI, [this, current, total](){
    label_total_->SetText(nbase::StringPrintf(L"%d/%d", current, total));
  });

}
