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
    OutputDebugString(L"Error Create Table\n");
    OutputDebugStringA(messaggeError);
    sqlite3_free(messaggeError);
  } else {
    OutputDebugString(L"Table created Successfully\n");
  }

  std::string sql_insert;
  for (auto& r : v) {
    std::string str = format(
    "insert into keyboard ('virtual_key_code', 'created_at') values (%d, %d);", 
    r.virtual_key_code, r.millisecond);
    exit = sqlite3_exec(DB, str.c_str(), NULL, 0, &messaggeError);
    if (exit != SQLITE_OK) {
      OutputDebugString(L"insert failed\n");
      OutputDebugStringA(messaggeError);
      sqlite3_free(messaggeError);
    } else {
      OutputDebugString(L"insert ok\n");
    }
  }

  sqlite3_close(DB);
  return true; 
}
