//
// Created by zhnkc9 on 2023/9/30.
//
#include <gtest/gtest.h>
#include "common_headers.hpp"
#include "LParser.hpp"
#include "util.h"
#include "inja.hpp"
#include "LBuilder.hpp"

TEST(TEST_LUACODE_PARSER, skinprefabs_by_line) {
    static std::ifstream ifs("D:\\饥荒\\scripts\\prefabs\\skinprefabs.lua");
    LParser::skinprefabs(ifs);
}

TEST(TEST_LUACODE_PARSER, prefabskins_by_line) {
    std::ifstream ifs("D:\\饥荒\\scripts\\prefabskins.lua");
    LParser::prefabskins(ifs);
}

TEST(TEST_LUACODE_PARSER, skinsdefsdata_test_line) {
    std::ifstream ifs("D:\\饥荒\\scripts\\skins_defs_data.lua");
    auto result = LParser::skinsdefsdata<std::unordered_map<std::string, std::vector<std::string>>>(ifs);
    std::unordered_map<string, vector<string>> dup_result(result.size());
    std::copy(result.begin(), result.end(), std::inserter(dup_result, dup_result.begin()));
    auto end = dup_result.end();
    regex re_dup1("(_?[pd]$|_p(?=_[a-z])|_d(?=_[a-z]))");
    std::vector<string> dict{"p", "_p", "d", "_d"};
    smatch match;
    auto *tmp = new string;
    // 定义一个 lambda 表达式，用于按照字符串长度排序
    auto compare = [](const std::string &a, const std::string &b) {
        return a.length() < b.length();
    };
    for (auto &p: result) {
        auto &first = p.first;
        *tmp = regex_replace(first, re_dup1, "", regex_constants::format_first_only);
        if (*tmp != first and dup_result.find(*tmp) != end) {
            cout << "ignore1:" << first << endl;
            dup_result.erase(first);
            continue;
        }
//        cout << "process:" << first << endl;
        vector<std::string> &arr = p.second;
        auto &dup_arr = dup_result[first];
        auto size = arr.size();
        if (size == 1)
            continue;
        std::sort(arr.begin(), arr.end(), compare);
        // 第一个元素默认不处理了
        for (int i = 0; i < size - 1; ++i) {
            // 是不是比别人多了一个p 或者 d
            bool b = util::strDistanceChecker(arr[i], arr[i + 1], dict, 0, 1);
            if (b) {
                cout << "checker:" << arr[i + 1] << endl;
                dup_arr.erase(std::remove(dup_arr.begin(), dup_arr.end(), arr[i + 1]), dup_arr.end());
                ++i;
            }
        }
    }
    cout << dup_result.size() << endl;
}

TEST(TEST_LUACODE_PARSER, chinese_po_test_find_all) {
    std::ifstream ifs("D:\\饥荒\\scripts\\languages\\chinese_s.po");
    LParser::chinesepo(ifs);
}

TEST(TEST_LUACODE_PARSER, test_distinct_skinsdefsdata) {
    std::ifstream ifs("D:\\饥荒\\scripts\\skins_defs_data.lua");
    auto result = LParser::distinctSkinsdefsdata(ifs);
}

TEST(TEST_LUACODE_PARSER, test_render_skinprefabs) {
    std::ifstream ifs("D:\\饥荒\\scripts\\prefabs\\skinprefabs.lua");
    nlohmann::json result = LParser::skinprefabs(ifs);

    cout << result.dump(4) << std::endl;
    cout << result.size() << std::endl;

    LParser::processSkinprefabs("zhnkc9@gmail.com_", result);

    std::ofstream os(R"(D:\C++\skin-generator\test_skins_generator\testprefab.lua)");

    //    // 创建 Inja 渲染器
    inja::Environment env;
    env.set_trim_blocks(true);

    // 从文件读取模板内容
    std::ifstream templateFile(R"(D:\C++\skin-generator\test_skins_generator\template.tpl)");
    std::string templateStr((std::istreambuf_iterator<char>(templateFile)),
                            std::istreambuf_iterator<char>());

    nlohmann::json binddata = {{"data",   result},
                               {"prefix", "zhnkc9@gmail.com_"}};
    inja::CallbackFunction dump = ([](inja::Arguments &args) {
        return args[0]->dump();
    });
    inja::CallbackFunction trip = [](inja::Arguments &args) {
        std::string c = args[0]->get<std::string>();
        std::string c2 = args[1]->get<std::string>();
        util::removeSurroundingCharsRef(c, c2);
        return nlohmann::json(c);
    };
    inja::CallbackFunction has = [](inja::Arguments &args) {
        auto c2 = args[0]->get<nlohmann::json>();
        std::string c = args[1]->get<std::string>();
        return nlohmann::json(c2.contains(c));
    };
    env.add_callback("trip", trip);
    env.add_callback("has", has);
    env.add_callback("dump", dump);
    try {
        std::string rendered = env.render(templateStr, binddata);
        os << rendered << std::endl;
    } catch (const inja::RenderError &e) {
        std::cerr << "Render error: " << e.what() << std::endl;
    }
}

TEST(TEST_LUACODE_PARSER, test_render_register_inv) {
    std::ifstream ifs("D:\\inventoryimages1.xml");
    auto res = LParser::xml(ifs);

    // print res
    cout << res.dump(4) << std::endl;

    TemplateEngine engine;

    json binddata = {{"data", res}};

    std::ifstream iftpl(R"(D:\C++\skin-generator\test_skins_generator\register_inv.tpl)");
    auto r = engine.renderTemplate(iftpl, binddata);

    cout << r << std::endl;

}


TEST(TEST_LUACODE_PARSER, test_clothing_curios) {
    std::ifstream ifs("D:\\饥荒\\Local Collection Skins\\buildtool\\temp\\clothing.lua");
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

    cout << result.dump(4) << endl;

}
