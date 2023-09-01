//
// Created by lamuguo on 8/28/23.
//

#ifndef CANOPEN_LITE_OD_VARIABLE_H
#define CANOPEN_LITE_OD_VARIABLE_H

#include <assert.h>
#include <cstring>  // For std::memcpy
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "od_data.h"

namespace colite {

class Variable : public BaseObj {
public:
    Variable(const std::string& name, int index, int subindex = 0);
    Variable(const Variable& other);

    virtual ObjectType getType() { return VARIABLE_OBJ; }
    virtual Variable* get_variable(int subindex);
    virtual void add_member(std::unique_ptr<Variable> var) {
        // Do nothing
        assert(false);
    }
    int size() const;

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
};  // class Variable

}  // namespace colite

#endif //CANOPEN_LITE_OD_VARIABLE_H