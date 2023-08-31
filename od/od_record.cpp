#include "od_record.h"
#include <stdexcept>

namespace colite {

// virtual
Variable* Record::get_variable(int subindex) {
    return this->subindices[subindex].get();
}

// virtual
void Record::add_member(int subindex, std::unique_ptr<Variable> var) {
    name_to_subindex[var->name] = subindex;
    subindices[subindex] = std::move(var);
}

Variable* Record::get_variable(const std::string& name) {
    auto it = name_to_subindex.find(name);
    if (it == name_to_subindex.end()) {
        return nullptr;
    }
    return get_variable(it->second);
}

Variable* Record::operator[](const std::string& subindex) {
    auto it = name_to_subindex.find(subindex);
    if (it == name_to_subindex.end()) {
        return nullptr;
    }
    return subindices[it->second].get();
}

Variable* Record::operator[](int subindex) {
    auto it = subindices.find(subindex);
    if (it != subindices.end()) {
        return it->second.get();
    }
    throw std::runtime_error("Subindex " + std::to_string(subindex) + " not found.");
}

void Record::insert(int subindex, std::unique_ptr<Variable> var) {
    var->parent = this;
    subindices[subindex] = std::move(var);
    name_to_subindex[var->name] = subindex;
}

void Record::erase(const std::string& subindex) {
    auto it = name_to_subindex.find(subindex);
    if (it == name_to_subindex.end()) {
        return;
    }
    subindices.erase(it->second);
    name_to_subindex.erase(it);
}

size_t Record::size() const {
    return subindices.size();
}

std::vector<int> Record::keys() const {
    std::vector<int> keys;
    for (const auto& pair : subindices) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool Record::contains(const std::string& subindex) const {
    return name_to_subindex.find(subindex) != name_to_subindex.end();
}

bool Record::operator==(const Record& other) const {
    return index == other.index;
}

void Record::add_member(std::unique_ptr<Variable> variable) {
    variable->parent = this;
    name_to_subindex[variable->name] = variable->subindex;
    subindices[variable->subindex] = std::move(variable);
}

}  // namespace colite
