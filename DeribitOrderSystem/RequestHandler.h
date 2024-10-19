#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "IRequestHandler.h"
#include <curl/curl.h>
#include <string>
#include <vector>

class RequestHandler : public IRequestHandler {
public:
    std::string performRequest(const std::string& url, const std::string& post_fields, const std::vector<std::pair<std::string, std::string>>& headers) override;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
};

#endif // REQUEST_HANDLER_H
