//
// Created by zhnkc9 on 2023/9/30.
//

#ifndef SKINS_GENERATOR_LEXTRACTOR_H
#define SKINS_GENERATOR_LEXTRACTOR_H

#include "zip.h"
#include "common_headers.hpp"
#include <Windows.h>

namespace fs = std::filesystem;

#define TMP_DIR "tmp"

//#define SKIN_DEF "skins_defs_data.lua"

/*
 * 直接从folder或者zip中拿文件
 */
struct LAccessor {
    fs::path folder;
    // 这个路径必须用 / 分隔
    std::string path_prefix;
    struct zip *zipfile = nullptr;
    char buffer[4096]{};

    LAccessor(const fs::path &folder, const string &path_prefix) : zipfile(nullptr) {
        this->folder = folder;
        this->path_prefix = path_prefix;
    }

    LAccessor(const fs::path &folder, const string &path_prefix, const fs::path &zipPath) : \
        LAccessor(folder, path_prefix) {
        LOG(info) << "zip path : " << zipPath;
        this->zipfile = zip_open(zipPath.string().c_str(), ZIP_RDONLY, nullptr);
    }

    virtual void doZipFileCopy(zip_file *file, const fs::path &dest_path) {
        FILE *destFile = fopen(dest_path.string().c_str(), "wb");
        if (destFile != nullptr) {
            zip_int64_t bytesRead;
            while ((bytesRead = zip_fread(file, buffer, sizeof(buffer))) > 0) {
                fwrite(buffer, 1, bytesRead, destFile);
            }
            fclose(destFile);
            LOG(info) << "transfer: unzip success";
        } else {
            LOG(error) << "transfer: fopen destFile failed";
            throw std::runtime_error("transfer: fopen failed " + dest_path.string());
        }
        zip_fclose(file);
    }

    virtual bool doCopy(const fs::path &ori_path, const fs::path &dest_path) {
        return CopyFile(ori_path.string().c_str(), dest_path.string().c_str(), 0);
    }

    virtual void processIfNecessary(const fs::path &dest) {};

    // 根据path
    // path 必须是 / 分隔
    void transfer(const string &path, const fs::path &dest, bool process = false) {

        LOG(info) << "transfer: <= " << folder / path_prefix / path;
        LOG(info) << "transfer: => " << dest;
        auto &&f = folder / path_prefix / path;
        std::filesystem::create_directories(dest.parent_path());
        if (!doCopy(f, dest)) {
            auto p = path_prefix + "/" + path;
            LOG(info) << "transfer: copy failed, try unzip " << p;
            struct zip_file *file = zip_fopen(zipfile, p.c_str(), ZIP_FL_UNCHANGED);
            if (file != nullptr) {
                doZipFileCopy(file, dest);
            } else {
                LOG(error) << "transfer: file not found on zip";
                // 直接终止方法

                throw std::runtime_error("transfer: file not found on zip");
            }
        } else {
            LOG(info) << "transfer: copy success";
        }
        if (process)
            processIfNecessary(dest);
    }

};


class AnimAccessor : public LAccessor {
public:

    std::string prefix;

    explicit AnimAccessor(const fs::path &folder, const std::string &prefix) : \
    LAccessor(folder / "data", "anim/dynamic", folder / "data\\databundles\\anim_dynamic.zip") {
        this->prefix = prefix;
    }

