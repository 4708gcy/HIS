/**
 * @file server_main.cpp
 * @brief HIS REST API 服务器入口
 * @details 启动 HTTP 服务器，加载所有数据，注册 API 路由
 */

#include <iostream>
#include <signal.h>
#include "ApiServer.h"

#ifdef _WIN32
#include <windows.h>
#endif

static httplib::Server *g_server = nullptr;

void signalHandler(int signum)
{
    std::cout << "\n[INFO] 收到退出信号，正在保存数据并关闭服务器..." << std::endl;
    if (g_server)
    {
        DataManager::getInstance().saveAll();
        g_server->stop();
    }
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::cout << "=== HIS REST API 服务器 ===" << std::endl;
    std::cout << "[INFO] 正在加载数据..." << std::endl;

    DataManager::getInstance().init();
    std::cout << "[INFO] 数据加载完成" << std::endl;

    httplib::Server svr;
    g_server = &svr;

    // 注册所有 API 路由
    registerApiRoutes(svr);

    // 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    int port = 8080;
    std::cout << "[INFO] 服务器启动于 http://localhost:" << port << std::endl;
    std::cout << "[INFO] API 文档: http://localhost:" << port << "/api/departments" << std::endl;
    std::cout << "[INFO] 按 Ctrl+C 退出" << std::endl;

    if (!svr.listen("0.0.0.0", port))
    {
        std::cerr << "[ERROR] 无法启动服务器，端口 " << port << " 可能被占用" << std::endl;
        return 1;
    }

    // 服务器停止后保存数据
    DataManager::getInstance().saveAll();
    std::cout << "[INFO] 服务器已关闭，数据已保存" << std::endl;

    return 0;
}
