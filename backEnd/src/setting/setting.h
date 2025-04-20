#pragma once
#include <string>

class Setting {

  public:
    static int init();

    static int reg_http_handler();

  private:
    static std::string get_compile_time();
};
