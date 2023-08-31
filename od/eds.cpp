//
// Created by lamuguo on 8/29/23.
//
#include <iomanip>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>

#include "fmt/core.h"
#include "SimpleIni.h"

#include "eds.h"
#include "od_variable.h"  // Assuming this is where SIGNED_TYPES and other datatype related utilities are defined

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace colite {

std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); }
    );
    return s;
}

int64_t evaluate_simple_expression(const std::string& expr) {
    std::istringstream stream(expr);
    int64_t total = 0;
    int64_t current_value;
    char op;

    while (stream >> current_value) {
        total += current_value;
        stream >> op; // Read the '+' character, if there's one.
    }

    return total;
}

Data to_int_with_node_id(int node_id, DataType type, const std::string &expression) {
    // TODO(lamuguo): to const
    std::regex regex("\\$NODEID");
    std::string modified_expression = std::regex_replace(expression, regex, std::to_string(node_id));

    int64_t value = evaluate_simple_expression(modified_expression);
    return to_int_data(type, value);
}

Data to_data(DataType type, const char* hexStr) {
    uint64_t value = std::strtoull(hexStr, nullptr, 16);
    return to_int_data(type, static_cast<int64_t>(value));
}

Data convert_variable(int node_id, DataType var_type, const std::string& raw) {
    switch (var_type) {
        case OCTET_STRING:
        case DOMAIN: {
            int v = std::stoi(raw, 0, 16);
            return Data(var_type, static_cast<int32_t>(v));
        }
        case VISIBLE_STRING:
        case UNICODE_STRING: {
            return Data(var_type, raw);
        }
        case REAL32:{
            float v = std::stof(raw);
            return Data(var_type, v);
        }
        case REAL64: {
            double v = std::stod(raw);
            return Data(var_type, v);
        }
        default: {
            return to_int_with_node_id(node_id, var_type, raw);
        }
    }
}

std::unique_ptr<Variable> build_variable(const CSimpleIniA& eds, const char* section, int node_id, int index, int subindex = 0) {
    const char* defaultValue;

    auto var = std::make_unique<Variable>(eds.GetValue(section, "ParameterName", ""), index, subindex);

    var->storage_location = std::string(eds.GetValue(section, "StorageLocation", ""));
    var->data_type = toDataType(eds.GetLongValue(section, "DataType", UNKNOWN));
    defaultValue = eds.GetValue(section, "DataType", nullptr);
    var->access_type = str_tolower(eds.GetValue(section, "AccessType", "rw"));

    // Handle pdo_mappable
    defaultValue = eds.GetValue(section, "PDOMapping", "0");
    var->pdo_mappable = std::stoi(defaultValue, nullptr, 16) != 0;

    // Handle LowLimit
    var->min = to_data(var->data_type, eds.GetValue(section, "LowLimit", "0"));
    var->max = to_data(var->data_type, eds.GetValue(section, "HighLimit", "0"));

    // Handle DefaultValue
    var->default_raw = eds.GetValue(section, "DefaultValue", "");
    if (!var->default_raw.empty()) {
        if (var->default_raw.find("$NODEID") != std::string::npos) {
            var->relative = true;
        }
        var->default_value = convert_variable(node_id, var->data_type, var->default_raw);
    }

    // Handle ParameterValue
    var->parameter_raw = eds.GetValue(section, "ParameterValue", "");
    if (!var->parameter_raw.empty()) {
        var->parameter_value = convert_variable(node_id, var->data_type, var->parameter_raw);
    }

    return var;
}

