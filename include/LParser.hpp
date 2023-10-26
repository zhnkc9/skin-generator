//
// Created by zhnkc9 on 2023/9/27.
//

#ifndef SKINS_GENERATOR_LPARSER_HPP
#define SKINS_GENERATOR_LPARSER_HPP

#include "json.hpp"

using nlohmann::json;
using std::ifstream;
using std::string;

#include <fstream>

namespace fs = std::filesystem;

#ifndef STRING_PARAM_FUNC
#define STRING_PARAM_FUNC(name) \
    inline json name(const fs::path &p) { \
        std::ifstream ifs(p); \
        if (!ifs.is_open()) { \
            throw std::runtime_error("Failed to open file: " + p.string()); \
        } \
        return name(ifs); \
    }
#endif

namespace LParser {

    template<typename M = std::map<std::string, std::vector<std::string>>>
    M skinsdefsdata(std::ifstream &ifs);

    json prefabskins(std::ifstream &ifs);

    json skinprefabs(std::ifstream &ifs);

    json clothing(std::ifstream &ifs);

    json chinesepo(std::ifstream &ifs);

    json distinctSkinsdefsdata(std::ifstream &ifs);

    void processSkinprefabs(const std::string &prefix, nlohmann::json &skinprefabs);

    json xml(std::ifstream &ifs);

    STRING_PARAM_FUNC(prefabskins)

    STRING_PARAM_FUNC(skinprefabs)

    STRING_PARAM_FUNC(clothing)

    STRING_PARAM_FUNC(chinesepo)

    STRING_PARAM_FUNC(distinctSkinsdefsdata)

    STRING_PARAM_FUNC(xml)

    [[maybe_unused]] __declspec(dllexport) void test(const string &body);

}

#undef STRING_PARAM_FUNC

#endif
