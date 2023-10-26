////
//// Created by zhnkc9 on 2023/10/18.
////
//
#include <gtest/gtest.h>
#include "common_headers.hpp"
#include "LParser.hpp"
#include "LBuilder.hpp"
#include "LExtractor.h"
#include "Gui.h"
#include "util.h"

#define ModName "Local Collection Skins"
namespace fs = std::filesystem;
TEST(F, F) {
    // debug
    boost::log::core::get()->
            set_filter(boost::log::trivial::severity
                       >= boost::log::trivial::debug);
    const char *game_path = "E:\\game\\Steam\\steamapps\\common\\Don't Starve Together";
    generate(game_path);

    auto modPath = fs::path(game_path, ModName);
}
