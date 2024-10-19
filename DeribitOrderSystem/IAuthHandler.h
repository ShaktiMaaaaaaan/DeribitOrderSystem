#ifndef IAUTH_HANDLER_H
#define IAUTH_HANDLER_H

#include <string>

class IAuthHandler {
public:
    virtual ~IAuthHandler() = default;

    virtual void setApiKey(const std::string& key) = 0;
    virtual void setApiSecret(const std::string& secret) = 0;
    virtual bool authenticate(const std::string& base_url) = 0;
};

#endif // IAUTH_HANDLER_H
