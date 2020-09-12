#include "stdafx.h"
#include "repo.h"
#include <winsqlite/winsqlite3.h>
#include <iostream>
#include "typedef.h"

void Repo::Init() {

}

SqliteDatabase::SqliteDatabase(const std::wstring &db) {

}

SqliteDatabase::~SqliteDatabase() {}

bool SqliteDatabase::Insert(std::vector<KeyboardRecord> &v) {
  progress_timer a(L"sqlite3 insert");
  sqlite3 *DB;
  std::string sql =
      "CREATE TABLE if not exists keyboard("
      "id integer PRIMARY KEY AUTOINCREMENT NOT NULL, "
      "virtual_key_code unsigned int  NOT NULL, "
      "created_at  unsigned int   NOT NULL);";
  int exit = 0;
  exit = sqlite3_open("keyboard.db", &DB);
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
