#include "log.h"

int main() {
    small_log::Init("log", "main");
    LOG(INFO) << "info";
}
