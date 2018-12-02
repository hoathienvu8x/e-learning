#ifndef WEBCURL_H
#define WEBCURL_H

#include <string>
#include <vector>

static inline size_t write_callback(char *buff, size_t size, size_t nmemb, void *up) {
    ((std::string *)up)->append((char *)buff, size * nmemb);
    return size * nmemb;
}

std::string get_html(std::string &url);
std::vector<std::string> extract_links(std::string html);
std::vector<std::string> parse_links_absolute(std::vector<std::string> links, std::string relativeToUrl);
std::string reformat_http_response(std::string text);
bool is_url(std::string url);

#endif
