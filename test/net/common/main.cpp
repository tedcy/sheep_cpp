#include "asyncer_test.h"
#include "event_test.h"
#include "timer_test.h"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
