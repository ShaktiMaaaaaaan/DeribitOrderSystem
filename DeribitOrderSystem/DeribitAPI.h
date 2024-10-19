#ifndef DERIBIT_API_H
#define DERIBIT_API_H

#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>  // Include JSON library
#include <vector>  // Include vector for handling headers

class DeribitAPI {
private:
    std::string api_key;
    std::string api_secret;
    std::string base_url;
    std::string endpoints[6];

    // Add member variables to store tokens
    std::string access_token;
    std::string refresh_token;


public:
   // DeribitAPI(const std::string& config_path);
    DeribitAPI(const std::string& config_path);

    // Authenticates and fetches access token
    bool authenticate();

    // Utility method to perform HTTP requests
    std::string performRequest(const std::string& endpoint, const std::string& post_fields, const std::vector<std::pair<std::string, std::string>>& headers);

    // Places an order (buy/sell)
    std::string placeOrder(const std::string& instrument, double amount, double price, const std::string& side);

    // Cancels an order
    bool cancelOrder(const std::string& order_id);

    // Modifies an order
    bool modifyOrder(const std::string& order_id, double new_price, double new_amount, const std::string& advanced);
    
    // Gets the orderbook for a specific symbol
    std::string getOrderbook(const std::string& instrument);

    // Gets current positions
    std::string getPosition(const std::string& instrument_name);
};

#endif // DERIBIT_API_H
