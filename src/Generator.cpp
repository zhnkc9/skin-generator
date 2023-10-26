////
//// Created by zhnkc9 on 2023/10/18.
////
//
//#include "common_headers.hpp"
//#include "LParser.hpp"
//#include "LBuilder.hpp"
//#include "LExtractor.h"
//#include "Gui.h"
//
//namespace fs = std::filesystem;
//
//#define ModVersion "0.1.beta"
//#define ModProxyPrefix "zhnkc9@gmail.com_"
//#define ModName "Local Collection Skins"
//#define TmpPath "tmp"
//#define SkinPrefabsWritePath ModName"/scripts/prefabs/kleiskinprefabs.lua"
//#define ClothingWritePath ModName"/scripts/clothing_curios.lua"
//#define RegisterSkinWritePath ModName"/scripts/register_skin.lua"
//#define ModInfoWritePath ModName"/modinfo.lua"
//#define ModMainWritePath ModName"/modmain.lua"
//
//
//#define RENDER_TEMPLATE_TO_FILE(templateFile, jsonData, outputFile)                \
//    do {                                                                         \
//        std::ifstream templateStream(templateFile);                              \
//        if (!templateStream.is_open()) {                                         \
//            LOG(error) << "Error opening template file: " << templateFile << std::endl; \
//            break;                                                               \
//        }                                                                        \
//        std::string templateStr((std::istreambuf_iterator<char>(templateStream)),  \
//                                std::istreambuf_iterator<char>());                 \
//        std::string rendered = engine.renderTemplate(templateStr, jsonData);       \
//        std::ofstream(outputFile) << rendered;                                    \
//    } while (false)
//
//
//#define TRANSFER_IF_NOT_EXIST(source, destination, transferFunction) \
//    if (fs::exists(destination)) { \
//        LOG(info) << source << " : using cache " << destination << std::endl; \
//    } else { \
//        transferFunction(source,destination); \
//    }
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
//void process_prefab(const string &prefix, json &skin_prefabs, json &builds) {
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
//            } else if (resourceType == IMAGES) {
//                bool process = ext == ".xml";
//                xmlAcc.transfer(origin_name + ext, target.str(), process);
//            }
//            target.str("");
//        }
//    }
//}
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
//
//void genetate(const string &game_path, const fs::path &workspace) {
//
//    string zip("resource/source.zip");
//    auto engine = TemplateEngine();
//    json renderData;
//    renderData["modname"] = ModName;
//    renderData["version"] = ModVersion;
//    renderData["prefix"] = ModProxyPrefix;
//    auto &&modPath = workspace / ModName;
//
//    if (util::unzip(zip, modPath.string()) != 0) {
//        LOG(error) << "unzip fail" << endl;
//        return;
//    }
//
//    LExtractor extractor(game_path, ModProxyPrefix);
//    LAccessor &&scriptAcc = extractor.scriptAccessor();
//    LAccessor &&xmlAcc = extractor.xmlAccessor();
//
//    // 构建缓存
//    auto prefabskins_file = TmpPath  "/prefabskins.lua";
//    auto skins_defs_file = TmpPath "/skins_defs_data.lua";
//    auto skinprefabs_file = TmpPath "/skinprefabs.lua";
//    auto clothing_file = TmpPath "/clothing.lua";
//
//    {
//        // 构造文件生成
//        TRANSFER_IF_NOT_EXIST("prefabskins.lua", prefabskins_file, scriptAcc.transfer)
//        TRANSFER_IF_NOT_EXIST("skins_defs_data.lua", skins_defs_file, scriptAcc.transfer)
//        TRANSFER_IF_NOT_EXIST("prefabs/skinprefabs.lua", skinprefabs_file, scriptAcc.transfer)
//        TRANSFER_IF_NOT_EXIST("clothing.lua", clothing_file, scriptAcc.transfer)
//    }
//
//    {
//        json &&prefabskins = LParser::prefabskins(prefabskins_file);
//        json &&skinsdefsdata = LParser::distinctSkinsdefsdata(skins_defs_file);
//
//        // 皮肤合集，1个prefab 多个皮肤
//        json &&skins = all_skin(prefabskins, skinsdefsdata);
//        // 针对 skinprefabs builds 合集 1个皮肤多个build
//        // 这里主要还是给gui用的
//        json prefab_builds = prefab_skin_build(skins, skinsdefsdata);
//
//        // 所有的构建都在其中
//        // 和他取交集
//        json all_builds = flat_defs(skinsdefsdata);
//        // 只要这里去重做对了，就不会出现找不到资源的问题
//        // item 处理
//        json &&skin_prefabs = LParser::skinprefabs(skinprefabs_file);
//        process_prefab("" ModProxyPrefix, skin_prefabs, prefab_builds);
//        renderData["skin_prefabs"] = skin_prefabs;
//        // 衣服处理
//        json &&clothing = LParser::clothing(clothing_file);
//        process_prefab(ModProxyPrefix, clothing, all_builds);
//        renderData["clothing"] = skin_prefabs;
//    }
//
//    {
//        // xml 资源生成
//        AssertBuilder as(ModProxyPrefix);
//        as.with(IMAGES, "images1");
//        as.with(IMAGES, "images2");
//        as.with(IMAGES, "images3");
//        renderData["invAssets"] = {"assets", as.build()};
//
//    }
//
//    {
//        // xml
//        auto inv1 = LParser::xml((modPath / "images/" ModProxyPrefix "inventoryimages1.xml").string());
//        auto inv2 = LParser::xml((modPath / "images/" ModProxyPrefix "inventoryimages2.xml").string());
//        auto inv3 = LParser::xml((modPath / "images/" ModProxyPrefix "inventoryimages3.xml").string());
//        renderData["inv1"] = inv1;
//        renderData["inv2"] = inv1;
//        renderData["inv3"] = inv1;
//    }
//
//    {
//        // 传送
////        transfer_asset(renderData["skin_prefabs"], modPath, extractor);
////        transfer_asset(renderData["clothing"], modPath, extractor);
////        transfer_asset(renderData["invAssets"], modPath, extractor);
//    }
//
//    RENDER_TEMPLATE_TO_FILE(R"(resource/skinprefabs.tpl)", renderData, modPath / SkinPrefabsWritePath);
//    RENDER_TEMPLATE_TO_FILE(R"(resource/clothing.tpl)", renderData, modPath / ClothingWritePath);
//    RENDER_TEMPLATE_TO_FILE(R"(resource/register_inv.tpl)", renderData, modPath / RegisterSkinWritePath);
//    RENDER_TEMPLATE_TO_FILE(R"(resource/modinfo.tpl)", renderData, modPath / ModInfoWritePath);
//    RENDER_TEMPLATE_TO_FILE(R"(resource/modmain.tpl)", renderData, modPath / ModMainWritePath);
//
//}
