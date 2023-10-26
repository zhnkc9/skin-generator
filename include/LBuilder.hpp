//
// Created by zhnkc9 on 2023/10/7.
//

#ifndef SKINS_GENERATOR_LBUILDER_HPP
#define SKINS_GENERATOR_LBUILDER_HPP

#include "inja.hpp"
#include "common_headers.hpp"
#include "util.h"
#include <sstream>

class TemplateEngine {

public:
    TemplateEngine() {
        env.set_trim_blocks(true);
        env.add_callback("dump", dump);
        env.add_callback("trip", trip);
        env.add_callback("has", has);
        env.add_callback("asset2str", asset2str);
        env.add_callback("size", size);
        env.add_callback("origin_asset2str", origin_asset2str);
    }

    std::string renderTemplate(std::ifstream &ifs, const inja::json &data) {
        // ifs to string
        std::string template_text((std::istreambuf_iterator<char>(ifs)),
                                  std::istreambuf_iterator<char>());
        return renderTemplate(template_text, data);
    }

    std::string renderTemplate(const std::string &template_text, const inja::json &data) {
        // 使用模板引擎渲染模板
        return env.render(template_text, data);
    }

private:
    // 自定义回调函数
    static json dump(inja::Arguments &args) {
        return args[0]->dump();
    }

    static json size(inja::Arguments &args) {
        return args[0]->size();
    }

    static json trip(inja::Arguments &args) {
        std::string c = args[0]->get<std::string>();
        std::string c2 = args[1]->get<std::string>();
        util::removeSurroundingCharsRef(c, c2);
        return c;
    };

    static json has(inja::Arguments &args) {
        auto c2 = args[0]->get<nlohmann::json>();
        std::string c = args[1]->get<std::string>();
        return c2.contains(c);
    };

    static json asset2str(inja::Arguments &args) {
        auto asset = args[0]->get<nlohmann::json>();
        asset["type"].get_ref<std::string &>();
        std::stringstream ss;
        ss << "Asset(\"" << asset["type"].get_ref<std::string &>()
           << "\", \"" << asset["path"].get_ref<std::string &>()
           << asset["name"].get_ref<std::string &>()
           << asset["ext"].get_ref<std::string &>()
           << "\")";
        return ss.str();
    };

    static json origin_asset2str(inja::Arguments &args) {
        auto asset = args[0]->get<nlohmann::json>();
        asset["type"].get_ref<std::string &>();
        std::stringstream ss;
        ss << "Asset(\"" << asset["type"].get_ref<std::string &>()
           << "\", \"" << asset["path"].get_ref<std::string &>()
           << asset["originName"].get_ref<std::string &>()
           << asset["ext"].get_ref<std::string &>()
           << "\")";
        return ss.str();
    };


    inja::Environment env;
};


enum ResourceType {
    ITEM,
    BG_LOADING,
    BIGPORTRAITS,
    IMAGES,
};

/*
 * 加上几个类型
 * */
enum AssetType {
    DYNAMIC_ANIM,
    PKGREF,
    IMAGE,
    ATLAS,
    ATLAS_BUILD,
    DYNAMIC_ATLAS,
};

// 映射表
extern std::map<AssetType, std::string> asset2str;

/*
 * 映射路径
 * */
extern std::map<ResourceType, std::string> pathMap;

/*
 * 后缀映射
 * */
extern std::map<AssetType, std::string> extMap;

/*
 * 资源构建
 * */
class Asset : public std::string {
public:
    AssetType type;

    ResourceType resourceType;

    string originName;

    string name;

    string path;

    string ext;

    Asset(AssetType type, ResourceType resourceType, std::string originName, const std::string &name, const std::string &path,
          std::string ext)
            : type(type), resourceType(resourceType), originName(std::move(originName)), name(name), path(path), ext(std::move(ext)) {
        std::stringstream ss;
        ss << "Asset(\"" << asset2str[type] << "\", \"" << path << name << "\")";
        append(ss.str());
    }

    [[nodiscard]] json toJson() const {
        nlohmann::json jsonAsset;
        jsonAsset["type"] = asset2str[type];
        jsonAsset["resourceType"] = resourceType;
        jsonAsset["originName"] = originName;
        jsonAsset["name"] = name;
        jsonAsset["path"] = path;
        jsonAsset["ext"] = ext;
        return jsonAsset;
    }

};

/*
 * 根据类型构建
 * */
class AssertBuilder {

public:
    string prefix;
    json assets = json::array();

    explicit AssertBuilder(string p) : prefix(std::move(p)) {
    }

    void with(ResourceType resource, const string &name) {
        auto _ = build(resource, prefix, name);
        // concat
        assets.insert(assets.end(), _.begin(), _.end());
    }

    [[nodiscard]] json build() const {
        return assets;
    }

    static json build(ResourceType resource, const string &prefix, const string &name) {
        json assets;
        if (resource == ITEM) {
            string &p = pathMap[ITEM];
            assets.emplace_back(Asset(DYNAMIC_ANIM, ITEM, name, prefix + name, p, extMap[DYNAMIC_ANIM]).toJson());
            assets.emplace_back(Asset(PKGREF, ITEM, name, prefix + name, p, extMap[PKGREF]).toJson());
        } else if (resource == BG_LOADING) {
            string &p = pathMap[BG_LOADING];
            assets.emplace_back(Asset(IMAGE, BG_LOADING, name, prefix + name, p, extMap[IMAGE]).toJson());
            assets.emplace_back(Asset(ATLAS, BG_LOADING, name, prefix + name, p, extMap[ATLAS]).toJson());
        } else if (resource == BIGPORTRAITS) {
            string &p = pathMap[BIGPORTRAITS];
            assets.emplace_back(Asset(DYNAMIC_ATLAS, BIGPORTRAITS, name, prefix + name, p, extMap[DYNAMIC_ATLAS]).toJson());
            assets.emplace_back(Asset(PKGREF, BIGPORTRAITS, name, prefix + name, p, extMap[IMAGE]).toJson());
        } else if (resource == IMAGES) {
            string &p = pathMap[IMAGES];
            assets.emplace_back(Asset(IMAGE, IMAGES, name, prefix + name, p, extMap[IMAGE]).toJson());
            assets.emplace_back(Asset(ATLAS, IMAGES, name, prefix + name, p, extMap[ATLAS]).toJson());
        } else {
            throw std::runtime_error("unknown resource type" + std::to_string(resource));
        }
        return assets;
    }
};

#endif //SKINS_GENERATOR_LBUILDER_HPP
