#include <gtest/gtest.h>
#include <vector>

#include "network/socketcan.h"

using namespace colite;

TEST(CANTest, TestSendReceive) {
    SocketCANNetwork network("can0");
    SocketCANNetwork n2("can0");

    // 发送标准ID
    network.send_message(0x123, {1, 2, 3, 4, 5, 6, 7, 8});
    auto frame = n2.recv_message();
    ASSERT_EQ(frame.can_id, 0x123);
    ASSERT_EQ(frame.can_dlc, 8);
    ASSERT_EQ(std::vector<uint8_t>(frame.data, frame.data + 8), std::vector<uint8_t>({1, 2, 3, 4, 5, 6, 7, 8}));

    // 发送扩展ID
    network.send_message(0x12345, {});
    frame = n2.recv_message();
    ASSERT_EQ(frame.can_id, 0x12345);
    ASSERT_EQ(frame.can_dlc, 0);
}
