#pragma once
#include <map>

class Repo {
public:
  void Init();
  std::map<int, size_t> statistics;
};
