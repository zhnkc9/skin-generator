//
// Created by zhnkc9 on 2023/10/11.
//
#include "common_headers.hpp"
#include "LParser.hpp"
#include "LBuilder.hpp"
#include "LExtractor.h"
#include "Gui.h"
#include "config.h"
#include <unordered_set>
#include <shlobj.h>

namespace fs = std::filesystem;

#define ModVersion PROJECT_VERSION
#define ModProxyPrefix "ms_zhnkc9_gmail_com_"
#define ModName "Local Collection Skins"
#define TmpPath "tmp"
#define SkinPrefabsWritePath "scripts/prefabs/kleiskinprefabs.lua"
#define ClothingWritePath "scripts/clothing_curios.lua"
#define RegisterSkinWritePath "scripts/register_skin.lua"
#define ModInfoWritePath "modinfo.lua"
#define ModMainWritePath "modmain.lua"
#define ModInitWritePath "scripts/init.lua"


[[maybe_unused]] char *selectFolder() {
    BROWSEINFO bi = {nullptr};
    bi.ulFlags = BIF_USENEWUI;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != nullptr) {
        char *path = new char[MAX_PATH];
        SHGetPathFromIDList(pidl, path);
        CoTaskMemFree(pidl);
        return path;
    }
    return nullptr;
}


[[maybe_unused]] char *skins_data(const char *game_path) {
    try {
        LExtractor extractor = LExtractor(game_path);

        LAccessor &&accessor = extractor.scriptAccessor();
        auto workspace = fs::current_path();
        auto prefabskins = workspace / "tmp" / "prefabskins.lua";
        auto skins_defs = workspace / "tmp" / "skins_defs_data.lua";

        accessor.transfer("prefabskins.lua", prefabskins.string());
        accessor.transfer("skins_defs_data.lua", skins_defs.string());

        std::ifstream skins_defs_ifs(skins_defs);
        const json &defs = LParser::distinctSkinsdefsdata(skins_defs_ifs);

        std::ifstream prefabskins_ifs(prefabskins);
        json &&skins = LParser::prefabskins(prefabskins_ifs);
        for (auto it = skins.begin(); it != skins.end();) {
            if (it.key().find("beard") != std::string::npos) {
                it = skins.erase(it);
                continue;
            }
            auto &v = it.value().get_ref<json::array_t &>();

            erase_if(v, [&defs](const string &s) {
                return !defs.contains(s);
            });

            if (v.empty()) {
                it = skins.erase(it);
                continue;
            }

            it++;
        }
        std::string res = skins.dump();
        char *cstr = new char[res.length() + 1];
        strcpy(cstr, res.c_str());
        return cstr;
    }
    catch (const std::exception &e) {
        LOG(error) << "Exception caught: " << e.what() << std::endl;
        return new char[6]{'E', 'R', 'R', 'O', 'R', '\0'};
    }
}

[[maybe_unused]] char *skin_i8n(const char *game_path) {
    LExtractor extractor = LExtractor(game_path);

    LAccessor &&accessor = extractor.scriptAccessor();
    auto workspace = fs::current_path();
    auto I8N_path = workspace / "tmp" / "chinese_s.po";
    accessor.transfer("languages/chinese_s.po", I8N_path.string());

    std::ifstream ifs(I8N_path);
    const json &json = LParser::chinesepo(ifs);
    std::string str = json.dump();
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}

[[maybe_unused]] void init_log(const char *path) {
    logging::add_file_log(
            boost::log::keywords::file_name = path,
            boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] %Message%",
            boost::log::keywords::auto_flush = true
    );
    BOOST_LOG_TRIVIAL(info) << "init logging ....";
}


class OutsideLoggerSink : public boost::log::sinks::basic_formatted_sink_backend<char> {

private:
    LogCallBack callback;

public:

    explicit OutsideLoggerSink(LogCallBack callback) : callback(callback) {}

    void consume(logging::record_view const &rec, const std::string &message) {
        int level = rec.attribute_values()["Severity"].extract<logging::trivial::severity_level>().get();
        callback(level, message.c_str());
    }

};

