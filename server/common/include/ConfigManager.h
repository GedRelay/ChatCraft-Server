#pragma once
#include "const.h"

class SectionInfo;
// 每SectionInfo对象表示一个配置节，包含多个键值对
class SectionInfo {
    friend class ConfigManager;
public:
    std::string operator[](const std::string& key) const;  // 重载[]运算符，获取键对应的值

    
    template<typename T>
    T GetValueAs(const std::string& key) const;  // 获取指定键的值，并转换为指定类型。如果没有找到对应值，返回默认值

private:
    template<typename T>
    static T StringTo(const std::string& str);  // 将字符串转换为指定类型

    std::unordered_map<std::string, std::string> _key_value_map;  // 键值对
};


// ConfigManager类用于管理配置项，继承自单例类Singleton
class ConfigManager : public Singleton<ConfigManager> {
    friend class Singleton<ConfigManager>;
public:
    const SectionInfo& operator[](const std::string& section) const;  // 重载[]运算符，获取指定节的SectionInfo对象

    template<typename T=std::string>
    static T GetConfigAs(const std::string& section, const std::string& key);  // 获取指定节和键的值，并转换为指定类型

private:
    ConfigManager();

    std::unordered_map<std::string, SectionInfo> _section_map;  // 配置项
    std::string _config_file_relative_path{ "../common/config.ini" };  // 配置文件路径
};




template<typename T>
T SectionInfo::GetValueAs(const std::string& key) const {
    auto it = _key_value_map.find(key);
    if (it != _key_value_map.end()) {
        return StringTo<T>(it->second);  // 找到对应值，转换为指定类型
    }
    return T();  // 没有找到对应值，返回默认值
}


template<typename T>
T SectionInfo::StringTo(const std::string& str) {
    if constexpr(std::is_integral_v<T>) {
        return static_cast<T>(std::stoul(str));
    } 
    else if constexpr (std::is_floating_point_v<T>) {
        return static_cast<T>(std::stod(str));
    }
    else if constexpr(std::is_same_v<T, bool>) {
        return str == "true" || str == "1";
    } 
    else if constexpr(std::is_same_v<T, std::string>) {
        return str;
    }
    else {
        static_assert(!std::is_same_v<T, T>, "Unsupported type");
    }
}


template<typename T>
T ConfigManager::GetConfigAs(const std::string& section, const std::string& key) {
    auto it = GetInstance()->_section_map.find(section);
    if (it != GetInstance()->_section_map.end()) {
        return it->second.GetValueAs<T>(key);  // 找到对应值，转换为指定类型
    }
    return T();  // 没有找到对应值，返回默认值
}