#pragma once
#include <map>

class Repo {
public:
  void Init();
  std::map<int, size_t> statistics;
};

struct KeyboardRecord {
  uint64_t virtual_key_code;
  uint64_t millisecond;
};

class SqliteDatabase {
public:
  SqliteDatabase(const std::wstring &db);
  virtual ~SqliteDatabase();

  bool Insert(std::vector<KeyboardRecord> &v);
};
