//
// Created by lamuguo on 8/29/23.
//

#ifndef CANOPEN_LITE_OD_DATA_H
#define CANOPEN_LITE_OD_DATA_H

#include <cstdint>
#include <memory>
#include <string>
#include <variant>

namespace colite {

enum DataType {
    UNKNOWN = 0x0,
    BOOLEAN = 0x1,
    INTEGER8 = 0x2,
    INTEGER16 = 0x3,
    INTEGER32 = 0x4,
    UNSIGNED8 = 0x5,
    UNSIGNED16 = 0x6,
    UNSIGNED32 = 0x7,
    REAL32 = 0x8,
    VISIBLE_STRING = 0x9,
    OCTET_STRING = 0xA,
    UNICODE_STRING = 0xB,
    DOMAIN = 0xF,
    REAL64 = 0x11,
    INTEGER64 = 0x15,
    UNSIGNED64 = 0x1B
};
DataType toDataType(int value);
//int sizeOfDataType(DataType dt);

using DataValue = std::variant<
        int8_t, int16_t, int32_t, int64_t,
        uint8_t, uint16_t, uint32_t, uint64_t,
        bool, float, double,
        std::string
>;

class Data {
public:
    Data() : type(UNKNOWN), value(0) {}
    Data(DataType type, DataValue value) : type(type), value(value) {}
    Data(const Data& other) : type(other.type), value(other.value) {}

    DataType getType() const { return type; }
    const DataValue& getValue() const { return value; }

    template <typename T>
    T get() const {
        return std::get<T>(value);
    }

private:
    DataType type;
    DataValue value;
};

bool operator==(const Data& lhs, const Data& rhs);
std::ostream& operator<<(std::ostream& os, const Data& data);

const Data UNKNOWN_DATA = Data(UNKNOWN, 0);
const Data INT32_ZERO = Data(INTEGER32, static_cast<int32_t>(0));
Data to_int_data(DataType type, int64_t value);

/* Object and related staff. */

enum ObjectType {
    DOMAIN_OBJ = 2,
    VARIABLE_OBJ = 7,
    ARRARY_OBJ = 8,
    RECORD_OBJ = 9
};
ObjectType toObjectType(int value);

class Variable;

class BaseObj {
public:
    BaseObj() = default;
    virtual ~BaseObj() = default;

    virtual ObjectType getType() = 0;
    virtual Variable* get_variable(int subindex) = 0;
    virtual void add_member(int subindex, std::unique_ptr<Variable> var) = 0;

    BaseObj *parent = nullptr;
};

}  // namespace colite

#endif //CANOPEN_LITE_OD_DATA_H
