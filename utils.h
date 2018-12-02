#ifndef UTILS_H
#define UTILS_H

#include <string>

typedef struct {
    std::string path;
    std::string domain;
    int port;
    std::string protocol;
    std::string query;
} url_t;
// http://www.zedwood.com/article/cpp-boost-url-regex
std::string trim(const std::string &str);
url_t parse_url(const std::string &url);
bool is_url_absolute(std::string url);
std::string get_first_piece(std::string& url);
std::string base_url(std::string &url);
std::string absolute_url(std::string url, std::string &relativeToUrl);
void init_daemon();

#endif
