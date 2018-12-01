#ifndef UTILS_H
#define UTILS_H

#include <string>

typedef struct {
    std::string path;
    std::string domain;
    int port;
    std::string path;
    std::string query;
} url_t;
// http://www.zedwood.com/article/cpp-boost-url-regex
std::string trim(const std::string &str);
url_t parse_url(const std::string &url);


#endif
