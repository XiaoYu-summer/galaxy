#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>

#include "Constant.h"
#include "apiControllers/PassportApiController.h"
#include "utils/PassportUtils.h"
#include "utils/ResUtils.h"

void PassportApiController::InitRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/passport/api/v1/token")
        .methods("POST"_method)([&app](const crow::request& request, crow::response& response) {
            auto requestBody = crow::json::load(request.body);
            if (!requestBody) {
                return FailResponse(response, ErrorCode::PASSWORD_ERROR, "Invalid request body");
            }

            if (!requestBody.has("account") || !requestBody.has("password")) {
                return FailResponse(response, ErrorCode::PARAMS_ERROR, "Invalid parameters");
            }

            std::string accountName = requestBody["account"].s();
            std::string password = requestBody["password"].s();
            std::string accountFilePath = PassportUtils::GetAccountPasswordFilePath().string();

            std::ifstream inFile(accountFilePath);
            if (!inFile) {
                PassportUtils::InitAccountPasswordFile();
                return FailResponse(response, ErrorCode::AUTH_ERROR, "Server Passport not found");
            }

            std::string fileContent((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
            inFile.close();

            crow::json::rvalue accountPassword = crow::json::load(fileContent);
            if (accountName != accountPassword["account"].s() || password != accountPassword["password"].s()) {
                return FailResponse(response, ErrorCode::PASSWORD_ERROR, "Authorization failed");
            }

            auto& session = app.get_context<Session>(request);
            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            std::string token = boost::uuids::to_string(uuid);

            session.set(TOKEN_KEY, token);
            auto expireTime = std::chrono::system_clock::now() + std::chrono::hours(1);
            session.set(TOKEN_EXPIRE_KEY, std::chrono::system_clock::to_time_t(expireTime));

            crow::json::wvalue responseData({{"accessToken", token}});
            return SuccessResponse(response, "Gain token success", responseData);
        });

    CROW_ROUTE(app, "/passport/api/v1/password")
        .methods("PUT"_method)([&app](const crow::request& request, crow::response& response) {
            auto requestBody = crow::json::load(request.body);
            if (!requestBody) {
                return FailResponse(response, ErrorCode::PARAMS_ERROR, "Invalid request body");
            }

            std::string oldPassword = requestBody["oldPassword"].s();
            std::string newPassword = requestBody["newPassword"].s();
            auto [accountName, password] = PassportUtils::ReadAccountPasswordFromFile();

            if (oldPassword != password) {
                return FailResponse(response, ErrorCode::OLD_PASSWORD_ERROR, "Old password is incorrect");
            }

            PassportUtils::WriteAccountPasswordToFile(accountName, newPassword);
            return SuccessResponse(response, "Password changed successfully");
        });

    CROW_ROUTE(app, "/passport/api/v1/token")
        .methods("DELETE"_method)([&app](const crow::request& request, crow::response& response) {
            auto& session = app.get_context<Session>(request);
            session.remove("token");
            session.remove("expireTime");
            return SuccessResponse(response, "Logged out successfully");
        });
}