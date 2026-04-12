#pragma once

#include <string>

class SplashTexture {
public:
    bool Load(const std::string& path);
    void Destroy();

    unsigned int id = 0;
    int width = 0;
    int height = 0;
};