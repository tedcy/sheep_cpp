#include "connector_test.h"
#include "client_test.h"
#include "client_pool_test.h"
#include "small_test.h"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    auto result = RUN_ALL_TESTS();
    block_server::ServerThread::GetInstance().Stop();
    return result;
}
