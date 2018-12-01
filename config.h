#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

typedef struct {
    int delay = 1000;
    int depth = 0;
    int threads = 10;
    std::vector<std::string> startUrls;
} config_t;

static inline int intval(std::string s) {
    return std::stoi(s);
}

config_t load_config(const char *filename);
bool save_config(const char *filename, config_t conf);
void show_config(config_t conf);
config_t set_config(config_t conf, std::string key, int value);
config_t add_url(config_t conf, std::string url);
config_t remove_url(config_t conf, std::string url);

#endif
