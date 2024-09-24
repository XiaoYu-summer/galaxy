#pragma once
#include <crow.h>

#include "code/ErrorCode.h"
#include "utils/ResUtils.h"
struct ReqLoggerMiddleware {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        if (!req.get_header_value("x-apm-traceid").empty()) {
            CROW_LOG_INFO << "Request: " << method_name(req.method) << " " << req.url << " "
                          << req.get_header_value("x-apm-traceid");
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};