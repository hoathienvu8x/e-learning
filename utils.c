#include "utils.h"

std::string trim(const std::string &str){
    size_t start = str.find_first_not_of(" \n\r\t");
    size_t until = str.find_last_not_of(" \n\r\t");
    std::string::const_iterator i = start == std::string::npos ? str.begin() : str.begin() + start;
    std::string::const_iterator x = until == std::string::npos ? str.end() : str.begin() + until + 1;
    return std::string(i, x);
}

url_t parse_url(const std::string &url) {
    url_t uri;
    int offset = 0;
    size_t pos1, pos2, pos3, pos4;
    std::string x = trim(url);
    offset = offset == 0 && x.compare(0, 8, "https://") == 0 ? 8 : offset;
    offset = offset == 0 && x.compare(0, 7, "http://") == 0 ? 7 : offset;
    pos1 = x.find_first_of('/', offset + 1);
    uri.path = pos1 == std::string::npos ? "" : x.substr(pos1);
    uri.domain = std::string(x.begin() + offset, pos1 == std::string::npos ? x.begin() + pos1 : x.end());
    uri.path = (pos2 = uri.path.find("#")) != std::string::npos ? uri.path.substr(0, pos2) : uri.path;
    uri.port = std::stoi((pos3 = uri.domain.find(":")) != std::string::npos ? uri.domain.substr(pos3 + 1) : "");
    uri.domain = uri.domain.substr(0, pos3 != std::string::npos ? pos3 : uri.domain.length());
    uri.protocol = offset > 0 ? x.substr(0, offset - 3) : "";
    uri.query = (pos4 = uri.path.find("?")) != std::string::npos ? uri.path.substr(pos4 + 1) : "";
    uri.path = pos4 != std::string::npos ? uri.path.substr(0, pos4) : uri.path;
    return uri;
}
