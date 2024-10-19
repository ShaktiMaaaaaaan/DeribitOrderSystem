// RequestHandler.cpp
#include "RequestHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h> // Include curl for HTTP requests

//std::string RequestHandler::performRequest(const std::string& url, const std::string& post_fields, const std::vector<std::pair<std::string, std::string>>& headers) {
//    CURL* curl = curl_easy_init();
//    if (!curl) {
//        return "CURL initialization failed";
//    }
//
//    std::string response_string;
//    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
//
//    struct curl_slist* chunk = NULL;
//    for (const auto& header : headers) {
//        std::string header_string = header.first + ": " + header.second;
//        chunk = curl_slist_append(chunk, header_string.c_str());
//    }
//    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
//
//    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nullptr);
//    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
//    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // Optional for debugging
//
//    CURLcode res = curl_easy_perform(curl);
//    if (res != CURLE_OK) {
//        response_string = "CURL error: " + std::string(curl_easy_strerror(res));
//    }
//
//    curl_slist_free_all(chunk); // Cleanup
//    curl_easy_cleanup(curl); // Cleanup
//    return response_string;
//}


std::string RequestHandler::performRequest(const std::string& endpoint, const std::string& post_fields, const std::vector<std::pair<std::string, std::string> >& headers) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string full_url = "https://test.deribit.com/api/v2/" + endpoint;

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
