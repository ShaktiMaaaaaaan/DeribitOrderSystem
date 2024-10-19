#ifndef IREQUEST_HANDLER_H
#define IREQUEST_HANDLER_H

#include <string>
#include <vector>

class IRequestHandler {
public:
    virtual ~IRequestHandler() = default;

    virtual std::string performRequest(const std::string& url, const std::string& post_fields, const std::vector<std::pair<std::string, std::string>>& headers) = 0;
};

#endif // IREQUEST_HANDLER_H


