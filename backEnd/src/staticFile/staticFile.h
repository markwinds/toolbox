//
// Created by 14640 on 2024/11/7.
//

#pragma once

#include <string>
#include <vector>

int init_static_file();

int static_file_get_file(const std::string& path, std::vector<char>& data, std::string& mime_type);
