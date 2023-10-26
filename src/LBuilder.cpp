//
// Created by zhnkc9 on 2023/10/7.
//

#include "LBuilder.hpp"

//
std::map<AssetType, std::string> extMap = {
        {DYNAMIC_ANIM, ".zip"},
        {PKGREF,       ".dyn"},
        {IMAGE,        ".tex"},
        {ATLAS,        ".xml"},
        {ATLAS_BUILD,        ".xml"},
        {DYNAMIC_ATLAS,        ".xml"},
};

std::map<ResourceType, std::string> pathMap = {
        {ITEM,         "anim/dynamic/"},
        {BIGPORTRAITS, "bigportraits/"},
        {BG_LOADING,   "images/"},
        {IMAGES,       "images/"},
};

std::map<AssetType, std::string> asset2str = {
        {AssetType::DYNAMIC_ANIM,  "DYNAMIC_ANIM"},
        {AssetType::PKGREF,        "PKGREF"},
        {AssetType::IMAGE,         "IMAGE"},
        {AssetType::ATLAS,         "ATLAS"},
        {AssetType::ATLAS_BUILD,   "ATLAS_BUILD"},
        {AssetType::DYNAMIC_ATLAS, "DYNAMIC_ATLAS"},
};
