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
    virtual void add_member(std::unique_ptr<Variable> var);
    Variable* get_variable(const std::string& name);
    size_t size() const;

    // Member variables
    // TODO(lamuguo): Move parent to BaseObj, and consider whether we need to
    // use BaseObj*
    void* parent;
    int index;
    std::string name;
    std::string storage_location;
    std::unordered_map<int, std::unique_ptr<Variable>> subindices;
    std::unordered_map<std::string, int> name_to_subindex;
};

}  // namespace colite

#endif // CANOPEN_LITE_OD_RECORD_H
