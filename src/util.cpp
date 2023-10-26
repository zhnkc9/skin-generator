//
// Created by zhnkc9 on 2023/9/26.
//
#include <string>
#include <regex>
#include <fstream>
#include <iostream>
#include "util.h"
#include <boost/regex.hpp>
#include <Windows.h>
#include <sstream>
#include "zip.h"
#include <filesystem>
#include "common_headers.hpp"

#define USE_BOOST

#ifdef USE_BOOST
using namespace boost;
#else
using namespace std;
#endif

using std::map;
using std::string;


std::string util::removeSurroundingChars(const std::string &input, const std::string &chars) {
    std::string result = input;
    while (!result.empty() && chars.find(result.front()) != std::string::npos) {
        result.erase(0, 1);
    }
    while (!result.empty() && chars.find(result.back()) != std::string::npos) {
        result.pop_back();
    }
    return result;
}

// 写一个直接指针版本的
void util::removeSurroundingCharsRef(std::string &input, const std::string &chars) {

    while (!input.empty() && chars.find(input.front()) != std::string::npos) {
        input.erase(0, 1);
    }
    while (!input.empty() && chars.find(input.back()) != std::string::npos) {
        input.pop_back();
    }
}

/*
 * 实现动态调整buffer
 * 将没处理完的数据给下一跳处理
 * 可以预留上次未处理的长度
 * handler 返回的 last_index 最好不超过 c_size,这样每次读取可以读取一整个的buffer_size
 */
void util::processFile(std::ifstream &ifs, int buffer_size, int c_size, const std::function<int(string &)> &handler) {

    if (!ifs.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }
    int max_size = buffer_size + c_size;
    int offset = 0;
    size_t limit;
    map<string, string> result;
    bool need_copy;
    std::vector<char> buffer(max_size);
    auto data = buffer.data();
    while (true) {
        // 如果 offset + buffer_size > max_size ，那就减 buffer_size
        int ryb = offset + buffer_size > max_size ? max_size - offset : buffer_size;
        int gcount = (int) ifs.read(data + offset, ryb).gcount();

        if (gcount == 0 and (ifs.eof() or ifs.bad()))
            break;

        // 待处理的字符串序列
        std::string str(buffer.begin(), buffer.begin() + (int) offset + gcount);

        // 我应该把字符串给他，并拿到最后的pos
        // 返回0 不复制不处理
        offset = handler(str);

        // 匹配到了，我给最后匹配到的pos
        // 如果没匹配到，我是不是应该给一个默认的 偏移长度。
        // 如果我给默认的偏移长度，这个参数应该给200，也就是lastindex = 200
        if ((need_copy = offset > 0)) {
            int str_len = (int) str.length();
            // offset 可能会非常大, 这里拿到了起始位置
            offset = str_len > offset ? offset : str_len;
            // 断言，offset不能和maxsize相等，否则无法进行组合
            assert(offset != max_size);
            // 数据copy的时候，应该跳过的数据量，应该和offset相对应
            limit = str_len - offset;
            need_copy = offset > 0 && limit > 0;
        }
        if (need_copy)
            // 这处理一下offset
            // 断言
            std::memcpy(data, data + limit, offset);
    }
}

/*
 * 整个文件处理
 */
void util::processFileWithRegexIt(std::ifstream &ifs, int buffer_size, int c_size, boost::regex &re,
                                  const std::function<bool(boost::sregex_iterator &, boost::sregex_iterator &, boost::smatch &)> &handler) {
    smatch match;
    smatch clear_match;
    sregex_iterator end;
    auto f = std::function < size_t(string & ) > {
            [&re, &match, &end, &handler, &c_size](string &str) -> int {

                sregex_iterator it(str.begin(), str.end(), re);

                auto b = handler(it, end, match);

                auto len = b ? (int) match.suffix().length() : c_size;

                return b ? len > c_size ? c_size : len : c_size;

            }
    };
    return util::processFile(ifs, buffer_size, c_size, f);

}


/*
 * 整个文件处理
 */
