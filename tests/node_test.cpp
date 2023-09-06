#include <thread>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "node/canopen_node.h"

using namespace colite;

const std::string EDS_PATH = fmt::format("{}{}", PROJECT_DIR, "/tests/sample.eds");


TEST(CANopenNodeTest, TestExpeditedUpload) {
    CANopenNode node("can0", 2, EDS_PATH);
    SDOClient client("can0", 2);

    std::thread node_thread([&]() {
        node.run();
    });

    auto var = client.expedited_upload(2, 0x1018, 1);
    ASSERT_EQ(var->default_value, to_int_data(UNSIGNED32, 4));
//
//    auto trans_type = client.expedited_upload(2, 0x1400, 1);
//    ASSERT_EQ(trans_type->default_value, to_int_data(INTEGER32, 0xfe));

    node_thread.join();
}
