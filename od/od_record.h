#ifndef CANOPEN_LITE_OD_RECORD_H
#define CANOPEN_LITE_OD_RECORD_H

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include "od_variable.h"

namespace colite {

class Record : public BaseObj {
public:
    Record(std::string name, int index)
            : parent(nullptr), index(index), name(name), storage_location("") {}

    virtual ObjectType getType() { return RECORD_OBJ; }
    virtual Variable* get_variable(int subindex);
    virtual void add_member(int subindex, std::unique_ptr<Variable> var);

    Variable* get_variable(const std::string& name);

    Variable* operator[](const std::string& subindex);

    Variable* operator[](int subindex);

    void insert(int subindex, std::unique_ptr<Variable> var);

    void erase(const std::string& subindex);

    size_t size() const;

    std::vector<int> keys() const;

    bool contains(const std::string& subindex) const;

    bool operator==(const Record& other) const;

    void add_member(std::unique_ptr<Variable> variable);

    // Member variables
    void* parent;
    int index;
    std::string name;
    std::string storage_location;
    std::unordered_map<int, std::unique_ptr<Variable>> subindices;
    std::unordered_map<std::string, int> name_to_subindex;
};

}  // namespace colite

#endif // CANOPEN_LITE_OD_RECORD_H