[[maybe_unused]] __declspec(dllexport) void register_log_callback(LogCallBack callback) {

    logging::core::get()->remove_all_sinks();

    boost::shared_ptr<boost::log::sinks::synchronous_sink<OutsideLoggerSink>> sink =
            boost::make_shared<boost::log::sinks::synchronous_sink<OutsideLoggerSink >>(boost::make_shared<OutsideLoggerSink>(callback));

    logging::core::get()->add_sink(sink);

    LOG(info) << "register_log_callback ...";

}


#define RENDER_TEMPLATE_TO_FILE(templateFile, jsonData, outputFile)                \
    do {                                                                           \
        LOG(info) << "render " << templateFile << " => " << outputFile ;           \
        std::ifstream templateStream(templateFile);                              \
        if (!templateStream.is_open()) {                                         \
            LOG(error) << "Error opening template file: " << templateFile << std::endl; \
            break;                                                               \
        }                                                                        \
        std::string templateStr((std::istreambuf_iterator<char>(templateStream)),  \
                                std::istreambuf_iterator<char>());                 \
        std::string rendered = engine.renderTemplate(templateStr, jsonData);       \
        std::ofstream(outputFile) << rendered;                                    \
    } while (false)


#define TRANSFER_IF_NOT_EXIST(source, destination, transferFunction, process) \
    if (fs::exists(destination)) { \
        LOG(info) << source << " : using cache " << destination << std::endl; \
    } else { \
        transferFunction(source,destination,process); \
    }

json flat_defs(json &defs) {
    json flat;
    for (auto it = defs.begin(); it != defs.end(); it++) {
        auto &v = it.value().get_ref<json::array_t &>();
        for (auto &s: v) {
            flat[s] = true;
        }
    }
    return flat;
}

json all_skin(const json &_skins, const json &defs) {
    json skins = _skins;
    for (auto it = skins.begin(); it != skins.end();) {
        if (it.key().find("beard") != std::string::npos) {
            it = skins.erase(it);
            continue;
        }
        auto &v = it.value().get_ref<json::array_t &>();
        erase_if(v, [&defs](const string &s) {
            return !defs.contains(s);
        });
        if (v.empty()) {
            it = skins.erase(it);
            continue;
        }
        it++;
    }
    return skins;
}

json prefab_skin_build(json skins, json &defs) {
    std::map<string, bool> builds;
    for (auto &it: skins.items()) {
        auto &vs = it.value().get_ref<json::array_t &>();
        for (auto &s: vs) {
            if (defs.contains(s)) {
                auto &v = defs[s].get_ref<json::array_t &>();
                for (auto &s2: v) {
                    builds[s2] = true;
                }
            }
        }
    }
    return builds;
}

void process_prefab(const string &prefix, json &skin_prefabs, json &builds, std::unordered_set<string> &filter) {
    LParser::processSkinprefabs(prefix, skin_prefabs);

    auto enable_filter = filter.size() > 0;

    cout << "skin_prefabs<<BEFORE_R<<size " << skin_prefabs.size() << endl;

    for (auto it = skin_prefabs.begin(); it != skin_prefabs.end();) {

        if (enable_filter and filter.count(it.key()) == 0) {
            cout << "skin_prefabs delete by filter: " << it.key() << endl;
            it = skin_prefabs.erase(it);
        }else if (!builds.contains(it.key())) {
            cout << "skin_prefabs delete: " << it.key() << endl;
            it = skin_prefabs.erase(it);
        } else
            ++it;

    }
    cout << "skin_prefabs<<R<<size " << skin_prefabs.size() << endl;
}

