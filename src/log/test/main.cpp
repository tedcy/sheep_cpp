#include "log.h"

using namespace small_log;
int main() {
    small_log::Init("log", "main");
    LOG(INFO) << "info";
}
