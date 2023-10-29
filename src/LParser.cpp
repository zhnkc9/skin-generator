//
// Created by zhnkc9 on 2023/9/26.
//
#include <unordered_set>
#include "util.h"
#include "LParser.hpp"
#include "common_headers.hpp"
#include "LBuilder.hpp"

namespace LParser {

    template<typename M>
    M skinsdefsdata(std::ifstream &ifs) {
        int bs = 4096;
        int cs = 256;
        // 要小写开头的数据
        regex re(R"lit(^TheInventory:AddRestrictedBuildFromLua\(\s*"(\w+)",\s*"(\w+)")lit");
        smatch match;
        M result;
        auto f = [&result, &re, &match](string &line) {
            regex_search(line, match, re);
            if (match[2].matched) {
                result[match[1]].push_back(match[2]);
            }
        };
        util::processFileWithLine(ifs, bs, cs, f);
        LOG(info) << "skindef::R<<len:" << result.size();
        return result;
    }

    /**
     * prefabskins 转换
     */
    json prefabskins(std::ifstream &ifs) {
        int bs = 4096;
        int cs = 256;
        // 要小写开头的数据
        regex re(R"(^\s+([0-9a-z_-]+)\s*=\s*$)");
        regex quotes_re(R"(^\s+\"([0-9a-z_-]+?)\")");
        smatch match;
        smatch value_match;
        bool need_find = false;
        json result;
        json *v = nullptr;
        auto f = [&result, &re, &quotes_re, &match, &need_find, &value_match, &v](string &line) {
            regex_search(line, match, re);
            if (match[1].matched) {
                // 拿到了数据
                need_find = true;
                v = &result[match[1]];
            } else {
                if (need_find) {
                    regex_search(line, value_match, quotes_re);
                    if (value_match[1].matched) {
                        (*v).push_back(value_match[1]);
                    }
                }
            }
        };
        util::processFileWithLine(ifs, bs, cs, f);

        return result;
    }

    /**
     *
     */
    nlohmann::json skinprefabs(std::ifstream &ifs) {
        int c_size = 200;
        int buffer_size = 4096;
        regex re("^table\\.insert\\(prefs, CreatePrefabSkin\\(\"(\\w+?)\"");
        regex kv_re(R"(^\s*(\w+)\s*=\s*(.+),$)");
        regex skins_re(R"(\s*(\w+)\s*=\s*\"?(\w+)\"?)");
        smatch match;
        smatch value_match;
        smatch skins_match;
        bool need_find;
        json result;
        json *v = nullptr;
        auto f =
                [&match, &re, &need_find, &result, &v, &value_match, &kv_re, &skins_re, &skins_match](string &line) -> void {
                    regex_search(line, match, re);
                    if (match[1].matched) {
                        // 拿到了数据
                        need_find = true;
                        v = &result[match[1]];
                    } else {
                        if (need_find) {
                            regex_search(line, value_match, kv_re);
                            if (value_match[2].matched) {
                                auto &v_k = value_match[1];
                                auto &v_v = value_match[2];
                                if (v_k == "skins") {
                                    (*v)["origin_skins"] = json(v_v);
                                    // {ghost_skin = "ghost_walter", normal_skin = "walter_bee"}
                                    json skins;
                                    auto first = v_v.begin();
                                    while (regex_search(first, v_v.end(), skins_match, skins_re)) {
                                        skins[skins_match[1]] = skins_match[2];
                                        first = skins_match.suffix().begin();
                                    }
                                    (*v)[v_k] = std::move(skins);
                                } else {
                                    (*v)[v_k] = v_v;
                                }
                            }
                        }
                    }
                };
        util::processFileWithLine(ifs, buffer_size, c_size, f);

        return result;
    }

    json chinesepo(std::ifstream &ifs) {
        int c_size = 200;
        int buffer_size = 4096;
        boost::regex re(
                R"(msgctxt\s*\"(STRINGS\.CHARACTER_NAMES\.|STRINGS\.SKIN_NAMES\.|STRINGS\.NAMES\.)([A-Za-z1-9_]*?)\"[\S\s]*?msgstr\s*?\"(.*?)\")");
        json result;
        auto f =
                [&result](boost::sregex_iterator &it, boost::sregex_iterator &end, boost::smatch &match) -> bool {
                    bool matched = false;
                    while (it != end) {
                        match = *it;
                        ++it;
                        result[match[2]] = match[3];
                        matched = true;
                    }
                    return matched;
                };
        util::processFileWithRegexIt(ifs, buffer_size, c_size, re, f);

        return result;
    }

