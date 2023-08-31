#include <gtest/gtest.h>
#include <string>

#include "fmt/core.h"

#include "od/eds.h"  // Replace with the actual path to your import_od header

const std::string EDS_PATH = fmt::format("{}{}", PROJECT_DIR, "/tests/sample.eds");

using namespace colite;

void PrintTo(const Data& data, std::ostream* os) {
    *os << data;  // Here we utilize the previously defined operator<< for the Data type.
}

class TestEDS : public ::testing::Test {
protected:
    colite::ObjectDictionary od;

    virtual void SetUp() override {
        od = colite::import_eds(EDS_PATH, 2);
    }
};

TEST_F(TestEDS, LoadNonExistingFile) {
    EXPECT_THROW(colite::import_eds("/path/to/wrong_file.eds", 2), std::runtime_error);
}

TEST_F(TestEDS, TestVariable) {
    Variable *var = od.get<Variable>(std::string("Producer heartbeat time"));
    EXPECT_TRUE(var != nullptr);
    EXPECT_EQ(var->index, 0x1017);
    EXPECT_EQ(var->subindex, 0);
    EXPECT_EQ(var->name, "Producer heartbeat time");
    EXPECT_EQ(var->data_type, UNSIGNED16);
    EXPECT_EQ(var->access_type, "rw");
    EXPECT_EQ(var->default_value, to_int_data(UNSIGNED16, 0));
    EXPECT_FALSE(var->relative);
}

TEST_F(TestEDS, TestCalculateWithNodeId) {
    EXPECT_EQ(Data(INTEGER32, 102), to_int_with_node_id(2, INTEGER32, "$NODEID+100"));
    EXPECT_EQ(Data(INTEGER32, 102), to_int_with_node_id(2, INTEGER32, "$NODEID + 100"));
    EXPECT_EQ(Data(INTEGER32, 102), to_int_with_node_id(2, INTEGER32, "100 + $NODEID"));
    EXPECT_EQ(Data(INTEGER32, 100), to_int_with_node_id(2, INTEGER32, "100"));
    EXPECT_EQ(Data(UNSIGNED8, static_cast<uint8_t>(102)), to_int_with_node_id(2, UNSIGNED8, "$NODEID+100"));
    EXPECT_EQ(Data(UNSIGNED8, static_cast<uint8_t>(102)), to_int_with_node_id(2, UNSIGNED8, "$NODEID + 100"));
    EXPECT_EQ(Data(UNSIGNED8, static_cast<uint8_t>(102)), to_int_with_node_id(2, UNSIGNED8, "100 + $NODEID"));
    EXPECT_EQ(Data(UNSIGNED8, static_cast<uint8_t>(100)), to_int_with_node_id(2, UNSIGNED8, "100"));
}

TEST_F(TestEDS, TestRelativeVariable) {
    Record *record = od.get<Record>("Receive PDO 0 Communication Parameter");
    Variable *var = record->get_variable("COB-ID use by RPDO 1");
    EXPECT_TRUE(var->relative);
    EXPECT_EQ(var->default_value, to_int_data(UNSIGNED32, 512 + od.node_id));
}

TEST_F(TestEDS, TestRecord) {
    Record *record = od.get<Record>("Identity object");
    EXPECT_TRUE(record != nullptr);
    EXPECT_EQ(record->size(), 5);
    EXPECT_EQ(record->index, 0x1018);
    EXPECT_EQ(record->name, "Identity object");

    Variable *var = record->get_variable("Vendor-ID");
    EXPECT_TRUE(var != nullptr);
    EXPECT_EQ(var->name, "Vendor-ID");
    EXPECT_EQ(var->index, 0x1018);
    EXPECT_EQ(var->subindex, 1);
    EXPECT_EQ(var->data_type, UNSIGNED32);
    EXPECT_EQ(var->access_type, "ro");
}

TEST_F(TestEDS, IntegerConvert) {
    EXPECT_EQ(to_int_data(INTEGER8, 1), to_data(INTEGER8, "0x1"));
    EXPECT_EQ(to_int_data(INTEGER8, 127), to_data(INTEGER8, "0x7f"));
    EXPECT_EQ(to_int_data(INTEGER8, -128), to_data(INTEGER8, "0x80"));
    EXPECT_EQ(to_int_data(INTEGER8, -1), to_data(INTEGER8, "0xFF"));
    EXPECT_EQ(to_int_data(INTEGER32, 1), to_data(INTEGER32, "0x1"));
    EXPECT_EQ(to_int_data(INTEGER32, -1), to_data(INTEGER32, "0xFFFFFFFF"));
    EXPECT_EQ(to_int_data(INTEGER32, 2147483647), to_data(INTEGER32, "0x7FFFFFFF"));
    EXPECT_EQ(to_int_data(INTEGER32, -2147483648), to_data(INTEGER32, "0x80000000"));
    EXPECT_EQ(to_int_data(INTEGER64, -1), to_data(INTEGER64, "0xFFFFFFFFFFFFFFFF"));
    EXPECT_EQ(to_int_data(INTEGER64, 9223372036854775807), to_data(INTEGER64, "0x7FFFFFFFFFFFFFFF"));
    EXPECT_EQ(to_int_data(INTEGER64, LLONG_MIN), to_data(INTEGER64, "0x8000000000000000"));
    EXPECT_EQ(to_int_data(INTEGER64, 0), to_data(INTEGER64, "0x0000000000000000"));
    EXPECT_EQ(to_int_data(INTEGER64, LLONG_MIN + 9), to_data(INTEGER64, "0x8000000000000009"));
}