    void processIfNecessary(const fs::path &dest) override {
        LAccessor::processIfNecessary(dest);
        LOG(info) << "transfer: process build.bin";
        const char *bin_name = "build.bin\0";
        const char *c_prefix = prefix.c_str();

        // 打开或创建 ZIP 文件
        struct zip *za = zip_open(dest.string().c_str(), ZIP_CREATE, nullptr);

        if (!za) {
            std::cerr << "error opening/creating ZIP file: " << zip_strerror(za) << std::endl;
            return;
        }
        struct zip_file *bin = zip_fopen(za, bin_name, ZIP_FL_UNCHANGED);

        char *header = new char[16]{};
        zip_fread(bin, header, 16);

        uint32_t name_l;
        zip_fread(bin, reinterpret_cast<char *>(&name_l), 4);

        char *name = new char[name_l + 1]{};
        zip_fread(bin, name, name_l);
        name[name_l] = '\0';

        // maybe: 这里判断一下 name是不是和 给定的那个name一样，不一样给个警告就行了

        auto *buffer = new uint8_t[4096];

        zip_int64_t len;
        std::vector<uint8_t> bin_c;
        // header
        bin_c.insert(bin_c.end(), header, header + 16);
        // 计算长度 prefix, name
        uint32_t new_namelen = name_l + strlen(c_prefix);
        // 写一个 newlen
        bin_c.insert(bin_c.end(), reinterpret_cast<uint8_t *>(&new_namelen),
                     reinterpret_cast<uint8_t *>(&new_namelen) + 4);
        // 向 bin_c 写一个 newname
        bin_c.insert(bin_c.end(), c_prefix, c_prefix + strlen(c_prefix));
        bin_c.insert(bin_c.end(), name, name + strlen(name));

        while ((len = zip_fread(bin, buffer, 4096)) > 0) {
            bin_c.insert(bin_c.end(), buffer, buffer + len);
        }

        struct zip_source *source = zip_source_buffer(za, reinterpret_cast<const void *>(bin_c.data()), bin_c.size(), 0);

        // 创建要添加的文件
        if (!source) {
            std::cerr << "error creating source buffer: " << zip_strerror(za) << std::endl;
            zip_close(za);
            return;
        }

        // 添加文件到 ZIP 文件中
        zip_fclose(bin);
        auto ai = zip_file_add(za, bin_name, source, ZIP_FL_OVERWRITE);
        if (ai < 0) {
            std::cerr << "error adding file to ZIP: " << zip_strerror(za) << std::endl;
            zip_source_free(source);
            zip_close(za);
            return;
        }

        // 关闭 ZIP 文件
        if (zip_close(za) < 0) {
            std::cerr << "error closing ZIP file: " << zip_strerror(za) << std::endl;
            return;
        }

        LOG(info) << "File " << c_prefix << name  << " has been update to ZIP file " << std::endl;

        delete[] header;
        delete[] name;
        delete[] buffer;

    }


};

class XmlAccessor : public LAccessor {

public:
    std::string prefix;

    explicit XmlAccessor(const fs::path &folder, const std::string &prefix) : \
    LAccessor(folder / "data", "images", folder / "data\\databundles\\images.zip") {
        this->prefix = prefix;
    }

    void processIfNecessary(const fs::path &dest) override {
        LAccessor::processIfNecessary(dest);
        regex re("(filename|name)=\"(.*?)\"");
        // 给 name filename 加前缀
        std::ifstream ifs(dest);
        std::vector<char> buffer(std::istreambuf_iterator<char>(ifs), {});
        std::string rs = regex_replace(std::string(buffer.begin(), buffer.end()), re, "$1=\"" + prefix + "$2\"", regex_constants::format_all);
        ifs.close();
        std::ofstream ofs(dest);
        ofs << rs;
    }
};

class BigportraitsAccessor : public XmlAccessor {

public :
    BigportraitsAccessor(const fs::path &folder, const std::string &prefix) : \
    XmlAccessor(folder, prefix) {
        this->folder = folder / "data";
        this->path_prefix = "bigportraits";
        auto &&zip = folder / "data\\databundles\\bigportraits.zip";
        this->zipfile = zip_open(zip.string().c_str(), ZIP_RDONLY, nullptr);
    }

};


class LExtractor {

public:

    explicit LExtractor(const fs::path &game_path, const std::string &prefix = "") {
        this->game_path = game_path;
        this->prefix = prefix;
    }

    [[maybe_unused]] AnimAccessor animAccessor() {
        return AnimAccessor(this->game_path, prefix);
    }

    BigportraitsAccessor bigportraitsAccessor() {
        return {BigportraitsAccessor(this->game_path, prefix)};
    }

    LAccessor scriptAccessor() {
        return {LAccessor(this->game_path / "data", \
        "scripts/", \
        this->game_path / "data\\databundles\\scripts.zip")};
    }

    XmlAccessor xmlAccessor() {
        return XmlAccessor(this->game_path, prefix);
    }


private:
    fs::path game_path;
    std::string prefix;
    fs::path dest;
};


#endif //SKINS_GENERATOR_LEXTRACTOR_H
