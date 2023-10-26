////
//// Created by zhnkc9 on 2023/10/18.
////
//
//#include <gtest/gtest.h>
//#include "common_headers.hpp"
//#include "LParser.hpp"
//#include "LBuilder.hpp"
//#include "LExtractor.h"
//#include "Gui.h"
//#include "util.h"
//
//namespace fs = std::filesystem;
//
//json flat_defs(json &defs) {
//    json flat;
//    for (auto it = defs.begin(); it != defs.end(); it++) {
//        auto &v = it.value().get_ref<json::array_t &>();
//        for (auto &s: v) {
//            flat[s] = true;
//        }
//    }
//    return flat;
//}
//
//json all_skin(const json &_skins, const json &defs) {
//    json skins = _skins;
//    for (auto it = skins.begin(); it != skins.end();) {
//        if (it.key().find("beard") != std::string::npos) {
//            it = skins.erase(it);
//            continue;
//        }
//        auto &v = it.value().get_ref<json::array_t &>();
//        erase_if(v, [&defs](const string &s) {
//            return !defs.contains(s);
//        });
//        if (v.empty()) {
//            it = skins.erase(it);
//            continue;
//        }
//        it++;
//    }
//    return skins;
//}
//
//json prefab_skin_build(json skins, json &defs) {
//    // 这个的data 当作是选择完的数据，用他和 skindef的key对比，
//    // 包含的话直接把 skindef的value拉平
//    std::map<string, bool> builds;
//    for (auto &it: skins.items()) {
//        auto &vs = it.value().get_ref<json::array_t &>();
//        for (auto &s: vs) {
//            if (defs.contains(s)) {
//                auto &v = defs[s].get_ref<json::array_t &>();
//                for (auto &s2: v) {
//                    builds[s2] = true;
//                }
//            }
//        }
//    }
//    return builds;
//}
//
//void process_prefab(string &prefix, json &skin_prefabs, json &builds) {
//    LParser::processSkinprefabs(prefix, skin_prefabs);
//    cout << "skin_prefabs<<R<<size " << skin_prefabs.size() << endl;
//
//    for (auto it = skin_prefabs.begin(); it != skin_prefabs.end();) {
//        if (!builds.contains(it.key())) {
//            cout << "skin_prefabs delete: " << it.key() << endl;
//            it = skin_prefabs.erase(it);
//        } else
//            ++it;
//
//    }
//    cout << "skin_prefabs<<R<<size " << skin_prefabs.size() << endl;
//}
//
//void transfer_asset(json &skin_prefabs, const fs::path &dest, LExtractor &extractor) {
//    auto &&animAcc = extractor.animAccessor();
//    auto &&bigAcc = extractor.bigportraitsAccessor();
//    auto &&xmlAcc = extractor.xmlAccessor();
//
//    std::stringstream target;
//
//    // 不验证资源，直接传送
//    for (auto &it: skin_prefabs.items()) {
//
//        LOG(debug) << " transfer_asset " << it.key();
//
//        auto &skin_prefab = it.value().get_ref<json::object_t &>();
//
//        auto &assets = skin_prefab["assets"].get_ref<json::array_t &>();
//
//        for (auto &&item: assets) {
//
//            auto &resourceType = item["resourceType"].get_ref<json::number_integer_t &>();
//            auto &origin_name = item["originName"].get_ref<string &>();
//            auto &name = item["name"].get_ref<string &>();
//            auto &ext = item["ext"].get_ref<string &>();
//            auto &path = item["path"].get_ref<string &>();
//            target << (dest / path / name).string() << ext;
//
//            if (resourceType == ITEM) {
//                bool process = ext == ".zip";
//                // zip and dyn
//                animAcc.transfer(origin_name + ext, target.str(), process);
//            } else if (resourceType == BIGPORTRAITS) {
//                bool process = ext == ".xml";
//                bigAcc.transfer(origin_name + ext, target.str(), process);
//            }else if (resourceType == IMAGES){
//                bool process = ext == ".xml";
//                xmlAcc.transfer(origin_name + ext, target.str(), process);
//            }
//            target.str("");
//        }
//    }
//}
//
//#define TRANSFER_IF_NOT_EXIST(source, destination, transferFunction) \
//    if (fs::exists(destination)) { \
//        LOG(info) << source << " : using cache " << destination << std::endl; \
//    } else { \
//        transferFunction(source,destination); \
//    }
//
//
//void CopyDirectoryContents(const fs::path &sourceDir, const fs::path &destinationDir) {
//    try {
//        if (!fs::exists(destinationDir)) {
//            fs::create_directory(destinationDir);
//        }
//
//        for (const auto &entry: fs::directory_iterator(sourceDir)) {
//            const fs::path sourceFile = entry.path();
//            const fs::path destinationFile = destinationDir / sourceFile.filename();
//
//            if (fs::is_directory(sourceFile)) {
//                // Recursively copy subdirectories
//                CopyDirectoryContents(sourceFile, destinationFile);
//            } else {
//                // Copy the file
//                fs::copy_file(sourceFile, destinationFile, fs::copy_options::overwrite_existing);
//            }
//        }
//    } catch (const std::exception &e) {
//        std::cerr << "Error: " << e.what() << std::endl;
//    }
//}
//
//TEST(TEST2, TEST3) {
//    boost::log::core::get()->
//            set_filter(boost::log::trivial::severity
//                       >= boost::log::trivial::info);
//    string gamepath(R"(E:\game\Steam\steamapps\common\Don't Starve Together)");
//    string prefix("ms_qwert_");
//    fs::path workspace(R"(E:\game\Steam\steamapps\common\Don't Starve Together\mods)");
//    string modname("Local Collection Skins_GENERATOR");
//    string zip("resource/source.zip");
//    auto engine = TemplateEngine();
//
//    // todo for dev 手动复制
//    CopyDirectoryContents("D:\\饥荒\\Local Collection Skins_gen", workspace / modname);
//
//    // todo for test
////    if (util::unzip(zip, (workspace / modname).string()) != 0) {
////        LOG(error) << "unzip fail" << endl;
////        return;
////    }
//
//    LExtractor extractor(gamepath, prefix);
//    LAccessor &&scriptAcc = extractor.scriptAccessor();
//    LAccessor &&xmlAcc = extractor.xmlAccessor();
//
//    // generate path
//    auto prefabs_curios = workspace / modname / "scripts/prefabs/kleiskinprefabs.lua";
//    auto clothing_curios = workspace / modname / "scripts/clothing_curios.lua";
//    auto misc_curios = workspace / modname / "scripts/misc_curios.lua";
//    auto register_skin = workspace / modname / "scripts/register_skin.lua";
//
//    // 临时路径
//    auto prefabskins_file = workspace / modname / "tmp" / "prefabskins.lua";
//    auto skins_defs_file = workspace / modname / "tmp" / "skins_defs_data.lua";
//    auto skinprefabs_file = workspace / modname / "tmp" / "skinprefabs.lua";
//    auto clothing_file = workspace / modname / "tmp" / "clothing.lua";
//
//    TRANSFER_IF_NOT_EXIST("prefabskins.lua", prefabskins_file, scriptAcc.transfer)
//    TRANSFER_IF_NOT_EXIST("skins_defs_data.lua", skins_defs_file, scriptAcc.transfer)
//    TRANSFER_IF_NOT_EXIST("prefabs\\skinprefabs.lua", skinprefabs_file, scriptAcc.transfer)
//    TRANSFER_IF_NOT_EXIST("clothing.lua", clothing_file, scriptAcc.transfer)
//
//    json &&prefabskins = LParser::prefabskins(prefabskins_file);
//    json &&skinsdefsdata = LParser::distinctSkinsdefsdata(skins_defs_file);
//
//    // 皮肤合集，1个prefab 多个皮肤
//    json &&skins = all_skin(prefabskins, skinsdefsdata);
//    // 针对 skinprefabs builds 合集 1个皮肤多个build
//    // 这里主要还是给gui用的
//    json prefab_builds = prefab_skin_build(skins, skinsdefsdata);
//
//    // 所有的构建都在其中
//    // 和他取交集
//    json all_builds = flat_defs(skinsdefsdata);
//
//    // 各个皮肤lua的处理、去重、渲染
//    json &&skin_prefabs = LParser::skinprefabs(skinprefabs_file);
//    // 只要这里去重做对了，就不会出现找不到资源的问题
//    process_prefab(prefix, skin_prefabs, prefab_builds);
//    std::thread workerThread([&skin_prefabs, &extractor, &workspace, &modname]() {
//        try {
//            transfer_asset(skin_prefabs, workspace / modname, extractor);
//        }
//        catch (const std::exception &e) {
//            // 捕获异常并打印错误消息
//            std::cerr << "发生错误: " << e.what() << std::endl;
//        }
//    });
//
//    // 衣服处理
//    json &&clothing = LParser::clothing(clothing_file);
//    process_prefab(prefix, clothing, all_builds);
//    transfer_asset(clothing, workspace / modname, extractor);
//
//    // xml
//    xmlAcc.transfer("inventoryimages1.tex", (workspace / modname / "images" / (prefix + "inventoryimages1.tex")).string());
//    xmlAcc.transfer("inventoryimages2.tex", (workspace / modname / "images" / (prefix + "inventoryimages2.tex")).string());
//    xmlAcc.transfer("inventoryimages3.tex", (workspace / modname / "images" / (prefix + "inventoryimages3.tex")).string());
//    xmlAcc.transfer("inventoryimages1.xml", (workspace / modname / "images" / (prefix + "inventoryimages1.xml")).string(), true);
//    xmlAcc.transfer("inventoryimages2.xml", (workspace / modname / "images" / (prefix + "inventoryimages2.xml")).string(), true);
//    xmlAcc.transfer("inventoryimages3.xml", (workspace / modname / "images" / (prefix + "inventoryimages3.xml")).string(), true);
//
//    std::ifstream prefabs_tpl(R"(resource/skinprefabs.tpl)");
//    std::ifstream clothing_tpl(R"(resource/clothing.tpl)");
//    std::ifstream register_inv_tpl(R"(resource/register_inv.tpl)");
//    std::string register_inv_tpl_str((std::istreambuf_iterator<char>(register_inv_tpl)), std::istreambuf_iterator<char>());
//
//    // code
//    // register_inv_tpl to string
//    auto inv1 = LParser::xml((workspace / modname / ("images/" + prefix + "inventoryimages1.xml")).string());
//    auto inv2 = LParser::xml((workspace / modname / ("images/" + prefix + "inventoryimages2.xml")).string());
//    auto inv3 = LParser::xml((workspace / modname / ("images/" + prefix + "inventoryimages3.xml")).string());
//    std::ofstream(register_skin) << engine.renderTemplate(register_inv_tpl_str, json{{"data", inv1}})
//                                 << engine.renderTemplate(register_inv_tpl_str, json{{"data", inv2}})
//                                 << engine.renderTemplate(register_inv_tpl_str, json{{"data", inv3}});
//
//    std::ofstream(prefabs_curios) << engine.renderTemplate(prefabs_tpl, json{{"data",   skin_prefabs},
//                                                                             {"prefix", prefix}});
//    std::ofstream(clothing_curios) << engine.renderTemplate(clothing_tpl, json{{"data",   clothing},
//                                                                               {"prefix", prefix}});
//
//    workerThread.join();
//}
//
////json _skin_data(string game_path) {
////    LExtractor extractor = LExtractor(game_path);
////
////    LAccessor &&accessor = extractor.scriptAccessor();
////    auto workspace = fs::current_path();
////    auto prefabskins = workspace / "tmp" / "prefabskins.lua";
////    auto skins_defs = workspace / "tmp" / "skins_defs_data.lua";
////
////    accessor.transfer("prefabskins.lua", prefabskins.string());
////    accessor.transfer("skins_defs_data.lua", skins_defs.string());
////
////    std::ifstream skins_defs_ifs(skins_defs);
////    const json &defs = LParser::distinctSkinsdefsdata(skins_defs_ifs);
////
////    std::ifstream prefabskins_ifs(prefabskins);
////    json &&skins = LParser::prefabskins(prefabskins_ifs);
////    for (auto it = skins.begin(); it != skins.end();) {
////        if (it.key().find("beard") != std::string::npos) {
////            it = skins.erase(it);
////            continue;
////        }
////        // 获取 ref
////        auto &v = it.value().get_ref<json::array_t &>();
////
////        erase_if(v, [&defs](const string &s) {
////            return !defs.contains(s);
////        });
////
////        if (v.empty()) {
////            it = skins.erase(it);
////            continue;
////        }
////
////        it++;
////    }
////
////    return skins;
////
////}
////
////TEST(test_gen, test_gen) {
////    string gamepath("E:\\game\\Steam\\steamapps\\common\\Don't Starve Together");
////
////    // 先把文件提取，todo 如果有缓存就用缓存
////    LExtractor extractor(gamepath, "zhnkc9@gmail.com_");
////
////    LAccessor &&xmlAcc = extractor.xmlAccessor();
////    LAccessor &&scriptAcc = extractor.scriptAccessor();
////    AnimAccessor &&animAcc = extractor.animAccessor();
////    LAccessor &&bigAcc = extractor.bigportraitsAccessor();
////
////    auto skins_defs = "D:\\C++\\skin-generator\\cmake-build-release\\test_skins_generator\\tmp\\skins_defs_data.lua";
////
////    std::ifstream skin_defs_ifs(skins_defs);
////    json &&defs = LParser::distinctSkinsdefsdata(skin_defs_ifs);
////
//////    scriptAcc.transfer(SKIN_DEF, TMP_DIR "/" SKIN_DEF);
////
////    json &&data = _skin_data(gamepath);
////
////    // 这个的data 当作是选择完的数据，用他和 skindef的key对比，
////    // 包含的话直接把 skindef的value拉平
////    std::map<string, bool> skins;
////    for (auto &it: data.items()) {
////        auto &vs = it.value().get_ref<json::array_t &>();
////        for (auto &s: vs) {
////            if (defs.contains(s)) {
////                auto &v = defs[s].get_ref<json::array_t &>();
////                for (auto &s2: v) {
////                    skins[s2] = true;
////                }
////            }
////        }
////    }
////
////    // 获取 skinprefabs
////    std::ifstream skin_prefabs_ifs("D:\\C++\\skin-generator\\cmake-build-release\\test_skins_generator\\tmp\\skinprefabs.lua");
////    json &&skin_prefabs = LParser::skinprefabs(skin_prefabs_ifs);
////
////    // skin_prefabs to 1.txt
////    std::ofstream ofs("skin_prefabs.txt");
////    std::ofstream ofs2("skins.txt");
////    ofs << skin_prefabs.dump(4) << endl;
////    ofs2 << json(skins).dump(4) << endl;
////
////    cout << "skin_prefabs<<size " << skin_prefabs.size() << endl;
////
////    for (auto &it: skin_prefabs.items()) {
////        // 交集
////        if (!skins.contains(it.key())) {
////            cout << "skin_prefabs<<delete:< " << it.key() << endl;
////            skin_prefabs.erase(it.key());
////            continue;
////        }
////    }
////
////    cout << "skin_prefabs<<R<<size " << skin_prefabs.size() << endl;
////
////    LParser::processSkinprefabs("zhnkc9@gmail.com_", skin_prefabs);
////
////    std::stringstream target;
////
////    // 不验证资源，直接传送
////    for (auto &it: skin_prefabs.items()) {
////
////        auto &skin_prefab = it.value().get_ref<json::object_t &>();
////
////        auto &assets = skin_prefab["assets"].get_ref<json::array_t &>();
////
////        for (auto &&item: assets) {
////
////            auto &resourceType = item["resourceType"].get_ref<json::number_integer_t &>();
////            auto &origin_name = item["originName"].get_ref<string &>();
////            auto &name = item["name"].get_ref<string &>();
////            auto &ext = item["ext"].get_ref<string &>();
////            auto &path = item["path"].get_ref<string &>();
////            target << TMP_DIR << "/" << path << "/" << name << ext;
////
////            if (resourceType == ITEM) {
////                bool process = ext == ".zip";
////                // zip and dyn
////                animAcc.transfer(origin_name + ext, target.str(), process);
////            } else if (resourceType == BIGPORTRAITS) {
////                bool process = ext == ".xml";
////                bigAcc.transfer(origin_name + ext, target.str(), process);
////            }
////
////            target.str("");
////
////        }
////
////    }
////
//////    cout << skin_prefabs.dump(4) << endl;
////
////    // 循环，验证，每一项 挨个转移 资源，根据 asset
////
////    // 提取 skindef
////
//
