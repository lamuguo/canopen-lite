#include <string>
#include <unordered_map>
#include <stdexcept>
#include <memory>

#include "od_array.h"

namespace colite {

// virtual
Variable* Array::get_variable(int subindex) {
    auto it = subindices.find(subindex);
    if (it != subindices.end()) {
        return it->second.get();
    }
    if (subindex > 0 && subindex < 256) {
        auto var = std::make_unique<Variable>(*subindices[1]);
        var->name = name + "_" + std::to_string(subindex);
        var->subindex = subindex;
        add_member(std::move(var));
        return subindices[subindex].get();
    }
    throw std::runtime_error("Could not find subindex " + std::to_string(subindex));
}

// virtual
void Array::add_member(std::unique_ptr<Variable> var) {
        var->parent = this;
    this->name_to_subindex[var->name] = var->subindex;
    this->subindices[var->subindex] = std::move(var);
}

}  // namespace colite
