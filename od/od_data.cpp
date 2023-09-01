//
// Created by lamuguo on 8/29/23.
//

#include "od_data.h"

namespace colite {

ObjectType toObjectType(int value) {
    switch (value) {
        case 2: return DOMAIN_OBJ;
        case 7: return VARIABLE_OBJ;
        case 8: return ARRARY_OBJ;
        case 9: return RECORD_OBJ;
        default: return VARIABLE_OBJ;
    }
}

DataType toDataType(int value) {
    switch(value) {
        case 0x1: return BOOLEAN;
        case 0x2: return INTEGER8;
        case 0x3: return INTEGER16;
        case 0x4: return INTEGER32;
        case 0x5: return UNSIGNED8;
        case 0x6: return UNSIGNED16;
        case 0x7: return UNSIGNED32;
        case 0x8: return REAL32;
        case 0x9: return VISIBLE_STRING;
        case 0xA: return OCTET_STRING;
        case 0xB: return UNICODE_STRING;
        case 0xF: return DOMAIN;
        case 0x11: return REAL64;
        case 0x15: return INTEGER64;
        case 0x1B: return UNSIGNED64;
        default: return UNKNOWN;
    }
}

// TODO(lamuguo): use uint64_t instead.
Data to_int_data(DataType type, int64_t value) {
    switch (type) {
        case INTEGER8: return Data(type, static_cast<int8_t>(value));
        case INTEGER16: return Data(type, static_cast<int16_t>(value));
        case INTEGER32: return Data(type, static_cast<int32_t>(value));
        case INTEGER64: return Data(type, static_cast<int64_t>(value));
        case UNSIGNED8: return Data(type, static_cast<uint8_t>(value));
        case UNSIGNED16: return Data(type, static_cast<uint16_t>(value));
        case UNSIGNED32: return Data(type, static_cast<uint32_t>(value));
        case UNSIGNED64: return Data(type, static_cast<uint64_t>(value));
        default:
            return UNKNOWN_DATA;
    }
}

bool operator==(const Data& lhs, const Data& rhs) {
    return lhs.getDataType() == rhs.getDataType() && lhs.getValue() == rhs.getValue();
}

/*
 * Code for debugging below.
 */
// Convert DataType enum to string
std::string dataTypeToString(DataType type) {
    switch (type) {
        case UNKNOWN: return "UNKNOWN";
        case BOOLEAN: return "BOOLEAN";
        case INTEGER8: return "INTEGER8";
        case INTEGER16: return "INTEGER16";
        case INTEGER32: return "INTEGER32";
        case UNSIGNED8: return "UNSIGNED8";
        case UNSIGNED16: return "UNSIGNED16";
        case UNSIGNED32: return "UNSIGNED32";
        case REAL32: return "REAL32";
        case VISIBLE_STRING: return "VISIBLE_STRING";
        case OCTET_STRING: return "OCTET_STRING";
        case UNICODE_STRING: return "UNICODE_STRING";
        case DOMAIN: return "DOMAIN";
        case REAL64: return "REAL64";
        case INTEGER64: return "INTEGER64";
        case UNSIGNED64: return "UNSIGNED64";
        default: return "UNDEFINED";
    }
}

// Overload the operator<< for Data:
std::ostream& operator<<(std::ostream& os, const Data& data) {
    os << "Data { type: " << dataTypeToString(data.getDataType()) << ", value: ";

    // Print the correct value based on the type
    switch (data.getDataType()) {
        case BOOLEAN: os << data.get<bool>(); break;
        case INTEGER8: os << data.get<uint8_t>(); break;
        case INTEGER16: os << data.get<int16_t>(); break;
        case INTEGER32: os << data.get<int32_t>(); break;
        case INTEGER64: os << data.get<int64_t>(); break;
        case UNSIGNED8: os << data.get<int8_t>(); break;
        case UNSIGNED16: os << data.get<uint16_t>(); break;
        case UNSIGNED32: os << data.get<uint32_t>(); break;
        case UNSIGNED64: os << data.get<uint64_t>(); break;
        case REAL32: os << data.get<float>(); break;
        case REAL64: os << data.get<double>(); break;
        case VISIBLE_STRING: os << data.get<std::string>(); break;
        default: os << "UNDEFINED"; break; // You might want to add more cases for other DataTypes
    }

    os << " }";
    return os;
}

}  // namespace colite
