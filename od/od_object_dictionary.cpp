#include "od_object_dictionary.h"
#include <stdexcept>

namespace colite {

ObjectDictionary::ObjectDictionary()
    : bitrate(-1), node_id(-1) {}  // initialize with defaults

void ObjectDictionary::add_member(int index, int subindex, std::unique_ptr<Variable> var) {
    BaseObj *obj = this->indices[index].get();
    obj->add_member(subindex, std::move(var));
}

size_t ObjectDictionary::size() const {
    return indices.size();
}

std::vector<int> ObjectDictionary::keys() const {
    std::vector<int> keys;
    for (const auto& pair : indices) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool ObjectDictionary::contains(const std::string& index_or_name) const {
    return name_to_index.find(index_or_name) != name_to_index.end();
}

Variable* ObjectDictionary::get_variable(const std::string& name, int subindex) {
    auto it = name_to_index.find(name);
    if (it == name_to_index.end()) {
        return nullptr;
    }
    return indices[it->second]->get_variable(subindex);
}

}  // namespace colite
