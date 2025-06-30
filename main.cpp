#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

void createNestedDirectories(const fs::path& base_path, int current_depth, int max_depth) {
    if (current_depth > max_depth) {
        return;
    }

    // 创建当前层的10个目录
    for (int i = 1; i <= 9; ++i) {
        fs::path dir_path = base_path / std::to_string(i);

        try {
            if (!fs::exists(dir_path)) {
                if (fs::create_directory(dir_path)) {
                    std::cout << "创建目录: " << dir_path << std::endl;
                } else {
                    std::cerr << "无法创建目录: " << dir_path << std::endl;
                    continue;
                }
            }

            // 递归创建下一层
            createNestedDirectories(dir_path, current_depth + 1, max_depth);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "错误: " << e.what() << std::endl;
        }
    }
}

int start() {
    const fs::path base_dir = R"(C:\Users\Galaxy\workspace\test)";  // 基础目录名
    const int max_depth = 3;                  // 最大深度6层

    try {
        // 创建基础目录
        if (!fs::exists(base_dir)) {
            fs::create_directory(base_dir);
        }

        std::cout << "开始创建目录结构..." << std::endl;
        createNestedDirectories(base_dir, 1, max_depth);
        std::cout << "目录结构创建完成！" << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "主函数错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    // TIP Press <shortcut actionId="RenameElement"/> when your caret is at the <b>lang</b> variable name to see how CLion can help you rename it.
    auto lang = "C++";
    std::cout << "Hello and welcome to " << lang << "!\n";

    for (int i = 1; i <= 5; i++) {
        // TIP Press <shortcut actionId="Debug"/> to start debugging your code. We have set one <icon src="AllIcons.Debugger.Db_set_breakpoint"/> breakpoint for you, but you can always add more by pressing <shortcut actionId="ToggleLineBreakpoint"/>.
        std::cout << "i = " << i << std::endl;
    }

    // start();

    return 0;
    // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.
}