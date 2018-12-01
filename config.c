#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include "config.h"

config_t load_config(const char *filename) {
    config_t conf;
    try {
        std::string key, val, url;
        std::ifstream cFile(filename);
        while (cFile >> key >> val) {
            if (key == "delay") {
                conf.delay = intval(val);
            } else if (key == "depth") {
                conf.depth = intval(val);
            } else if (key == "threads") {
                conf.threads = intval(val);
            } else if (key == "urls") {
                for(int i = 0; i < intval(val); i++) {
                    cFile >> url;
                    conf.startUrls.push_back(url);
                }
            }
        }
        cFile.close();
    } catch(std::exception err) {
        throw std::runtime_error(std::string("Exception: (@load_config) ") + err.what());
    }
    return conf;
}

bool save_config(const char *filename, config_t conf) {
    try {
        std::ofstream cFile(filename);
        cFile << "delay " << conf.delay << std::endl;
        cFile << "depth " << conf.depth << std::endl;
        cFile << "threads " << conf.threads << std::endl;
        cFile << "urls " << conf.startUrls.size() << std::endl;
        for(size_t i = 0; i < conf.startUrls.size(); i++) {
            cFile << conf.startUrls[i] << std::endl;
        }
        cFile.close();
        return true;
    } catch (std::exception err) {
        throw std::runtime_error(std::string("Exception: (@save_config) ") + err.what());
    }
    return false;
}

void show_config(config_t conf){
    std::cout << "Configurations" << std::endl;
    std::cout << " Delay\t\t: " << conf.delay << " miliseconds" << std::endl;
    std::cout << " Depth\t\t: " << conf.depth << " level" << std::endl;
    std::cout << " Threads\t: " << conf.threads << " threads" << std::endl;
    std::cout << " URLS\t\t: " << conf.startUrls.size() << " urls" << std::endl;
    for(size_t i = 0; i < conf.startUrls.size(); i++) {
        std::cout << " " << (i+1) << ". " << conf.startUrls[i] << std::endl;
    }
}

config_t set_config(config_t conf, std::string key, int value) {
    if (value < 0) {
        return conf;
    }
    if (key == "delay") {
        conf.delay = value;
    } else if (key == "depth") {
        conf.depth = value;
    } else if (key == "threads") {
        conf.threads = value;
    }
    return conf;
}

config_t add_url(config_t conf, std::string url) {
    if (url.empty()) {
        return conf;
    }
    for(size_t i = 0; i < conf.startUrls.size(); i++) {
        if (conf.startUrls[i] == url) {
            return conf;
        }
    }
    conf.startUrls.push_back(url);
    return conf;
}

config_t remove_url(config_t conf, std::string url) {
    auto itr = std::find(conf.startUrls.begin(), conf.startUrls.end(), url);
    if (itr != conf.startUrls.end()) {
        conf.startUrls.erase(itr);
    }
    return conf;
}
