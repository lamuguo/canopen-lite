//
// Created by lamuguo on 8/28/23.
//
#include "od_variable.h"
#include <codecvt>

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

}  // namespace colite
