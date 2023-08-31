//
// Created by lamuguo on 8/28/23.
//

#ifndef CANOPEN_LITE_OD_ARRAY_H
#define CANOPEN_LITE_OD_ARRAY_H

#include <memory>
#include "od_variable.h"

namespace colite {

// TODO(lamuguo): Rename to ODArray
class Array : public BaseObj {
public:
    Array(std::string name, int index)
    : parent(nullptr), index(index), name(name), storage_location("") {}

    virtual ObjectType getType() { return ARRARY_OBJ; }
    virtual Variable* get_variable(int subindex);
    virtual void add_member(int subindex, std::unique_ptr<Variable> var);

    Variable* operator[](const std::string& subindex);

    inline size_t size() const { return subindices.size(); }

    std::vector<int> keys() const;

    bool operator==(const Array& other) const;

    void add_member(std::unique_ptr<Variable> variable);

    // Member variables
    void* parent;
    int index;
    std::string name;
    std::string storage_location;
    std::unordered_map<int, std::unique_ptr<Variable>> subindices;
    std::unordered_map<std::string, int> name_to_subindex;
};  // class Array

}  // namespace colite

#endif //CANOPEN_LITE_OD_ARRAY_H
