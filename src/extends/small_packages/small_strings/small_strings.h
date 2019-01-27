#pragma once

#include <string>
#include <list>
#include <memory>
#include <sstream>

namespace small_strings {
inline std::list<std::string> 
    split(const std::string &str, const char c) {
    std::istringstream ss(str);
    std::list<std::string> resultS;
    bool finish = false;
    while(true) {
        std::string temp;
        finish = static_cast<bool>(std::getline(ss, temp, c)) == false;
        if (finish) {
            break;
        }
        resultS.push_back(std::move(temp));
    }
    return resultS;
}
inline std::string TrimLeft(const std::string &str, const std::string &trimStr) {
    std::string result;
    auto index = str.find(trimStr);
    if (index < 0) {
        return result;
    }
    result = str.substr(index + trimStr.size(), str.size() - trimStr.size());
    return result;
}
}//namespace small_strings
