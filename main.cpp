#define CROW_STATIC_DIRECTORY "assets/"
#define CROW_STATIC_ENDPOINT "/assets/<path>"
#include <crow.h>

#include "Routes.h"
#include "utils/LogUtils.h"

int main() {
    crow::SimpleApp app;
    // 设置日志级别
    crow::logger::setLogLevel(crow::LogLevel::INFO);
    FileLogHandler logger;
    crow::logger::setHandler(&logger);

    // 设置路由
    SetupRoutes(app);

    // 启动服务器
    app.port(80).multithreaded().run();

    return 0;
}
