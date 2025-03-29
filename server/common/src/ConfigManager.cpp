#include "../include/ConfigManager.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>


std::string SectionInfo::operator[](const std::string& key) const {
    auto it = _key_value_map.find(key);
    if (it != _key_value_map.end()) {
        return it->second;
    }
    return "";
}

ConfigManager::ConfigManager() {
    // 读取配置文件
    boost::property_tree::ptree pt;
    boost::filesystem::path current_path = boost::filesystem::current_path();
    boost::filesystem::path config_file_path = current_path / _config_file_relative_path;
    try {
        boost::property_tree::ini_parser::read_ini(config_file_path.string(), pt);
    } catch (const std::exception& e) {
        std::cerr << "Error reading config file: " << e.what() << std::endl;
        return;
    }
    // 遍历配置文件中的所有节
    for (const auto& section : pt) {
        const std::string& section_name = section.first;
        SectionInfo section_info;
        // 遍历节中的所有键值对
        for (const auto& key_value : section.second) {
            const std::string& key = key_value.first;
            const std::string& value = key_value.second.data();
            section_info._key_value_map[key] = value;  // 将键值对存入SectionInfo对象
        }
        _section_map[section_name] = section_info;  // 将SectionInfo对象存入_config_map
    }
}

const SectionInfo& ConfigManager::operator[](const std::string& section) const {
    auto it = _section_map.find(section);
    if (it != _section_map.end()) {
        return it->second;
    }
    static SectionInfo emptySection;  // 返回一个空的SectionInfo对象
    return emptySection;
}