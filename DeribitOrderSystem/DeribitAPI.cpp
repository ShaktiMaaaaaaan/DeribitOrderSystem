#include "DeribitAPI.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h>

using json = nlohmann::json;

DeribitAPI::DeribitAPI(const std::string& config_path) {
    // Load configuration
    std::ifstream config_file(config_path);
    json config_json;

    if (config_file.is_open()) {
        config_file >> config_json;

        // Set base URL and keys
        base_url = config_json["API"]["BaseURL"];
        api_key = config_json["API"]["Key"];
        api_secret = config_json["API"]["Secret"];

        // Set endpoints
        endpoints[0] = config_json["Endpoints"]["Auth"];
        endpoints[1] = config_json["Endpoints"]["PlaceOrder"];
        endpoints[2] = config_json["Endpoints"]["CancelOrder"];
        endpoints[3] = config_json["Endpoints"]["ModifyOrder"];
        endpoints[4] = config_json["Endpoints"]["GetOrderbook"];
        endpoints[5] = config_json["Endpoints"]["GetPositions"];
    }
    else {
        std::cerr << "Could not open config file." << std::endl;
    }
}

bool DeribitAPI::authenticate() {
    // Construct the JSON request body
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

    // Set the required headers for the request
    std::vector<std::pair<std::string, std::string>> headers = {
        {"Content-Type", "application/json"}
    };

    std::string response = performRequest(endpoints[0], post_fields, headers);

    // Check if the response is empty
    if (response.empty()) {
        std::cerr << "Error: Empty response from the server. Authentication failed." << std::endl;
        return false;
    }

    // Print the authentication response
    try {
        auto json_response = json::parse(response);
        if (json_response.contains("result")) {
            access_token = json_response["result"]["access_token"];
            refresh_token = json_response["result"]["refresh_token"];
            std::cout << "Access Token: " << access_token << std::endl;
            std::cout << "Refresh Token: " << refresh_token << std::endl;
            return true;  // Authentication successful
        }
        else {
            std::cerr << "Authentication failed: " << json_response.dump() << std::endl;
            return false;
        }
    }
    catch (json::parse_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return false;
    }
}



// Callback function to write response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

std::string DeribitAPI::performRequest(const std::string& endpoint, const std::string& post_fields, const std::vector<std::pair<std::string, std::string> >& headers) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string full_url = base_url + endpoint;

        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());

        // Set custom headers
        struct curl_slist* header_list = NULL;
        for (const auto& header : headers) {
            std::string header_string = header.first + ": " + header.second;
            header_list = curl_slist_append(header_list, header_string.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(header_list);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        }
    }
    return readBuffer;
}

std::string DeribitAPI::placeOrder(const std::string& instrument, double amount, double price, const std::string& side) {
    // Construct the method (private/buy or private/sell based on the side)
    std::string method = side == "buy" ? "private/buy" : "private/sell";

    // Construct the JSON request body
    std::string post_fields = R"({
                                    "method": ")" + method + R"(",
                                    "params": {
                                        "instrument_name": ")" + instrument + R"(",
                                        "amount": )" + std::to_string(amount) + R"(,
                                        "price": )" + std::to_string(price) + R"(,
                                        "otoco_config": [{}]
                                    },
                                    "jsonrpc": "2.0",
                                    "id": 10
                                })";

    std::cout << "Constructed JSON: " << post_fields << std::endl;

    std::string response = performRequest(endpoints[1], post_fields, {
                                                                    {"Authorization", "Bearer " + access_token},
                                                                    {"Content-Type", "application/json"}
                                                                    });

    std::cout << "Place Order Response: " << response << std::endl;
    std::string order_id = "";

    // Handle error response
    try {
        auto json_response = json::parse(response);
        if (json_response.contains("error")) {
            // Extract error details
            std::string error_message = json_response["error"]["message"];
            std::string param = json_response["error"]["data"]["param"];
            std::string reason = json_response["error"]["data"]["reason"];

            std::cerr << "Error placing order: " << error_message << "\n";
            std::cerr << "Invalid Parameter: " << param << " - " << reason << std::endl;

            // Optionally, handle specific error codes here
            if (param == "amount" && reason == "must be a multiple of contract size") {
                std::cerr << "Please check the contract size for this instrument." << std::endl;
                return "";  // Return an empty string or handle it appropriately in your application
            }
        }
        else {

            // Extract the order information
            order_id = json_response["result"]["order"]["order_id"];
            std::string order_state = json_response["result"]["order"]["order_state"];
            double contracts = json_response["result"]["order"]["contracts"];
            double price = json_response["result"]["order"]["price"];
            std::string instrument_name = json_response["result"]["order"]["instrument_name"];
            std::string direction = json_response["result"]["order"]["direction"];
            double amount = json_response["result"]["order"]["amount"];

            // Print the extracted information
            std::cout << "Order ID: " << order_id << std::endl;
            std::cout << "Order State: " << order_state << std::endl;
            std::cout << "Contracts: " << contracts << std::endl;
            std::cout << "Price: " << price << std::endl;
            std::cout << "Instrument Name: " << instrument_name << std::endl;
            std::cout << "Direction: " << direction << std::endl;
            std::cout << "Amount: " << amount << std::endl;

        }
    }
    catch (json::parse_error& e) {
        std::cerr << "Error parsing response: " << e.what() << std::endl;
    }

    return order_id;
}