void fill_device_info(const CSimpleIniA& ini, DeviceInformation& di) {
    auto pSection = ini.GetSection("DeviceInfo");
    if (pSection == nullptr) {
        std::cout << "eds file does not have a DeviceInfo section. This section is mandatory" << std::endl;
        return;
    }

    for (const auto& item : *pSection) {
        if (strncmp(item.first.pItem, "BaudRate_", 9) == 0) {
            int rate = std::stoi(item.first.pItem + 9);
            di.allowed_baudrates.insert(rate * 1000);
        } else if (strcmp(item.first.pItem, "VendorName") == 0) {
            di.vendor_name = item.second;
        } else if (strcmp(item.first.pItem, "VendorNumber") == 0) {
            di.vendor_number = std::stoi(item.second);
        } else if (strcmp(item.first.pItem, "ProductName") == 0) {
            di.product_name = item.second;
        } else if (strcmp(item.first.pItem, "ProductNumber") == 0) {
            di.product_number = std::stoi(item.second);
        } else if (strcmp(item.first.pItem, "RevisionNumber") == 0) {
            di.revision_number = std::stoi(item.second);
        } else if (strcmp(item.first.pItem, "OrderCode") == 0) {
            di.order_code = item.second;
        } else if (strcmp(item.first.pItem, "SimpleBootUpMaster") == 0) {
            di.simple_boot_up_master = (item.second == "1");
        } else if (strcmp(item.first.pItem, "SimpleBootUpSlave") == 0) {
            di.simple_boot_up_slave = (item.second == "1");
        } else if (strcmp(item.first.pItem, "Granularity") == 0) {
            di.granularity = std::stoi(item.second); // Assuming granularity is an integer type in your struct
        } else if (strcmp(item.first.pItem, "DynamicChannelsSupported") == 0) {
            di.dynamic_channels_supported = (item.second == "1");
        } else if (strcmp(item.first.pItem, "GroupMessaging") == 0) {
            di.group_messaging = (item.second == "1");
        } else if (strcmp(item.first.pItem, "NrOfRXPDO") == 0) {
            di.nr_of_RXPDO = std::stoi(item.second);
        } else if (strcmp(item.first.pItem, "NrOfTXPDO") == 0) {
            di.nr_of_TXPDO = std::stoi(item.second);
        } else if (strcmp(item.first.pItem, "LSS_Supported") == 0) {
            di.LSS_supported = (item.second == "1");
        }
    }
}

