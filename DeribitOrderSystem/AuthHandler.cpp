#include "AuthHandler.h"

void AuthHandler::setApiKey(const std::string& key) {
    api_key = key;
}

void AuthHandler::setApiSecret(const std::string& secret) {
    api_secret = secret;
}

bool AuthHandler::authenticate(const std::string& base_url) {
    std::string post_fields = R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "public/auth",
        "params": {
            "grant_type": "client_credentials",
            "client_id": ")" + api_key + R"(",
            "client_secret": ")" + api_secret + R"("
        }
    })";

    std::vector<std::pair<std::string, std::string>> headers = {
        {"Content-Type", "application/json"}
    };

    RequestHandler req_handler; // Create an instance or pass it in
    std::string response = req_handler.performRequest(base_url + "/api/v2/public/auth", post_fields, headers);
    json json_response = json::parse(response);

    if (json_response.contains("result")) {
        access_token = json_response["result"]["access_token"];
        refresh_token = json_response["result"]["refresh_token"];
        return true;
    }
    return false;
}
