#pragma once

#include "singleton.h"

class Setting : public Singleton<Setting> {

  public:
    int init();

    int reg_http_handler();

  private:
    std::string get_compile_time();
};