ObjectDictionary import_eds(const std::string& eds_path, int node_id) {
    ObjectDictionary od;

    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile(eds_path.c_str());
    if (rc < 0) {
        throw std::runtime_error(fmt::format("Error in loading eds file '{}'", eds_path));
    }

    {
        // FileInfo section
        const auto* pSection = ini.GetSection("FileInfo");
        if (pSection) {
            for (const auto& item : *pSection) {
                od.file_info[item.first.pItem] = item.second;
            }
        }
    }

    {
        // Comments section
        const auto* pSection = ini.GetSection("Comments");
        if (pSection != nullptr) {
            auto it = pSection->find("Lines");
            int linecount = (it == pSection->end() ? 0 : std::stoi(it->second));

            std::string comments = "";
            for (int line = 1; line <= linecount; ++line) {
                std::string key = "Line" + std::to_string(line);
                auto lineIt = pSection->find(key.c_str());
                if (lineIt != pSection->end()) {
                    comments += lineIt->second;
                    if (line != linecount) {
                        comments += '\n';
                    }
                }
            }

            od.comments = comments;
        }
    }

    {
        // DeviceComissioning section
        const auto* pSection = ini.GetSection("DeviceComissioning");
        if (pSection != nullptr) {
            auto bitrateIt = pSection->find("BaudRate");
            if (bitrateIt != pSection->end()) {
                od.bitrate = std::stoi(bitrateIt->second) * 1000;
            }

            auto nodeIdIt = pSection->find("NodeID");
            if (nodeIdIt != pSection->end()) {
                // 注意: std::stoi 的第二个参数允许指定基数。在这里我们假设 NodeID 可能是一个十六进制数，所以我们提供了基数 0，这允许自动检测。
                od.node_id = std::stoi(nodeIdIt->second, nullptr, 0);
            }
        }
    }

    fill_device_info(ini, od.device_information);

    CSimpleIniA::TNamesDepend sections;
    ini.GetAllSections(sections);

    // TODO(lamuguo): Move to const.
    std::regex pattern(R"(^[Dd]ummy[Uu]sage$)");
    std::regex subIndexPattern(R"(^([0-9A-Fa-f]{4})[S|s]ub([0-9A-Fa-f]+)$)");
    std::regex indexNamePattern(R"(^([0-9A-Fa-f]{4})Name)");
    std::regex indexPattern(R"(^[0-9A-Fa-f]{4}$)");
    for (const auto& section : sections) {
        std::smatch indexMatch;
        std::string sectionStr(section.pItem);
        if (std::regex_match(sectionStr, indexMatch, indexPattern)) {
            int index = std::stoi(indexMatch[0].str(), nullptr, 16);
            const char* nameChar = ini.GetValue(section.pItem, "ParameterName");
            std::string name(nameChar ? nameChar : "");

            ObjectType object_type = toObjectType(std::stoi(
                    ini.GetValue(section.pItem, "ObjectType"), nullptr, 0));

            std::string storage_location(ini.GetValue(
                    section.pItem, "StorageLocation", ""));

            switch (object_type) {
                case VARIABLE_OBJ:
                case DOMAIN_OBJ: {
                    std::unique_ptr<Variable> var = build_variable(ini, section.pItem, node_id, index);
                    add_object(od, std::move(var));
                    break;
                }
                case ARRARY_OBJ: {
                    auto arr = std::make_unique<Array>(name, index);
                    if (ini.GetValue(section.pItem, "CompactSubObj")) {
                        auto last_subindex = std::make_unique<Variable>("Number of entries", index, 0);
                        last_subindex->data_type = UNSIGNED8;
                        arr->add_member(std::move(last_subindex));
                        arr->add_member(build_variable(ini, section.pItem, node_id, index, 1));
                    }
                    arr->storage_location = storage_location;
                    add_object(od, std::move(arr));
                    break;
                }
                case RECORD_OBJ: {
                    std::unique_ptr<Record> record = std::make_unique<Record>(name, index);
                    record->storage_location = storage_location;
                    add_object(od, std::move(record));
                    break;
                }
                default: {
                    // error
                    throw "incorrect object type";
                }
            }
            continue;
        }

        // "^[Dd]ummy[Uu]sage$"
        if (std::regex_match(section.pItem, pattern)) {
            for (int i = 1; i <= 7; ++i) {
                std::ostringstream oss;
                oss << "Dummy" << std::setw(4) << std::setfill('0') << i;
                std::string key = oss.str();
                const char* value = ini.GetValue(section.pItem, key.c_str());
                if (value != nullptr && std::stoi(value) == 1) {
                    auto var = std::make_unique<Variable>(key, i, 0);
                    var->data_type = toDataType(i);
                    var->access_type = "const";
                    add_object(od, std::move(var));
                }
            }
            continue;
        }


        // section "[{index}sub{subindex}]"
        std::smatch subIndexMatch;
        if (std::regex_match(sectionStr, subIndexMatch, subIndexPattern)) {
            int index = std::stoi(subIndexMatch[1].str(), nullptr, 16);
            int subindex = std::stoi(subIndexMatch[2].str(), nullptr, 16);
            std::unique_ptr<Variable> var = build_variable(ini, section.pItem, node_id, index, subindex);
            od.add_member(index, subindex, std::move(var));
            continue;
        }

        // section "[{index}Name]"
        std::smatch indexNameMatch;
        if (std::regex_match(sectionStr, indexNameMatch, indexNamePattern)) {
            int index = std::stoi(indexNameMatch[1].str(), nullptr, 16);
            const char* numOfEntriesChar = ini.GetValue(section.pItem, "NrOfEntries");
            int numOfEntries = std::stoi(numOfEntriesChar);
            Array *arr = static_cast<Array *>(od.indices[index].get());
            auto src_var = std::make_unique<Variable>(*arr->get_variable(1));
            for (int subindex = 1; subindex <= numOfEntries; ++subindex) {
                auto var = std::make_unique<Variable>(*src_var);
                std::string name(ini.GetValue(section.pItem, std::to_string(subindex).c_str(), ""));
                var->name = name;
                var->subindex = subindex;
                arr->add_member(std::move(var));
            }
            continue;
        }
    }

    return od;
}

}  // namespace colite
