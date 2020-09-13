#include "stdafx.h"
#include "repo.h"
#include <winsqlite/winsqlite3.h>
#include <iostream>
#include "typedef.h"

void Repo::Init() {

}

bool SqliteDatabase::Insert(const std::wstring &db, std::vector<KeyboardRecord> &v) {
  progress_timer a(L"sqlite3 insert");
  sqlite3 *DB;
  std::string sql =
      "CREATE TABLE if not exists keyboard("
      "id integer PRIMARY KEY AUTOINCREMENT NOT NULL, "
      "virtual_key_code unsigned int  NOT NULL, "
      "created_at  unsigned int   NOT NULL);";
  int exit = 0;
  exit = sqlite3_open16(db.c_str(), &DB);
  if (exit != SQLITE_OK) {
    LOGE(L"failed to open db");
    return false;
  }
  char *messaggeError;
  exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

  if (exit != SQLITE_OK) {
    LOGV(L"Error Create Table");
    OutputDebugStringA(messaggeError);
    sqlite3_free(messaggeError);
  } else {
    LOGV(L"Table created Successfully");
  }

  std::string sql_insert;
  sqlite3_exec(DB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
  for (auto& r : v) {
    std::string str = format(
    "insert into keyboard ('virtual_key_code', 'created_at') values (%d, %d);", 
    r.virtual_key_code, r.millisecond);
    exit = sqlite3_exec(DB, str.c_str(), NULL, 0, &messaggeError);
    if (exit != SQLITE_OK) {
      LOGV(L"insert failed");
      OutputDebugStringA(messaggeError);
      sqlite3_free(messaggeError);
    } else {
      LOGV(L"insert ok");
    }
  }
  sqlite3_exec(DB, "END TRANSACTION;", NULL, NULL, NULL);
  sqlite3_close(DB);
  return true; 
}


static int result_row_callback(void* param, 
                           int columns, 
                           char** column_values, 
                           char** column_names) {
  auto *v = (std::map<int, int64_t>*) param;
  if (columns != 2) {
    LOGE(L"columns is %d", columns);
    return 0;
  }
  //KeyboardRecord r = {atoi(column_values[0]), atoll(column_values[1])};
  int vk = atoi(column_values[0]);
  (*v)[vk] = ((*v)[vk]) + 1;
  //for (int i = 0; i < columns; ++i) {
    //OutputDebugStringA(column_names[i]);
    //OutputDebugStringA(" = ");
    //OutputDebugStringA(column_values[i]);
    //OutputDebugStringA("\n");
  //}
  return 0;
}

void SqliteDatabase::Select(const std::wstring &db, std::map<int, int64_t> *data) {
  sqlite3 *DB;
  int ret = 0;
  ret = sqlite3_open16(db.c_str(), &DB);
  if (ret != SQLITE_OK) {
    LOGI(L"failed to open db");
    return;
  }
  char *error_msg;
  std::string sql_insert;
  ret = sqlite3_exec(DB, 
  "select virtual_key_code, created_at from keyboard" , result_row_callback, data, &error_msg);
    if (ret != SQLITE_OK) {
      LOGV(L"select failed");
      OutputDebugStringA(error_msg);
      sqlite3_free(error_msg);
    } else {
      LOGV(L"insert ok");
    }
  sqlite3_close(DB);
}