void util::processFileWithLine(std::ifstream &ifs, int buffer_size, int c_size, const std::function<void(std::string &)> &handler) {
    std::stringstream iss;
    string line;
    auto f = std::function < size_t(string & ) > {
            [&iss, &line, &handler](string &str) -> int {
                iss.clear();
                iss.str(str);
                while (std::getline(iss, line)) {
                    // 不处理最后一行
                    if (iss.eof()) {
                        return (int) line.size();
                    }
                    handler(line);
                }
                return 0;
            }
    };
    return util::processFile(ifs, buffer_size, c_size, f);
}

bool util::strDistanceChecker(string &s1, string &s2, const std::vector<string> &dict, int ops_count, int max_ops) {
    // 如果编辑操作次数超过了最大限制，返回False
    if (ops_count > max_ops) {
        return false;
    }

    // 如果两个字符串相等，返回True
    if (s1 == s2) {
        return true;
    }

    // 尝试删除、插入和替换操作
    for (const string &op: dict) {

        // 先判断length
        if (s1.length() + op.length() != s2.length()) {
            continue;
        }
        // 尝试插入操作
        for (size_t i = 0; i <= s1.size(); ++i) {
            string new_s1 = s1.substr(0, i) + op + s1.substr(i);
            if (strDistanceChecker(new_s1, s2, dict, ops_count + 1, max_ops)) {
                return true;
            }
        }
    }
    return false;
}


//string endsWith(const std::string &str, const std::string &suffix) {
string util::stripSuffix(const std::string &str, std::vector<string> &suffixes) {
    size_t strLength = str.length();
    for (const auto &suffix: suffixes) {
        size_t suffixLength = suffix.length();
        // 确保字符串长度大于等于后缀长度
        if (strLength < suffixLength) {
            return str;
        }
        // 比较字符串的末尾是否与后缀匹配
        if (str.compare(strLength - suffixLength, suffixLength, suffix) == 0) {
            // 字符串截取返回
            return str.substr(0, strLength - suffixLength);
        }
    }
    return str;
}


bool util::isFile(char *path) {
    DWORD f = GetFileAttributes(path);
    return f != INVALID_FILE_ATTRIBUTES && !(f & FILE_ATTRIBUTE_DIRECTORY);
}

bool util::isFolder(char *path) {
    DWORD f = GetFileAttributes(path);
    return f != INVALID_FILE_ATTRIBUTES && (f & FILE_ATTRIBUTE_DIRECTORY);
}

int util::unzip(const std::string &archive, const std::string &dest_s) {
    struct zip *za;
    struct zip_file *zf;
    struct zip_stat sb{};
    int err;
    size_t i, len;
    constexpr size_t size = 4096;
    size_t sum;
    char buf[size];
    std::filesystem::path dest(dest_s);
    if ((za = zip_open(archive.c_str(), 0, &err)) == nullptr) {
        zip_error_to_str(buf, sizeof(buf), err, errno);
        LOG(error) << archive << ": can't open zip archive `" << buf << "': " << strerror(errno) << "\n";
        return 1;
    }
    for (i = 0; i < zip_get_num_entries(za, 0); i++) {
        if (zip_stat_index(za, i, 0, &sb) == 0) {
            len = strlen(sb.name);
            std::string &&dest_file = (dest / sb.name).string();
            LOG(debug) << "Name: [" << dest_file << "], ";
            LOG(debug) << "Size: [" << sb.size << "], ";
            LOG(debug) << "mtime: [" << static_cast<unsigned int>(sb.mtime) << "]\n";
            if (sb.name[len - 1] == '/') {
                std::filesystem::create_directories(dest_file.c_str());
            } else {
                zf = zip_fopen_index(za, i, 0);
                if (!zf) {
                    LOG(error) << "unzip: open archive file fail. " << archive << " : " << dest_file << std::endl;
                    return 1;
                }

                std::ofstream os(dest_file, std::ios::binary);
                if (!os.is_open()) {
                    LOG(error) << "unzip: write zipfile fail. " << archive << " : " << dest_file << std::endl;
                    return 1;
                }
                sum = 0;
                while (sum != sb.size) {
                    len = zip_fread(zf, buf, size);
                    os.write(buf, static_cast<std::streamsize>(len));
                    sum += len;
                }
                zip_fclose(zf);
            }
        } else {
            LOG(error) << "unzip: can open zipfile. " << archive << std::endl;
            LOG(error) << "File[" << __FILE__ << "] Line[" << __LINE__ << "]\n";
            return 1;
        }
    }
    zip_close(za);
    return 0;
}

