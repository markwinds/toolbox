#pragma once

#include <curl.h>
#include <functional>
#include <string>

size_t curlRespStr(void* contents, size_t size, size_t nmemb, std::string* userp);
size_t curlRespFile(void* contents, size_t size, size_t nmemb, FILE** userp);

void configCurl(CURL* curl);

void writeData2File(const std::string& entryName, unsigned char* data, size_t dataLen);

int decompressZipData(
    const unsigned char* compressed_data,
    size_t               compressed_length,
    const std::function<void(std::string entryName, unsigned char* data, size_t dataLen)>&
        callback = writeData2File);

int decompressZipData(
    const std::wstring& filepath,
    const std::function<void(std::string entryName, unsigned char* data, size_t dataLen)>&
        callback = writeData2File);

void         restartProgram();
std::wstring getExecutablePath();

std::wstring string_to_wstring(const std::string& str);
std::string  wstring_to_string(const std::wstring& wstr);