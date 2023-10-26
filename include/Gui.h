//
// GUI API
// Created by zhnkc9 on 2023/9/27.
//
extern "C" {

[[maybe_unused]] typedef void (_stdcall * LogCallBack ) ( int , const char *msg ) ;

[[maybe_unused]] __declspec(dllexport)  void redirect_stdout(const char *path);

[[maybe_unused]] __declspec(dllexport)  char *skins_data(const char *path);

[[maybe_unused]] __declspec(dllexport)  char *skin_i8n(const char *path);

[[maybe_unused]] __declspec(dllexport) void init_log(const char *path);

[[maybe_unused]] __declspec(dllexport) void register_log_callback(LogCallBack callback);

[[maybe_unused]] __declspec(dllexport) void test_log();

[[maybe_unused]] __declspec(dllexport) void generate(const char *game_path);

}
