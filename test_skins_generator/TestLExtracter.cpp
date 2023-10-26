////
//// Created by zhnkc9 on 2023/10/10.
////
//
#include <gtest/gtest.h>
#include "common_headers.hpp"
#include "LExtractor.h"

TEST(TEST_LEXTRACTER, test_anim_rebuild) {
    string game_path = R"(E:\game\Steam\steamapps\common\Don't Starve Together)";
    string prefix = "zhnkc9@gmail.com_";
    auto extracter = LExtractor(game_path, prefix);
    auto anim = extracter.animAccessor();
    anim.transfer("abigail_formal.dyn", "d:/abigail_flower_formal.dyn");
    anim.transfer("abigail_formal.zip", "d:/abigail_flower_formal.zip", true);
}
//x
//TEST(TEST_LEXTRACTER, test_open_zip) {
//    string zip = R"(E:\game\Steam\steamapps\common\Don't Starve Together/data/databundles/anim_dynamic.zip)";
//    auto zipfile = zip_open(zip.c_str(), ZIP_RDONLY, nullptr);
//    struct zip_file *file = zip_fopen(zipfile, "anim/dynamic/abigail_formal.zip", ZIP_FL_UNCHANGED);
//    std::cout << (file != nullptr) << std::endl;
//}
//
TEST(TEST_LEXTRACTER, test_rebuild_xml) {
    string game_path = R"(E:\game\Steam\steamapps\common\Don't Starve Together)";
    string prefix = "zhnkc9@gmail.com_";
    auto extracter = LExtractor(game_path, prefix);
    auto xml = extracter.bigportraitsAccessor();
    xml.transfer("wx78_lunar.xml",
                 R"(D:\C++\skin-generator\cmake-build-release\test_skins_generator\tmp\bigportraits\zhnkc9@gmail.com_wx78_lunar.xml)", true);
}


TEST(TEST_LEXTRACTER, test_rebuild_xml_custom) {
    string prefix = "";
    regex re("(filename|name)=\"(.*?)\"");
    // ¸ø name filename ¼ÓÇ°×º
    std::ifstream ifs(R"(D:\C++\skin-generator\cmake-build-release\test_skins_generator\tmp\bigportraits\zhnkc9@gmail.com_wx78_lunar.xml)");
    std::vector<char> buffer(std::istreambuf_iterator<char>(ifs), {});
    std::string c(buffer.begin(), buffer.end());
    std::string rs = regex_replace(c, re, "$1", regex_constants::format_all);
    ifs.close();
    std::ofstream ofs(R"(D:\C++\skin-generator\cmake-build-release\test_skins_generator\tmp\bigportraits\zhnkc9@gmail.com_wx78_lunar.xml)");
    cout << rs;
    ofs << rs;
}


