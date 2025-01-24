#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>

#include "Constant.h"
#include "apiControllers/PassportApiController.h"
#include "utils/PassportUtils.h"
#include "utils/ResUtils.h"

void PassportApiController::InitRoutes(CrowApp& app) {
    InitAdminRoutes(app);
    InitClientRoutes(app);

    CROW_ROUTE(app, "/passport/api/v1/generate-pairing-code")
        .methods("POST"_method)([&app](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{"pairingCode", "pairingCode"}});
            return SuccessResponse(response, "Generate pairing code success", responseData);
        });
}

void PassportApiController::InitAdminRoutes(CrowApp& app){
    CROW_ROUTE(app, "/passport/api/v1/admin/token/generate")
        .methods("POST"_method)([&app](const crow::request& request, crow::response& response) {
            HandleJsonReqWithParams<std::string, std::string>(request, response,
                [&app, &request](const std::string& accountName, const std::string& password,crow::response& response) {
                    const std::string accountFilePath = PassportUtils::GetAccountPasswordFilePath().string();

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
                }, "account", "password");
        });

    CROW_ROUTE(app, "/passport/api/v1/admin/token/change")
        .methods("PUT"_method)([&app](const crow::request& request, crow::response& response) {
            HandleJsonReqWithParams<std::string, std::string>(request, response,
                [&app, &request](const std::string& oldPassword, const std::string& newPassword,crow::response& response){
                    auto [accountName, password] = PassportUtils::ReadAccountPasswordFromFile();

                    if (oldPassword != password) {
                        return FailResponse(response, ErrorCode::OLD_PASSWORD_ERROR, "Old password is incorrect");
                    }

                    PassportUtils::WriteAccountPasswordToFile(accountName, newPassword);
                    return SuccessResponse(response, "Password changed successfully");
                }, "oldPassword", "newPassword");
        });

    CROW_ROUTE(app, "/passport/api/v1/admin/token")
        .methods("DELETE"_method)([&app](const crow::request& request, crow::response& response) {
            auto& session = app.get_context<Session>(request);
            session.remove("token");
            session.remove("expireTime");
            return SuccessResponse(response, "Logged out successfully");
        });
}

void PassportApiController::InitClientRoutes(CrowApp& app){
    CROW_ROUTE(app, "/passport/api/v1/client/pairing")
        .methods("POST"_method)([&app](const crow::request& request, crow::response& response) {
            HandleJsonReqWithParams<std::string>(request, response,
                [](const std::string& pairingCode,crow::response& response){
                    return SuccessResponse(response, "pairing success");
                }, "pairingCode");
        });

    CROW_ROUTE(app, "/passport/api/v1/client/uppairing")
        .methods("POST"_method)([&app](const crow::request& request, crow::response& response) {
            HandleJsonReqWithParams<std::string>(request, response,
                [](const std::string& pairingCode,crow::response& response){
                    return SuccessResponse(response, "uppairing success");
                }, "pairingCode");
        });
}