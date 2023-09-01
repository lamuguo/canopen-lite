#include "od_record.h"
#include <stdexcept>

namespace colite {

// virtual
Variable* Record::get_variable(int subindex) {
    return this->subindices[subindex].get();
}

// virtual
void Record::add_member(std::unique_ptr<Variable> var) {
    var->parent = this;
    name_to_subindex[var->name] = var->subindex;
    subindices[var->subindex] = std::move(var);
}

Variable* Record::get_variable(const std::string& name) {
    auto it = name_to_subindex.find(name);
    if (it == name_to_subindex.end()) {
        return nullptr;
    }
    return get_variable(it->second);
}

size_t Record::size() const {
    return subindices.size();
}

}  // namespace colite
