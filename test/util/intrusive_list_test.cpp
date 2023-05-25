#include "small_test.h"
#include "intrusive_list.h"
#include <gtest/gtest.h>
#include <sys/user.h>
#include <vector>

using namespace std;

class TestIntrusiveList: public testing::Test{
};

struct TestNode : public IntrusiveListNode<TestNode> {
    int value = 0;
};

TEST_F(TestIntrusiveList, Test) {
    TestNode head;
    IntrusiveListInit(&head);

    vector<TestNode> nodes(5);
    for (auto &node : nodes) {
        node.value = &node - &nodes[0];
        IntrusiveListInsert(&head, &node);
    }

    auto result = IntrusiveListGetAllNodes(&head);
    ASSERT_EQ(result.size(), 5);
    int i = 4;
    for (auto &node : result) {
        ASSERT_EQ(node->value, i--);
    }

    for (auto &node :nodes) {
        IntrusiveListRemove(&node);
    }
    {
        auto result = IntrusiveListGetAllNodes(&head);
        ASSERT_EQ(result.size(), 0);
    }
}