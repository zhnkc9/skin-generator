//
// Created by zhnkc9 on 2023/9/29.
//

// common_headers.h

#ifndef COMMON_HEADERS_H
#define COMMON_HEADERS_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <boost/regex.hpp>
#include "util.h"
#include <fstream>
#include "json.hpp"
// LOG
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

using std::vector;
using std::string;
using std::map;
using std::cout;
using std::endl;
using std::function;
using std::string_view;
using std::unordered_map;
using namespace boost;
using nlohmann::json;
namespace logging = boost::log;

class InitLogger {

public:
    static InitLogger &getInstance() {
        static InitLogger instance;
        return instance;
    }
private:
    InitLogger() {
        // 添加控制台日志

        boost::log::add_console_log(
                std::cout,
                boost::log::keywords::format = "[%TimeStamp%] [%Severity%] %Message%"
        );

        logging::add_file_log("skins_generator.log");

        boost::log::core::get()->
                set_filter(boost::log::trivial::severity
                           >= boost::log::trivial::info);
        boost::log::add_common_attributes();
    }
};

static InitLogger& initLogger = InitLogger::getInstance();

#define LOG(lvl) BOOST_LOG_TRIVIAL(lvl)

#endif // COMMON_HEADERS_H

