//
// Created by lamuguo on 8/29/23.
//

#ifndef CANOPEN_LITE_EDS_H
#define CANOPEN_LITE_EDS_H

#include <string>
#include <exception>
#include <map>
#include "od_object_dictionary.h"

namespace colite {

class EDSFileError : public std::exception {
public:
    EDSFileError(const std::string& msg) : msg_(msg) {}
    const char* what() const noexcept override {
        return msg_.c_str();
    }
private:
    std::string msg_;
};

ObjectDictionary import_eds(const std::string& eds_path, int node_id);
Data to_int_with_node_id(int node_id, DataType var_type, const std::string& raw);
Data to_data(DataType type, const char* hexStr);

}  // namespace colite

#endif //CANOPEN_LITE_EDS_H
