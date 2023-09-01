//
// Created by lamuguo on 8/29/23.
//

#ifndef CANOPEN_LITE_OD_OBJECT_DICTIONARY_H
#define CANOPEN_LITE_OD_OBJECT_DICTIONARY_H

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

#include "od_data.h"
#include "od_array.h"
#include "od_record.h"
#include "od_variable.h"

namespace colite {

class ObjectDictionary {
    public:
    ObjectDictionary();
    void add_member(int index, int subindex, std::unique_ptr<Variable> var);

    template<typename T>
    T* get(const std::string& index) {
        auto it = name_to_index.find(index);
        if (it != name_to_index.end()) {
            return dynamic_cast<T*>(indices[it->second].get());
        }
        throw std::out_of_range("Index/name " + index + " not found.");
    }

    template<typename T>
    T* get(int index) {
        auto it = indices.find(index);
        if (it != indices.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        throw std::out_of_range("Index " + std::to_string(index) + " not found.");
    }

    // Member variables
    std::unordered_map<int, std::unique_ptr<BaseObj>> indices;
    std::unordered_map<std::string, int> name_to_index;
    std::string comments;
    int bitrate;
    int node_id;
    DeviceInformation device_information;
    std::unordered_map<std::string, std::string> file_info;
};

template<typename T>
void add_object(ObjectDictionary &od, std::unique_ptr<T> obj) {
    obj->parent = nullptr;  // means it's an obj for root.
    int idx = obj->index;
    od.name_to_index[obj->name] = idx;
    BaseObj *base_obj = obj.release();
    od.indices[idx].reset(base_obj);
}

}  // namespace colite

#endif // CANOPEN_LITE_OD_OBJECT_DICTIONARY_H
