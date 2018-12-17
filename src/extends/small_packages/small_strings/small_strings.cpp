#include "small_strings.h"

#include <sstream>

namespace small_strings {
std::shared_ptr<std::list<std::string>> 
    split(const std::string &str, const char c) {
    std::istringstream ss(str);
    auto resultS = std::make_shared<std::list<std::string>>();
    bool finish = false;
    while(true) {
        std::string temp;
        finish = static_cast<bool>(std::getline(ss, temp, c)) == false;
        if (finish) {
            break;
        }
        resultS->emplace_back(temp);
    }
    return resultS;
}
std::string TrimLeft(const std::string &str, const std::string &trimStr) {
    std::string result;
    auto index = str.find(trimStr);
    if (index < 0) {
        return result;
    }
    result = str.substr(index, trimStr.size());
    return result;
}
}//namespace small_strings
