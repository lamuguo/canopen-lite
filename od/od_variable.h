//
// Created by lamuguo on 8/28/23.
//

#ifndef CANOPEN_LITE_OD_VARIABLE_H
#define CANOPEN_LITE_OD_VARIABLE_H

#include <cstring>  // For std::memcpy
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "od_data.h"

namespace colite {

class ObjectDictionaryError {
public:
    std::string message = "Unsupported operation with the current Object Dictionary.";

    ObjectDictionaryError(std::string message) : message(message) {}
};  // class ObjectDictionaryError

class DeviceInformation {
    public:
    std::set<int> allowed_baudrates;
    std::string vendor_name;
    int vendor_number;
    std::string product_name;
    int product_number;
    int revision_number;
    std::string order_code;
    bool simple_boot_up_master;
    bool simple_boot_up_slave;
    bool granularity;
    bool dynamic_channels_supported;
    bool group_messaging;
    int nr_of_RXPDO;
    int nr_of_TXPDO;
    bool LSS_supported;
};  // class DeviceInformation


const std::set<DataType> SIGNED_TYPES {INTEGER8, INTEGER16, INTEGER32, INTEGER64};
const std::set<DataType> UNSIGNED_TYPES {UNSIGNED8, UNSIGNED16, UNSIGNED32, UNSIGNED64};
const std::set<DataType> FLOAT_TYPES {REAL32, REAL64};
const std::set<DataType> DATA_TYPES {VISIBLE_STRING, OCTET_STRING, UNICODE_STRING, DOMAIN};
const std::set<DataType> INTEGER_TYPES {INTEGER8, INTEGER16, INTEGER32, INTEGER64, UNSIGNED8, UNSIGNED16, UNSIGNED32, UNSIGNED64};
const std::set<DataType> NUMBER_TYPES {INTEGER8, INTEGER16, INTEGER32, INTEGER64, UNSIGNED8, UNSIGNED16, UNSIGNED32, UNSIGNED64, REAL32, REAL64};
const std::set<DataType> ALL_DATA_TYPES {
    BOOLEAN, INTEGER8, INTEGER16, INTEGER32, REAL32, VISIBLE_STRING, OCTET_STRING,
    UNICODE_STRING, DOMAIN, REAL64, INTEGER64, UNSIGNED64
};
const std::map<DataType, std::string> STRUCT_TYPES = {
        { BOOLEAN, "?" },
        { INTEGER8, "b" },
        { INTEGER16, "<h" },
        { INTEGER32, "<l" },
        { INTEGER64, "<q" },
        { UNSIGNED8, "B" },
        { UNSIGNED16, "<H" },
        { UNSIGNED32, "<L" },
        { UNSIGNED64, "<Q" },
        { REAL32, "<f" },
        { REAL64, "<d" }
};

class Variable : public BaseObj {
public:
    Variable(const std::string& name, int index, int subindex = 0);
    Variable(const Variable& other);

    virtual ObjectType getType() { return VARIABLE_OBJ; }
    virtual Variable* get_variable(int subindex);
    virtual void add_member(int subindex, std::unique_ptr<Variable> var) {
        // Do nothing
        // TODO(lamuguo): assert here?
    }
    int size() const;

    bool operator==(const Variable& other) const;

    bool isWritable() const;
    bool isReadable() const;
    void addValueDescription(int value, const std::string& descr);
    void addBitDefinition(const std::string& name, const std::vector<int>& bits);

    // Member variables
    // TODO(lamuguo): remove parent.
    void* parent = nullptr;    // Pointer to some parent structure
    int index;
    int subindex;
    std::string name;
    std::string unit = "";
    float factor = 1;
    Data min = INT32_ZERO;
    Data max = INT32_ZERO;
    std::string default_raw;

    Data default_value;
    bool relative = false;
    std::string parameter_raw;
    Data parameter_value;
    DataType data_type = UNKNOWN;
    std::string access_type = "rw";
    std::string description = "";
    std::unordered_map<int, std::string> value_descriptions;
    std::unordered_map<std::string, std::vector<int>> bit_definitions;
    std::string storage_location;
    bool pdo_mappable = false;
//
//    Data decode_raw(const std::vector<char>& data);
//    std::vector<char> encode_raw(const Data& value);
//    Data decode_phys(const Data& value);
//    Data encode_phys(const Data& value);
//    std::string decode_desc(int value);
//    int encode_desc(const std::string& desc);
//    int decode_bits(int value, const std::vector<int>& bits);
//    int encode_bits(int original_value, const std::vector<int>& bits, int bit_value);
};  // class Variable

}  // namespace colite

#endif //CANOPEN_LITE_OD_VARIABLE_H