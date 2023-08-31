//
// Created by lamuguo on 8/28/23.
//
#include "od_variable.h"
#include <locale>
#include <codecvt>
#include <cstring>

namespace colite {

Variable::Variable(const std::string& name, int index, int subindex)
        : name(name), index(index), subindex(subindex) {}

// Copy constructor
Variable::Variable(const Variable& other)
        : parent(other.parent),
          index(other.index),
          subindex(other.subindex),
          name(other.name),
          unit(other.unit),
          factor(other.factor),
          min(other.min),
          max(other.max),
          default_raw(""),
          default_value(other.default_value),
          relative(other.relative),
          data_type(other.data_type),
          access_type(other.access_type),
          description(other.description),
          value_descriptions(other.value_descriptions),
          bit_definitions(other.bit_definitions),
          storage_location(other.storage_location),
          pdo_mappable(other.pdo_mappable) {}

// virtual
Variable* Variable::get_variable(int subindex) {
    return this;
}

bool Variable::operator==(const Variable& other) const {
    return this->index == other.index && this->subindex == other.subindex;
}

int Variable::size() const {
    switch(data_type) {
        case BOOLEAN:
            return sizeof(bool) * 8;
        case INTEGER8:
            return sizeof(int8_t) * 8;
        case INTEGER16:
            return sizeof(int16_t) * 8;
        case INTEGER32:
            return sizeof(int32_t) * 8;
        case INTEGER64:
            return sizeof(int64_t) * 8;
        case UNSIGNED8:
            return sizeof(uint8_t) * 8;
        case UNSIGNED16:
            return sizeof(uint16_t) * 8;
        case UNSIGNED32:
            return sizeof(uint32_t) * 8;
        case UNSIGNED64:
            return sizeof(uint64_t) * 8;
        case REAL32:
            return sizeof(float) * 8;
        case REAL64:
            return sizeof(double) * 8;
            // add other data types if necessary
        default:
            return 8;  // default to 1 byte or 8 bits
    }
}

bool Variable::isWritable() const {
    return access_type.find('w') != std::string::npos;
}

bool Variable::isReadable() const {
    return access_type.find('r') != std::string::npos || access_type == "const";
}

void Variable::addValueDescription(int value, const std::string& descr) {
    value_descriptions[value] = descr;
}

void Variable::addBitDefinition(const std::string& name, const std::vector<int>& bits) {
    bit_definitions[name] = bits;
}
//
//    Data Variable::decode_raw(const std::vector<char>& data) {
//        Data result;
//        if (data_type == nullptr) {
//            throw ObjectDictionaryError("Unknown data type or not handled.");
//        }
//        switch (*data_type) {
//            case VISIBLE_STRING: {
//                std::strcpy(result.s.data(), data.data());
//                break;
//            }
//            case UNICODE_STRING: {
//                // Assuming data is in utf_16_le format
//            std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> conv;
//                std::wstring wideStr = conv.from_bytes(data.data());
//                std::string strData(wideStr.begin(), wideStr.end());
//                std::strcpy(result.s.data(), strData.c_str());
//                break;
//            }
//            case BOOLEAN: {
//                result.b = (data[0] != 0);
//                break;
//            }
//            case INTEGER8: {
//                std::memcpy(&result.i8, data.data(), sizeof(result.i8));
//                break;
//            }
//            case INTEGER16: {
//                std::memcpy(&result.i16, data.data(), sizeof(result.i16));
//                break;
//            }
//            case INTEGER32: {
//                std::memcpy(&result.i32, data.data(), sizeof(result.i32));
//                break;
//            }
//            case INTEGER64: {
//                std::memcpy(&result.i64, data.data(), sizeof(result.i64));
//                break;
//            }
//            case UNSIGNED8: {
//                std::memcpy(&result.u8, data.data(), sizeof(result.u8));
//                break;
//            }
//            case UNSIGNED16: {
//                std::memcpy(&result.u16, data.data(), sizeof(result.u16));
//                break;
//            }
//            case UNSIGNED32: {
//                std::memcpy(&result.u32, data.data(), sizeof(result.u32));
//                break;
//            }
//            case UNSIGNED64: {
//                std::memcpy(&result.u64, data.data(), sizeof(result.i64));
//                break;
//            }
//            default:
//                throw ObjectDictionaryError("Unknown data type or not handled.");
//        }
//        return result;
//    }

}  // namespace colite
