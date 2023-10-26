//
// Created by zhnkc9 on 2023/9/26.
//

#ifndef SKINS_GENERATOR_UTIL_H
#define SKINS_GENERATOR_UTIL_H

#include <boost/regex.hpp>

class util {

public:

    static std::string removeSurroundingChars(const std::string &input, const std::string &chars);

    static void removeSurroundingCharsRef(std::string &input, const std::string &chars);

    static void processFile(std::ifstream &ifs, int buffer_size, int c_size, const std::function<int(std::string &)> &handler);

    static void processFileWithRegexIt(std::ifstream &ifs, int buffer_size, int c_size, boost::regex &re,
                                       const std::function<bool(boost::sregex_iterator &, boost::sregex_iterator &, boost::smatch &)> &handler);

    static void processFileWithLine(std::ifstream &ifs, int buffer_size, int c_size, const std::function<void(std::string &)> &handler);

    static bool strDistanceChecker(std::string &s1, std::string &s2, const std::vector<std::string> &dict, int ops_count, int max_ops);

    static std::string stripSuffix(const std::string &str, std::vector<std::string> &suffixes);

    static bool isFile(char *path);

    static bool isFolder(char *path);

    static int unzip(const std::string &archive, const std::string &dest_s);

};

#endif
