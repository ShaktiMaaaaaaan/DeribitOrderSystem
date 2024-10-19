// DeribitOrderSystem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "DeribitAPI.h"
#include <iostream>
#include <string>


int main() {
    const std::string config_path = "app.config.json";  // Path to your config file


    // Instantiate the Deribit API class
    DeribitAPI deribit(config_path);

    // Authenticate the session
    if (deribit.authenticate()) {
        // Place a buy order for Bitcoin (example)
        std::string order_id = deribit.placeOrder("BCH_USDC-PERPETUAL", 1.0, 200.0, "buy");


        if (!order_id.empty()) {
            std::cout << "Successfully placed order, Order ID: " << order_id << std::endl;
        }
        else {
            std::cerr << "Failed to place the order." << std::endl;
        }


        // Cancel an order (example order ID)
      //  deribit.cancelOrder("ETH-SLIS-12");


        bool status  = deribit.cancelOrder("USDC-13563131618");
        if (status) {
            std::cout << "Order cancelled successfully!" << std::endl;
        }
        else {
            std::cout << "Failed to Cancel order." << std::endl;
        }


        // Modify an order (example order ID)
        bool success = deribit.modifyOrder("438994", 222.0, 4.0, "implv");
        if (success) {
            std::cout << "Order successfully modified!" << std::endl;
        }
        else {
            std::cout << "Failed to modify order." << std::endl;
        }

        // Get orderbook for Bitcoin perpetual
        std::string orderbook = deribit.getOrderbook("BCH_USDC-PERPETUAL");

        // View current positions
        std::string position = deribit.getPosition("BCH_USDC-PERPETUAL");

    }
    else {
        std::cout << "Authentication failed." << std::endl;
    }

    return 0;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
