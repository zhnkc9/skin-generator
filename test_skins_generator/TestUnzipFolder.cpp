////
//// Created by zhnkc9 on 2023/10/19.
////
//#include <cstring>
//#include "zip.h"
//#include <iostream>
//#include <fstream>
//#include <filesystem>
//
//int unzip(const std::string &archive, const std::string &dest_s) {
//    struct zip *za;
//    struct zip_file *zf;
//    struct zip_stat sb{};
//    int err;
//    size_t i, len;
//    constexpr size_t size = 4096;
//    size_t sum;
//    char buf[size];
//    std::filesystem::path dest(dest_s);
//    if ((za = zip_open(archive.c_str(), 0, &err)) == nullptr) {
//        zip_error_to_str(buf, sizeof(buf), err, errno);
//        std::cerr << archive << ": can't open zip archive `" << buf << "': " << strerror(errno) << "\n";
//        return 1;
//    }
//    for (i = 0; i < zip_get_num_entries(za, 0); i++) {
//        if (zip_stat_index(za, i, 0, &sb) == 0) {
//            len = strlen(sb.name);
//            std::string &&dest_file = (dest / sb.name).string();
////            std::cout << "Name: [" << dest_file << "], ";
////            std::cout << "Size: [" << sb.size << "], ";
////            std::cout << "mtime: [" << static_cast<unsigned int>(sb.mtime) << "]\n";
//            if (sb.name[len - 1] == '/') {
//                std::filesystem::create_directories(dest_file.c_str());
//            } else {
//                zf = zip_fopen_index(za, i, 0);
//                if (!zf) {
//                    std::cerr << "unzip: open archive file fail. " << archive << " : " << dest_file << std::endl;
//                    return 1;
//                }
//
//                std::ofstream os(dest_file, std::ios::binary);
//                if (!os.is_open()) {
//                    std::cerr << "unzip: write zipfile fail. " << archive << " : " << dest_file << std::endl;
//                    return 1;
//                }
//                sum = 0;
//                while (sum != sb.size) {
//                    len = zip_fread(zf, buf, size);
//                    os.write(buf, static_cast<std::streamsize>(len));
//                    sum += len;
//                }
//                zip_fclose(zf);
//            }
//        } else {
//            std::cerr << "unzip: can open zipfile. " << archive << std::endl;
//            std::cout << "File[" << __FILE__ << "] Line[" << __LINE__ << "]\n";
//            return 1;
//        }
//    }
//    zip_close(za);
//    return 0;
//}
//
//
//int main() {
//    std::string archive = "D:\\source.zip";
//    std::string dest = "d:/tttt/ffff/";
//    unzip(archive, dest);
//}
