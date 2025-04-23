//
// Created by 14640 on 2024/11/7.
//

#pragma once

#include <string>
#include <vector>

int initStaticFile();

int staticFileGetFile(const std::string& path, std::vector<char>& data, std::string& mime_type);
