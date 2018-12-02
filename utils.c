#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <regex>

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
// Source uri: https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux/17955149#17955149
void init_daemon() {
    pid_t pid;
    /* Fork off the parent process */
    pid = fork();
    /* An error occurred */
    if (pid < 0){
        exit(EXIT_FAILURE);
    }
    /* Success: Let the parent terminate */
    if (pid > 0){
        exit(EXIT_SUCCESS);
    }
    /* On success: The child process becomes session leader */
    if (setsid() < 0){
        exit(EXIT_FAILURE);
    }
    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    /* Fork off for the second time*/
    pid = fork();
    /* An error occurred */
    if (pid < 0){
        exit(EXIT_FAILURE);
    }
    /* Success: Let the parent terminate */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    /* Set new file permissions */
    umask(0);
    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");
    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--){
        close (x);
    }
    /* Open the log file */
    openlog ("firstdaemon", LOG_PID, LOG_DAEMON);
}

bool is_url_absolute(std::string url){
    const std::string urlRegexStr = "(http|ftp|https)://([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:/~+#-]*[\\w@?^=%&/~+#-])?";
    const std::regex urlRegex(urlRegexStr.c_str());
    return std::regex_match(url,urlRegex);
}

std::string base_url(std::string &url){
    std::string baseURL = "";
    std::size_t toIgnore = url.find("://");
    std::size_t found = url.find_last_of("/");
    if(found == std::string::npos || found < toIgnore + 3){
        baseURL = url + "/";
    } else {
        baseURL = url.substr(0,found + 1);
    }
    return baseURL;
}

std::string get_first_piece(std::string& url){
    std::size_t found = url.find("//");
    if(found == std::string::npos){
        found = url.find("/");
    } else {
        found = url.find("/",found+2);
    }
    if(found == std::string::npos){
        return url;
    }
    std::string firstPiece = url.substr(0,found);
    return firstPiece;
}

std::string absolute_url(std::string url, std::string &relativeToUrl){
    if (is_url_absolute(url)) {
        return url;
    }
    std::string baseURL = base_url(relativeToUrl);
    std::string absoluteUrl = "";
    std::size_t found = url.find("/");
    if(found != std::string::npos){
        if(found == 0){
            absoluteUrl = get_first_piece(relativeToUrl) +  url;
        } else {
            absoluteUrl = baseURL + url;
        }
    } else {
        absoluteUrl = baseURL + url;
    }
    return absoluteUrl;
}
