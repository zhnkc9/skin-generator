////
//// Created by zhnkc9 on 2023/10/19.
////
//
//#include <gtest/gtest.h>
//#include "zip.h"
//#include "zlib.h"
//#include "common_headers.hpp"
//#include <sys/stat.h>
//
//
//// 递归创建目录
//void create_directory(const std::string& path) {
//    size_t sep = path.find_last_of('/');
//    if (sep != std::string::npos) {
//        std::string parent_path = path.substr(0, sep);
//        create_directory(parent_path);
//    }
//    mkdir(path.c_str());
//}
//
//
//TEST(UNZIP, test_unzip) {
//
//    const char* archive_path = "d:/source.zip";
//    const char* destination_directory = "d:/destination/";
//
//    // 打开ZIP文件
//    int error;
//    zip_t* archive = zip_open(archive_path, 0, &error);
//    if (!archive) {
//        std::cerr << "Failed to open ZIP archive: " << zip_strerror(archive) << std::endl;
//    }
//
//    // 创建目标文件夹
//    create_directory(destination_directory);
//
//    // 递归解压ZIP文件
//    for (int i = 0; i < zip_get_num_entries(archive, 0); i++) {
//        extract_folder(archive, i, destination_directory);
//    }
//
//    // 关闭ZIP文件
//    zip_close(archive);
//
//    std::cout << "ZIP archive extracted to: " << destination_directory << std::endl;
//
//}
//
//
//// 递归解压文件和文件夹
//void extract_folder(zip_t* archive, const char* folder_name, const char* destination_directory) {
//    int num_entries = zip_get_num_entries(archive, 0);
//
//    for (int i = 0; i < num_entries; i++) {
//        const char* entry_name = zip_get_name(zip_get_index(archive, i, 0), 0);
//
//        // 检查文件或文件夹是否在指定的文件夹中
//        if (strstr(entry_name, folder_name) == entry_name) {
//            struct zip_stat file_info;
//            zip_stat_init(&file_info);
//            zip_stat_index(archive, i, 0, &file_info);
//
//            // 构建目标路径
//            char extracted_path[256];
//            strcpy(extracted_path, destination_directory);
//            strcat(extracted_path, file_info.name);
//
//            // 如果是目录，创建目录并继续递归解压
//            if (file_info.name[strlen(file_info.name) - 1] == '/') {
//                create_directory(extracted_path);
//                extract_folder(archive, file_info.name, destination_directory);
//            } else {
//                // 解压文件
//                zip_file_t* file = zip_fopen_index(archive, i, 0);
//                if (!file) {
//                    std::cerr << "Failed to open file in ZIP archive: " << zip_strerror(archive) << std::endl;
//                    continue;
//                }
//
//                FILE* extracted_file = fopen(extracted_path, "wb");
//                if (!extracted_file) {
//                    std::cerr << "Failed to create extracted file: " << extracted_path << std::endl;
//                    zip_fclose(file);
//                    continue;
//                }
//
//                char buffer[4096];
//                int len;
//                while ((len = zip_fread(file, buffer, sizeof(buffer)) > 0)) {
//                    fwrite(buffer, 1, len, extracted_file);
//                }
//
//                fclose(extracted_file);
//                zip_fclose(file);
//            }
//        }
//    }
//}
//
//
//int main() {
//    const char* archive_path = "your_zip_file.zip";
//    const char* destination_directory = "destination/";
//
//    // 打开ZIP文件
//    int error;
//    zip_t* archive = zip_open(archive_path, 0, &error);
//    if (!archive) {
//        std::cerr << "Failed to open ZIP archive: " << zip_strerror(archive) << std::endl;
//        return 1;
//    }
//
//    // 创建目标文件夹
//    create_directory(destination_directory);
//
//    // 递归解压ZIP文件
//    int num_entries = zip_get_num_entries(archive, 0);
//    for (int i = 0; i < num_entries; i++) {
//        extract_entry(archive, i, destination_directory);
//    }
//
//    // 关闭ZIP文件
//    zip_close(archive);
//
//    std::cout << "ZIP archive extracted to: " << destination_directory << std::endl;
//    return 0;
//}
