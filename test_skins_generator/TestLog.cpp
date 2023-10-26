////
//// Created by zhnkc9 on 2023/10/15.
////
//
//#include "gtest/gtest.h"
//#include <boost/log/core.hpp>
//#include <boost/log/trivial.hpp>
//#include <boost/log/expressions.hpp>
//#include <boost/log/sinks/text_file_backend.hpp>
//#include <boost/log/utility/setup/common_attributes.hpp>
//#include <boost/log/utility/setup/console.hpp>
//#include <boost/log/utility/setup/file.hpp>
//#include <fstream>
//#include <iostream>
//#include <boost/log/expressions.hpp>
//#include <boost/log/sinks/sync_frontend.hpp>
//#include <boost/log/sinks/text_file_backend.hpp>
//#include <boost/log/sources/severity_logger.hpp>
//#include <boost/log/sources/record_ostream.hpp>
//
//namespace logging = boost::log;
//namespace sinks = boost::log::sinks;
//namespace src = boost::log::sources;
//namespace expr = boost::log::expressions;
//
//class MyCustomSink : public sinks::basic_formatted_sink_backend<char> {
//public:
//
//    // 将日志消息写入自定义目标的操作
//    static void consume(logging::record_view const &rec, const std::string &message) {
//
//        // 在这里实现将消息写入你的自定义目标的逻辑
//        // 例如，写入文件或者发送到网络
//        // 这里只是简单地将消息打印到标准输出
//        std::cout << "Custom Sink: " << message << std::endl;
//    }
//};
//
//TEST(testmain, ttt) {
//    sinks::synchronous_sink<MyCustomSink> f;
//    // 添加自定义 sink 到 Boost.Log
//    boost::shared_ptr<sinks::synchronous_sink<MyCustomSink>> sink =
//            boost::make_shared<sinks::synchronous_sink<MyCustomSink>>(boost::make_shared<MyCustomSink>());
//
//    logging::core::get()->add_sink(sink);
//
//    std::cout << static_cast<logging::trivial::severity_level>(0) << std::endl;
//    std::cout << static_cast<logging::trivial::severity_level>(1) << std::endl;
//    std::cout << static_cast<logging::trivial::severity_level>(2) << std::endl;
//    std::cout << static_cast<logging::trivial::severity_level>(3) << std::endl;
//    std::cout << static_cast<logging::trivial::severity_level>(4) << std::endl;
//    std::cout << static_cast<logging::trivial::severity_level>(5) << std::endl;
//    std::cout << static_cast<logging::trivial::severity_level>(6) << std::endl;
//
//}
//
//
//void init_logging() {
//    // 创建一个文本输出流
//    boost::shared_ptr<std::ostream> console_stream(&std::clog);
//    boost::shared_ptr<std::ostream> file_stream(new std::ofstream("sample.log"));
//
//    // 创建 text_ostream_backend
//    typedef logging::sinks::text_ostream_backend backend_t;
//    boost::shared_ptr<backend_t> console_backend = boost::make_shared<backend_t>();
//    boost::shared_ptr<backend_t> file_backend = boost::make_shared<backend_t>();
//
//    // 将输出流添加到 backend
//    console_backend->add_stream(console_stream);
//    file_backend->add_stream(file_stream);
//
//    // 创建前端 sink
//    typedef logging::sinks::synchronous_sink<backend_t> sink_t;
//    boost::shared_ptr<sink_t> console_sink = boost::make_shared<sink_t>(console_backend);
//    boost::shared_ptr<sink_t> file_sink = boost::make_shared<sink_t>(file_backend);
//
//    // 注册 sink 到 logging core
//    logging::core::get()->add_sink(console_sink);
//    logging::core::get()->add_sink(file_sink);
//
//    // 设置日志过滤级别
//    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
//
//}