void transfer_asset(json &skin_prefabs, const fs::path &dest, LExtractor &extractor) {

    if (skin_prefabs.size() == 0)
        return;

    auto &&animAcc = extractor.animAccessor();
    auto &&bigAcc = extractor.bigportraitsAccessor();
    auto &&xmlAcc = extractor.xmlAccessor();

    std::stringstream target;

    // 资源不给定，只能离线用
    std::unordered_set<string> skip_type = {"base", "body", "hand", "legs", "feet"};

    for (auto &it: skin_prefabs.items()) {

        auto &skin_prefab = it.value().get_ref<json::object_t &>();

        LOG(debug) << " transfer_asset " << it.key() << " " << it.value() << " " << skin_prefab["type"];

//        if (skin_prefab["type"] != nullptr){
//            auto && type  = util::removeSurroundingChars(skin_prefab["type"], "\"");
//            if (skip_type.count(type) > 0) continue;
//        }

        auto &assets = skin_prefab["assets"].get_ref<json::array_t &>();

        for (auto &&item: assets) {

            auto &resourceType = item["resourceType"].get_ref<json::number_integer_t &>();
            auto &origin_name = item["originName"].get_ref<string &>();
            auto &name = item["name"].get_ref<string &>();
            auto &ext = item["ext"].get_ref<string &>();
            auto &path = item["path"].get_ref<string &>();
            target << (dest / path / name).string() << ext;


            if (resourceType == ITEM) {
                bool process = ext == ".zip";
                if (process) {
                    TRANSFER_IF_NOT_EXIST(origin_name + ext, target.str(), animAcc.transfer, process)
                }
            } else if (resourceType == BIGPORTRAITS) {
                bool process = ext == ".xml";
                if (process) {
                    TRANSFER_IF_NOT_EXIST(origin_name + ext, target.str(), bigAcc.transfer, process)
                }
            } else if (resourceType == IMAGES) {
                bool process = ext == ".xml";
                if (process) {
                    TRANSFER_IF_NOT_EXIST(origin_name + ext, target.str(), xmlAcc.transfer, process)
                }
            }
            target.str("");
        }
    }
}

