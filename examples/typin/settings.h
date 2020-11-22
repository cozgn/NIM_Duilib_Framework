#pragma once
#include <string>

class BootStart {
public:
    BootStart();
public:
    static bool setEnable(bool enable);
    static bool isEnable();

    virtual ~BootStart();

private:
    const static std::wstring REG_RUN;
};