bool DeribitAPI::cancelOrder(const std::string& order_id) {
    // Construct the JSON request body
    std::string post_fields = R"({
        "jsonrpc": "2.0",
        "id": 4214,
        "method": "private/cancel",
        "params": {
            "order_id": ")" + order_id + R"("
        }
    })";

    // Perform the request to the API endpoint for canceling an order
    std::string response = performRequest(endpoints[2], post_fields, {
        {"Authorization", "Bearer " + access_token}, // Add authorization header if required
        {"Content-Type", "application/json"}
        });

    std::cout << "Cancel Order Response: " << response << std::endl;

    try {
        // Parse the JSON response
        auto json_response = json::parse(response);
        if (json_response.contains("result") && json_response["result"].contains("success")) {
            return json_response["result"]["success"];
        }
        else {
            std::cerr << "Cancellation failed: " << json_response.dump() << std::endl;
            return false;
        }
    }
    catch (json::parse_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return false;
    }
}

bool DeribitAPI::modifyOrder(const std::string& order_id, double new_price, double new_amount, const std::string& advanced) {
    // Construct the JSON request body using nlohmann::json
    json msg = {
        {"jsonrpc", "2.0"},
        {"id", 3725},
        {"method", "private/edit"},
        {"params", {
            {"order_id", order_id},
            {"price", new_price},
            {"amount", new_amount},
            {"advanced", advanced}
        }}
    };

    // Convert JSON object to string for HTTP request
    std::string post_fields = msg.dump();
    std::cout << "Modify Order Request: " << post_fields << std::endl;

    // Send the request and get the response
    std::string response = performRequest(endpoints[3], post_fields, {
        {"Authorization", "Bearer " + access_token},
        {"Content-Type", "application/json"}
        });

    std::cout << "Modify Order Response: " << response << std::endl;

    // Parse and handle the JSON response
    try {
        auto json_response = json::parse(response);
        if (json_response.contains("result")) {
            auto order_result = json_response["result"]["order"];
            if (order_result.contains("order_id") && order_result["order_id"] == order_id) {
                std::cout << "Order modified successfully. New price: " << order_result["price"] << std::endl;
                return true;
            }
        }
        else {
            std::cerr << "Modification failed: " << json_response.dump() << std::endl;
            return false;
        }
    }
    catch (json::parse_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return false;
    }
}



std::string DeribitAPI::getOrderbook(const std::string& instrument) {
    const int depth = 10;

    // Construct the JSON-RPC request with the depth parameter
    std::string post_fields = R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "public/get_order_book",
        "params": {
            "instrument_name": ")" + instrument + R"(",
            "depth": )" + std::to_string(depth) + R"(
        }
    })";

    // Perform the request
    std::string response = performRequest(endpoints[4], post_fields, {});

    // Output the response for debugging
    std::cout << "Get Orderbook Response: " << response << std::endl;

    // Parse the JSON response
    try {
        json jsonResponse = json::parse(response);

        // Check if the response contains an error
        if (jsonResponse.contains("error")) {
            std::cerr << "Error in response: " << jsonResponse["error"].dump() << std::endl;
            return ""; // Return an empty string on error
        }

        // Access the result and print relevant information
        auto& result = jsonResponse["result"];
        std::cout << "Instrument Name: " << result["instrument_name"] << std::endl;
        std::cout << "Best Bid Price: " << result["best_bid_price"] << std::endl;
        std::cout << "Best Ask Price: " << result["best_ask_price"] << std::endl;
        std::cout << "Best Bid Amount: " << result["best_bid_amount"] << std::endl;
        std::cout << "Best Ask Amount: " << result["best_ask_amount"] << std::endl;
        // You can access more fields as needed

    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    }

    return response; // Return the raw response if needed
}




std::string DeribitAPI::getPosition(const std::string& instrument_name) {
    // Construct the JSON payload to match the provided request format
    std::string post_fields = R"({
        "jsonrpc": "2.0",
        "id": 12,
        "method": "private/get_position",
        "params": {
            "instrument_name": ")" + instrument_name + R"("
        }
    })";

    // Perform the API request, adding the necessary headers
    std::string response = performRequest(endpoints[5], post_fields, {
        {"Authorization", "Bearer " + access_token},  // Ensure you have a valid access token
        {"Content-Type", "application/json"}          // Ensure content-type is JSON
        });

    // Output the response for debugging purposes
    std::cout << "Get Position Response: " << response << std::endl;

    try {
        // Parse the JSON response
        auto json_response = json::parse(response);

        // Check if the response contains the "result" field
        if (json_response.contains("result")) {
            // Extract relevant details from the response (as per your example)
            auto result = json_response["result"];
            std::cout << "Instrument Name: " << result["instrument_name"] << std::endl;
            std::cout << "Mark Price: " << result["mark_price"] << std::endl;
            std::cout << "Leverage: " << result["leverage"] << std::endl;
            // You can print or extract more fields as needed

            return response;  // Return the full response if needed
        }
        else if (json_response.contains("error")) {
            // Handle API error response
            std::cerr << "Error in response: " << json_response["error"].dump() << std::endl;
        }
    }
    catch (json::parse_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }

    return "";  // Return an empty string in case of failure
}