void CopyDirectoryContents(const fs::path &sourceDir, const fs::path &destinationDir) {
    try {
        if (!fs::exists(destinationDir)) {
            fs::create_directories(destinationDir);
        }

        for (const auto &entry: fs::directory_iterator(sourceDir)) {
            const fs::path &sourceFile = entry.path();
            const fs::path destinationFile = destinationDir / sourceFile.filename();

            if (fs::is_directory(sourceFile)) {
                // Recursively copy subdirectories
                CopyDirectoryContents(sourceFile, destinationFile);
            } else {
                // Copy the file
                fs::copy_file(sourceFile, destinationFile, fs::copy_options::overwrite_existing);
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw e;
    }
}

int main() {
    const char *game_path = R"(E:\game\Steam\steamapps\common\Don't Starve Together)";
//    const char *game_path = selectFolder();
    try {
        LOG(info) << "start generate ... " << game_path << " <= " << ModName;
        if (game_path == nullptr) {
            LOG(error) << "game_path is null";
            return 1;
        }
        auto engine = TemplateEngine();
        json renderData;
        renderData["modname"] = ModName;
        renderData["version"] = ModVersion;
        renderData["prefix"] = ModProxyPrefix;
        auto &&destPath = fs::path(game_path) / "mods\\" ModName;

        // 手动复制
        CopyDirectoryContents(R"(resource\source)", destPath);

        LExtractor extractor(game_path, ModProxyPrefix);
        LAccessor &&scriptAcc = extractor.scriptAccessor();

        auto prefabskins_file = TmpPath  "/prefabskins.lua";
        auto skins_defs_file = TmpPath "/skins_defs_data.lua";
        auto skinprefabs_file = TmpPath "/skinprefabs.lua";
        auto clothing_file = TmpPath "/clothing.lua";
        auto chinese_po_file = TmpPath "/chinese_s.po";

        {
            TRANSFER_IF_NOT_EXIST("prefabskins.lua", prefabskins_file, scriptAcc.transfer, false)
            TRANSFER_IF_NOT_EXIST("skins_defs_data.lua", skins_defs_file, scriptAcc.transfer, false)
            TRANSFER_IF_NOT_EXIST("prefabs/skinprefabs.lua", skinprefabs_file, scriptAcc.transfer, false)
            TRANSFER_IF_NOT_EXIST("clothing.lua", clothing_file, scriptAcc.transfer, false)
            TRANSFER_IF_NOT_EXIST("languages/chinese_s.po", chinese_po_file, scriptAcc.transfer, false)
        }

        {
            json &&chinese_po = LParser::chinesepo(chinese_po_file);
            json &&prefabskins = LParser::prefabskins(prefabskins_file);
            json &&skinsdefsdata = LParser::distinctSkinsdefsdata(skins_defs_file);

            json &&skins = all_skin(prefabskins, skinsdefsdata);
            json prefab_builds = prefab_skin_build(skins, skinsdefsdata);

            json all_builds = flat_defs(skinsdefsdata);

            ifstream filter_file("resource/filter.txt");
            std::unordered_set<string> filter;
            if (filter_file.is_open()) {
                string line;
                while (getline(filter_file, line)) {
                    if (chinese_po.contains(line)) {
                        LOG(debug) << "filter item: " << chinese_po[line];
                        filter.insert(chinese_po[line]);
                    }
                    LOG(debug) << "filter item: " << line;
                    filter.insert(line);
                }
            }
            LOG(debug) << "filter: " << json(filter);

            json &&skin_prefabs = LParser::skinprefabs(skinprefabs_file);
            process_prefab("" ModProxyPrefix, skin_prefabs, prefab_builds, filter);
            LOG(info) << "generate transfer resource33 ";
            renderData["skin_prefabs"] = skin_prefabs;

            json &&clothing = LParser::clothing(clothing_file);
            LOG(info) << "generate transfer resource1 ";
            process_prefab(ModProxyPrefix, clothing, all_builds, filter);
            LOG(info) << "generate transfer resource2 ";
            renderData["clothing"] = clothing;
        }

        // 图片我就不注册了，直接hook的klei接口
//        {
//            AssertBuilder as(ModProxyPrefix);
//            as.with(IMAGES, "inventoryimages1");
//            as.with(IMAGES, "inventoryimages2");
//            as.with(IMAGES, "inventoryimages3");
//            renderData["invAssets"] = as.build();
//            json arr{{"innerobj", {{"assets", as.build()}}}};
//            transfer_asset(arr, modPath, extractor);
//        }

//        {
//            auto inv1 = LParser::xml((modPath / "images/" ModProxyPrefix "inventoryimages1.xml").string());
//            auto inv2 = LParser::xml((modPath / "images/" ModProxyPrefix "inventoryimages2.xml").string());
//            auto inv3 = LParser::xml((modPath / "images/" ModProxyPrefix "inventoryimages3.xml").string());
//            renderData["inv1"] = inv1;
//            renderData["inv2"] = inv2;
//            renderData["inv3"] = inv3;
//        }

        LOG(info) << "generate transfer resource ";

        transfer_asset(renderData["clothing"], destPath, extractor);
        std::thread workerThread([&renderData, &destPath, &extractor]() {
            try {
                transfer_asset(renderData["skin_prefabs"], destPath, extractor);
            }
            catch (const std::exception &e) {
                // 捕获异常并打印错误消息
                std::cerr << "error: " << e.what() << std::endl;
            }
        });
        workerThread.join();

        LOG(info) << "render data .....";

        RENDER_TEMPLATE_TO_FILE(R"(D:\C++\skin-generator-master\skin-generator-master/resource/skinprefabs.tpl)", renderData,
                                destPath / SkinPrefabsWritePath);
        RENDER_TEMPLATE_TO_FILE(R"(D:\C++\skin-generator-master\skin-generator-master/resource/clothing.tpl)", renderData,
                                destPath / ClothingWritePath);
        RENDER_TEMPLATE_TO_FILE(R"(D:\C++\skin-generator-master\skin-generator-master/resource/register_inv.tpl)", renderData,
                                destPath / RegisterSkinWritePath);
        RENDER_TEMPLATE_TO_FILE(R"(D:\C++\skin-generator-master\skin-generator-master/resource/modinfo.tpl)", renderData,
                                destPath / ModInfoWritePath);
        RENDER_TEMPLATE_TO_FILE(R"(D:\C++\skin-generator-master\skin-generator-master/resource/modmain.tpl)", renderData,
                                destPath / ModMainWritePath);
        RENDER_TEMPLATE_TO_FILE(R"(D:\C++\skin-generator-master\skin-generator-master/resource/init.tpl)", renderData, destPath / ModInitWritePath);


        LOG(info) << "generate success " << destPath;


    } catch (const std::exception &e) {
        LOG(error) << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
}
