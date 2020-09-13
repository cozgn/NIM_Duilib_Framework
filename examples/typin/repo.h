#pragma once
#include <map>

class Repo {
public:
  void Init();
  std::map<int, size_t> statistics;
};

struct KeyboardRecord {
  int virtual_key_code;
  int64_t millisecond;
};

class SqliteDatabase {
public:
  static bool Insert(const std::wstring &db, std::vector<KeyboardRecord> &v);
  static void Select(const std::wstring &db, std::map<int, int64_t>* data);
};
