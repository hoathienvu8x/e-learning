#include <stdexcept>
#include <curl/curl.h>
#include <map>

#include "utils.h"
#include "webcurl.h"

std::string get_html(std::string &url) {
    std::string readBuffer;
    try {
        CURLcode code;
        CURL *curl;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (curl == NULL) {
            throw std::runtime_error("Failed to create CURL connection");
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
        curl_easy_setopt(curl, CURLOPT_UNRESTRICTED_AUTH, 1L);
        curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Linux; Android 6.0.1; Nexus 5X Build/MMB29P) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.96 Mobile Safari/537.36 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        code = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (code != CURLE_OK) {
            throw std::runtime_error("Failed to retrieve web page");
        }
    } catch(std::exception err) {
        throw std::runtime_error(err.what());
    }
    return readBuffer;
}

std::vector<std::string> extract_links(std::string html) {
    std::string htmlRaw = reformat_http_response(html);
    const std::string urlStart[] = {"href=\"", "href = \"", "href='", "href = '"};
    const std::string urlEndChars = "\"#?, ";
    std::vector<std::string> links;
    for (auto startText : urlStart) {
        while (true) {
            size_t startPos = htmlRaw.find(startText);
            if (startPos == std::string::npos) break;
            startPos += startText.length();
            int endPos = htmlRaw.find_first_of(urlEndChars, startPos);
            std::string url = htmlRaw.substr(startPos, endPos - startPos);
            if (url.find("mailto:") != std::string::npos){
                htmlRaw.erase(0, endPos);
                continue;
            }
            if (is_url(url)) {
                links.push_back(url);
            }
            htmlRaw.erase(0, endPos);
        }
    }
    return links;
}

std::vector<std::string> parse_links_absolute(std::vector<std::string> links, std::string relativeToUrl) {
    std::vector<std::string> rlinks;
    for(auto url : links) {
        rlinks.push_back(absolute_url(url,relativeToUrl));
    }
    return rlinks;
}

std::string reformat_http_response(std::string text) {
    std::string allowedChrs = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01233456789.,/\":#?+-_= ";
    std::map<char, char> mm;
    for (char ch : allowedChrs) mm[ch] = ch;
    mm['\n'] = ' ';
    std::string res = "";
    for (char ch : text) {
        if (mm[ch]) res += tolower(mm[ch]);
    }
    return res;
}

bool is_url(std::string url) {
    std::string forbiddenTypes[] = {".css", ".js", ".pdf", ".png", ".jpeg", ".jpg", ".ico", ".gif",".bmp"};
    for (auto type : forbiddenTypes) {
        if (url.find(type) != std::string::npos) return false;
    }
    return true;
}