    template std::map<std::string, std::vector<std::string>> skinsdefsdata(std::ifstream &ifs);

    template std::unordered_map<std::string, std::vector<std::string>> skinsdefsdata(std::ifstream &ifs);

    nlohmann::json distinctSkinsdefsdata(std::ifstream &ifs) {
        auto result = skinsdefsdata(ifs);
        // result 转 json
        json dup_result(result);

//        regex re_dup1("_?([pd]|alt)$|_alt(?=_[a-z])|_p(?=_[a-z])|_d(?=_[a-z])|_alt(?=_item)");
        regex re_dup1("_?([pd]|alt)$|_alt(?=_[a-z])|_p(?=_[a-z])|_d(?=_[a-z])|_alt(?=_item)");
        std::vector<string> dict{"p", "_p", "d", "_d", "alt", "_alt", "_alt_"};
        smatch match;
        auto *tmp = new string;
        auto compare = [](const std::string &a, const std::string &b) {
            return a.length() < b.length();
        };
        for (auto &p: result) {

            auto &first = p.first;
            LOG(debug) << "skindef::distinct::process:" << first;
            *tmp = regex_replace(first, re_dup1, "", regex_constants::format_first_only);
            if (*tmp != first and dup_result.contains(*tmp)) {
                LOG(info) << "skindef::distinct::matched:" << first;
                dup_result.erase(first);
                continue;
            }
            vector<std::string> &arr = p.second;
            auto &dup_arr = dup_result[first];

            LOG(debug) << "skindef::distinct::value::process:" << dup_arr;

            auto size = arr.size();
            if (size == 1)
                continue;
            std::sort(arr.begin(), arr.end(), compare);
            // 第一个元素默认不处理了
//            for (int i = 0; i < size - 1; ++i) {
//                // 是不是比别人多了一个p 或者 d
//                LOG(debug) << "skindef::distinct::arr::process:" << arr[i] << " " << arr[i + 1];
//                bool b = util::strDistanceChecker(arr[i], arr[i + 1], dict, 0, 1);
//                if (b) {
//                    LOG(debug) << "skindef::distinct::checker:" << arr[i + 1];
//                    dup_arr.erase(std::remove(dup_arr.begin(), dup_arr.end(), arr[i + 1]), dup_arr.end());
//                    ++i;
//                }
//            }

            for (int i = 0; i < size - 1; ++i) {
                // 是不是比别人多了一个p 或者 d
                for (int z = i + 1; z < size; z++) {
                    LOG(debug) << "skindef::distinct::arr::process:" << arr[i] << " " << arr[z];
                    bool b = util::strDistanceChecker(arr[i], arr[z], dict, 0, 1);
                    if (b) {
                        LOG(info) << "skindef::distinct::matched:" << arr[z];
                        dup_arr.erase(std::remove(dup_arr.begin(), dup_arr.end(), arr[z]), dup_arr.end());
                        break;
                    }
                }
            }


        }
        LOG(info) << "skindef::distinct::R<<len:" << dup_result.size();
        delete tmp;
        return dup_result;
    }

