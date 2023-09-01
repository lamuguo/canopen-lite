#include "od_object_dictionary.h"
#include <stdexcept>

namespace colite {

ObjectDictionary::ObjectDictionary()
    : bitrate(-1), node_id(-1) {}  // initialize with defaults

void ObjectDictionary::add_member(int index, int subindex, std::unique_ptr<Variable> var) {
    BaseObj *obj = this->indices[index].get();
    var->index = index;
    var->subindex = subindex;
    obj->add_member(std::move(var));
}

}  // namespace colite
