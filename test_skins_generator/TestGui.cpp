//
// Created by zhnkc9 on 2023/10/11.
//
#include <gtest/gtest.h>
#include "common_headers.hpp"
#include "Gui.h"

TEST(TEST_GUI, test_skins_data) {
    const char *path = "E:\\game\\Steam\\steamapps\\common\\Don't Starve Together\0";
    auto string = skins_data(path);
//    cout << string << endl;
}

TEST(TEST_GUI, test_i8n) {
    const char *path = "E:\\game\\Steam\\steamapps\\common\\Don't Starve Together\0";
    auto string = skin_i8n(path);
    std::ofstream ofs("d:/2.txt");
    ofs << string;
    cout << string << endl;
}