    /*
     * 需要根据 skindefsdata 数据去掉杂项
     * */
    void processSkinprefabs(const string &prefix, json &skinprefabs) {

        std::unordered_set<string> skip_key = {"share_bigportrait_name", "skin_tags", "base_prefab", "build_name_override", "feet_cuff_size",
                                               "assets", "fx_prefab",
                                               "prefabs", "origin_skins", "bigportrait_anim", "skins"};

        std::unordered_set<string> clothing_type = {"body", "hand", "legs", "feet"};

        regex item_type("^(.*?)_?(builder|item)$");

        // 用于匹配人物的正则
        regex characte("^.*(wathgrithr|wes|wanda|wortox|wendy|wormwood|woodie|willow|wolfgang|" \
                    "waxwell|wilson|webber|winona|wonkey|wurt|wickerbottom|wx78|warly|walter).*$");

        smatch match;

        for (auto skin_it = skinprefabs.begin(); skin_it != skinprefabs.end(); ++skin_it) {

            auto &kvs = skin_it.value();
            auto &skinid = skin_it.key();
            auto type = util::removeSurroundingChars(kvs["type"], "\"");
            string pattern = kvs.contains("base_prefab") ? \
                    util::removeSurroundingChars(kvs["base_prefab"], "\"")
                    + "|" + skinid : skinid;

            // 如果 skinid 后缀是item _item build _build
            if (regex_search(skinid, match, item_type))
                pattern += "|" + match[1];

            LOG(info) << "processSkinprefabs:pattern:" << pattern << "; [1] " << match[1];

            regex dup("(?<=\")([^\"/]*?(" + pattern + ")[^\"]*?)(?=\")");

            string origin_build_name;

            if (!kvs.contains("build_name_override")) {
                origin_build_name = skinid;
                kvs["build_name_override"] = prefix + origin_build_name;
            } else {
                origin_build_name = util::removeSurroundingChars(kvs["build_name_override"], "\"");
                kvs["build_name_override"] = prefix + origin_build_name;
            }
            kvs["origin_build_name"] = origin_build_name;

            // 渲染assets
            if (type == "item") {
                kvs["assets"] = AssertBuilder::build(ResourceType::ITEM, prefix, origin_build_name);
            } else if (type == "base") {
                json &skins = kvs["skins"];
                kvs["share_bigportrait_name"] = "\"" + skinid + "\"";
                AssertBuilder builder(prefix);
                for (auto it = skins.begin(); it != skins.end(); ++it) {
                    auto ghost_id = it.value().get<string>();
                    auto base_prefab = util::removeSurroundingChars(kvs["base_prefab"], "\"");
                    if (ghost_id == "ghost_" + base_prefab or ghost_id == "ghost_" + base_prefab + "_build") {
                        continue;
                    }
                    // prefix
                    it.value() = prefix + ghost_id;
//                    if (it.key() == "normal_skin")
//                        builder.with(ResourceType::BIGPORTRAITS, ghost_id);
                    builder.with(ResourceType::ITEM, ghost_id);
                }
                kvs["assets"] = builder.build();
            } else if (clothing_type.count(type) > 0) {
                kvs["assets"] = AssertBuilder::build(ResourceType::ITEM, prefix, origin_build_name);
            }
//            kvs["rarity"] = "\"ModMade\"";
            kvs["release_group"] = "groupid";

            // 只处理这两个
            if (type == "item" or type == "base") {
                // init function 干掉
//                kvs.erase("feet_cuff_size");
//                kvs.erase("init_fn");
//                kvs.erase("bigportrait_anim");
//                kvs.erase("rarity_modifier");

                // 迭代属性看看哪个小可爱有问题
                for (auto it = kvs.begin(); it != kvs.end(); ++it) {
                    auto &key = it.key();
                    if (skip_key.count(key) > 0) continue;
                    json v = it.value();
                    if (v.is_string())
                        it.value() = regex_replace(v.get<string>(), dup, prefix + "$1");
                }
            } else if (clothing_type.count(type) > 0) {
                if (regex_search(skinid, match, characte)) {
                    kvs["affinity"] = R"(")" + match[1] + R"(")";
                }
            }

        }

    }

    /*
     * xml 解析
     * */
    json xml(std::ifstream &ifs) {
        int c_size = 200;
        int buffer_size = 4096;
        boost::regex re("(filename|name)=\"(.*?)\"");
        json result;
        auto f =
                [&result](boost::sregex_iterator &it, boost::sregex_iterator &end, boost::smatch &match) -> bool {
                    bool matched = false;
                    while (it != end) {
                        match = *it;
                        ++it;
                        result[match[1]].push_back(match[2]);
                        matched = true;
                    }
                    return matched;
                };
        util::processFileWithRegexIt(ifs, buffer_size, c_size, re, f);
        return result;
    }

    nlohmann::json clothing(std::ifstream &ifs) {
        int c_size = 200;
        int buffer_size = 4096;
        regex re("^\\s+(\\w+) =$");
        regex kv_re(R"(^\s+(\w+)\s*=\s*(.+),)");
        smatch match;
        smatch value_match;
        bool need_find = false;
        json result;
        json *v = nullptr;
        auto f =
                [&match, &re, &need_find, &result, &v, &value_match, &kv_re](string &line) -> void {
                    regex_search(line, match, re);
                    if (match[1].matched) {
                        // 拿到了数据
                        need_find = true;
                        v = &result[match[1]];
                    } else {
                        if (need_find) {
                            regex_search(line, value_match, kv_re);
                            if (value_match[2].matched) {
                                auto &v_k = value_match[1];
                                auto &v_v = value_match[2];
                                (*v)[v_k] = v_v;
                            }
                        }
                    }
                };
        util::processFileWithLine(ifs, buffer_size, c_size, f);
        return result;

    }

    void test(const string &body) {
        cout << body << endl;
    }

}


