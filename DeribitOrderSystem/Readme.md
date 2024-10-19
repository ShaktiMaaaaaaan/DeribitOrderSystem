Deribit Order System
This project is an order execution and management system designed to interact with the Deribit exchange API. The system supports key operations such as placing, canceling, and modifying orders, retrieving the order book, and viewing current positions.

Features
Authentication: Utilizes Deribit API's authentication with API keys and secrets.
Order Execution: Place limit orders and manage existing orders.
Order Book Retrieval: Fetch the current order book for specific instruments.
Position Management: View current positions and trades.
Error Handling: Robust error handling during API communication.
Requirements
Software
C++11 or higher
Visual Studio (or any other preferred C++ IDE)
nlohmann/json library for JSON parsing
libcurl for HTTP requests
Dependencies
nlohmann/json (for JSON parsing)
libcurl (for making HTTP requests)
You can install these libraries through your package manager or include them manually in the project.

Installation
Steps
Clone the repository:

bash
git clone https://github.com/ShaktiMaaaaaaan/DeribitOrderSystem.git
Configure the project in Visual Studio or another IDE of your choice.

Install Dependencies:

Add nlohmann/json.hpp to your project.
Link libcurl for HTTP requests.
Set Up API Keys:

In your project directory, create a configuration file (e.g., config.json) with your Deribit API credentials:
json
{
    "API": {
        "BaseURL": "https://test.deribit.com",
        "Key": "your_api_key",
        "Secret": "your_api_secret"
    }
}
Build the Project.

Project Structure
bash
DeribitOrderSystem/
│
├── src/
│   ├── DeribitOrderSystem.cpp      # Main application logic
│   ├── DeribitAPI.h                # Header for Deribit API interactions
│   ├── DeribitAPI.cpp              # Implementation for Deribit API calls
│   ├── AuthHandler.h               # Header for handling authentication
│   ├── AuthHandler.cpp             # Implementation for authentication logic
│   ├── RequestHandler.h            # Header for HTTP request handling
│   ├── RequestHandler.cpp          # Implementation for HTTP request handling
│   ├── IAuthHandler.h              # Interface for authentication
│   └── IRequestHandler.h           # Interface for HTTP requests
│
├── config/
│   └── config.json                 # Configuration file with API credentials
│
├── README.md                       # Project documentation
└── LICENSE                         # Project license (if applicable)
Usage
1. Authentication
The system authenticates with Deribit using API keys and retrieves an access token to perform requests. Make sure to update the config.json file with your API keys.

2. Placing an Order
To place a limit order, modify and run the following snippet in DeribitOrderSystem.cpp:

cpp
DeribitAPI api("config.json");
bool success = api.placeLimitOrder("BCH_USDC-PERPETUAL", "buy", 1.0, 200.0);
if (success) {
    std::cout << "Order placed successfully!" << std::endl;
} else {
    std::cout << "Order placement failed." << std::endl;
}
3. Retrieving Order Book
To get the order book for an instrument:

cpp
std::string orderbook = api.getOrderBook("BTC-PERPETUAL");
std::cout << "Order Book: " << orderbook << std::endl;
4. Viewing Positions
Retrieve your current positions:

cpp
std::string positions = api.getPositions();
std::cout << "Positions: " << positions << std::endl;
5. Error Handling
If any API call fails, error messages will be printed to the console, and detailed error logs can be handled through custom handlers.

Development
Extending the Project
You can add more API interactions, such as canceling orders, modifying existing ones, or adding new order types. The DeribitAPI class can be extended to support these functionalities.

Testing
Unit testing can be added using Google Test or other C++ testing frameworks.
Test against Deribit's testnet (set "BaseURL" to https://test.deribit.com in your config.json file).
Contributing
Feel free to fork this project, submit issues, or create pull requests for any improvements.

License
This project is licensed under the MIT License - see the LICENSE file for details.

References
Deribit API Documentation
nlohmann/json
libcurl
This README provides a comprehensive guide to setting up, using, and contributing to the Deribit Order System project. Feel free to adapt it based on specific needs or new features added to the project.
