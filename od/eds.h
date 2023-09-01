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

ObjectDictionary import_eds(const std::string& eds_path, int node_id);
Data to_int_with_node_id(int node_id, DataType var_type, const std::string& raw);
Data to_data(DataType type, const char* hexStr);

}  // namespace colite

#endif //CANOPEN_LITE_EDS_H
