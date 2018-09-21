#include "small_ioutils.h"

#include <fstream>
#include <sstream>

namespace small_ioutils{
std::string ReadFileName(const std::string &name, std::string &errMsg) {
    std::ifstream file(name);
    if (!file) {
        errMsg = ("open file " + name + " failed");
        return "";
    }
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}
}