TEST_F(TestEDS, TestRecordWithLimits) {
    auto int8 = od.get<Variable>(0x3020);
    EXPECT_EQ(to_int_data(INTEGER8, 0), int8->min);
    EXPECT_EQ(to_int_data(INTEGER8, 127), int8->max);

    auto uint8 = od.get<Variable>(0x3021);
    EXPECT_EQ(to_int_data(UNSIGNED8, 2), uint8->min);
    EXPECT_EQ(to_int_data(UNSIGNED8, 10), uint8->max);

    auto int32 = od.get<Variable>(0x3030);
    EXPECT_EQ(to_int_data(INTEGER32, -1), int32->min);
    EXPECT_EQ(to_int_data(INTEGER32, 0), int32->max);

    auto int64 = od.get<Variable>(0x3040);
    EXPECT_EQ(to_int_data(INTEGER64, -9223372036854775799), int64->min);
    EXPECT_EQ(to_int_data(INTEGER64, 10), int64->max);
}

TEST_F(TestEDS, TestArrayCompactSubObj) {
    auto array = od.get<Array>(0x1003);
    EXPECT_TRUE(array != nullptr); // assuming you have an Array type
    EXPECT_EQ(array->index, 0x1003);
    EXPECT_EQ(array->name, "Pre-defined error field");

    auto var = array->get_variable(5);
    EXPECT_TRUE(var != nullptr); // assuming you have a Variable type
    EXPECT_EQ(var->name, "Pre-defined error field_5");
    EXPECT_EQ(var->index, 0x1003);
    EXPECT_EQ(var->subindex, 5);
    EXPECT_EQ(var->data_type, UNSIGNED32); // assuming you have this defined somewhere
    EXPECT_EQ(var->access_type, "ro");
}
TEST_F(TestEDS, ExplicitNameSubobj) {
    auto arr = od.get<Array>(0x3004);
    EXPECT_EQ(arr->name, "Sensor Status");
    auto sub_1 = arr->get_variable(1);
    EXPECT_EQ(sub_1->name, "Sensor Status 1");
    auto sub_3 = arr->get_variable(3);
    EXPECT_EQ(sub_3->name, "Sensor Status 3");
    EXPECT_EQ(sub_3->default_value, to_int_data(UNSIGNED16, 3));
}

TEST_F(TestEDS, ParameterNameWithPercent) {
    auto obj = od.get<Array>(0x3003);
    EXPECT_EQ(obj->name, "Valve % open");
}

TEST_F(TestEDS, CompactSubobjParameterNameWithPercent) {
    auto obj = od.get<Array>(0x3006);
    EXPECT_EQ(obj->name, "Valve 1 % Open");
}

TEST_F(TestEDS, SubIndexWithCapitalS) {
    auto obj = od.get<Record>(0x3010);
    auto sub_obj = obj->get_variable(0);
    EXPECT_EQ(sub_obj->name, "Temperature");
}

TEST_F(TestEDS, DummyVariable) {
    auto var = od.get<Variable>("Dummy0003");
    EXPECT_TRUE(dynamic_cast<Variable*>(var) != nullptr);
    EXPECT_EQ(var->index, 0x0003);
    EXPECT_EQ(var->subindex, 0);
    EXPECT_EQ(var->name, "Dummy0003");
    EXPECT_EQ(var->data_type, INTEGER16); // Assuming you have a similar data type enum like `canopen.objectdictionary`
    EXPECT_EQ(var->access_type, "const");
    EXPECT_EQ(var->size(), 16);
}

TEST_F(TestEDS, DummyVariableUndefined) {
    try {
        od.get<Variable>("Dummy0001");
    } catch (const std::out_of_range& e) {
        EXPECT_STREQ(e.what(), "Index/name Dummy0001 not found.");
    }
}

TEST_F(TestEDS, Comments) {
    std::string expected_comment = "|-------------|\n| Don't panic |\n|-------------|";
    EXPECT_EQ(od.comments, expected_comment);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
