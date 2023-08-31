#include <string>
#include <unordered_map>
#include <stdexcept>
#include <vector>
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
        add_member(subindex, std::move(var));
        return subindices[subindex].get();
    }
    throw std::runtime_error("Could not find subindex " + std::to_string(subindex));
}

// virtual
void Array::add_member(int subindex, std::unique_ptr<Variable> var) {
    this->subindices[subindex] = std::move(var);
}

Variable* Array::operator[](const std::string& subindex) {
    auto it = name_to_subindex.find(subindex);
    if (it == name_to_subindex.end()) {
        return nullptr;
    }
    return subindices[it->second].get();
}

std::vector<int> Array::keys() const {
    std::vector<int> keys;
    for (const auto& pair : subindices) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool Array::operator==(const Array& other) const {
    return index == other.index;
}

void Array::add_member(std::unique_ptr<Variable> variable) {
    variable->parent = this;
    name_to_subindex[variable->name] = variable->subindex;
    subindices[variable->subindex] = std::move(variable);
}

}  // namespace colite
