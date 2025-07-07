#include "ImageManager.h"
#include <iostream>

ImageManager* ImageManager::instance = nullptr;

ImageManager* ImageManager::GetInstance() {
    if (instance == nullptr) {
        instance = new ImageManager();
    }
    return instance;
}

ImageManager::~ImageManager() {
    ReleaseAll();
}

bool ImageManager::LoadImage(const std::string& key, LPCTSTR filePath) {
    IMAGE* img = new IMAGE();

    try {
        loadimage(img, filePath);

        // 检查图片是否成功加载
        if (img->getwidth() > 0 && img->getheight() > 0) {
            images[key] = img;
            return true;
        }
        else {
            delete img;
            return false;
        }
    }
    catch (...) {
        delete img;
        return false;
    }
}

IMAGE* ImageManager::GetImage(const std::string& key) {
    auto it = images.find(key);
    if (it != images.end()) {
        return it->second;
    }
    return nullptr;
}

void ImageManager::ReleaseAll() {
    for (auto& pair : images) {
        delete pair.second;
    }
    images.clear();
}

void ImageManager::LoadCharacterImages() {
    // 加载所有角色图片
    LoadImage("warrior", _T("img\\Knight3.png"));

}

void ImageManager::DrawImageWithTransparency(const std::string& key, int x, int y) {
    IMAGE* img = GetImage(key);
    if (img) {
        DrawImageWithTransparency(img, x, y);
    }
}

void ImageManager::DrawImageWithTransparency(IMAGE* img, int x, int y) {
    if (!img) return;
    
    int width = img->getwidth();
    int height = img->getheight();
    
    // 方法1: 使用掩码绘制（适用于PNG透明背景）
    // 创建掩码
    IMAGE maskImg(width, height);
    SetWorkingImage(&maskImg);
    setbkcolor(WHITE);
    cleardevice();
    
    // 将原图绘制到掩码上，黑色部分变成白色，其他变成黑色
    putimage(0, 0, img);
    
    // 恢复到主画布
    SetWorkingImage(NULL);
    
    // 使用AND和OR操作实现透明效果
    putimage(x, y, &maskImg, SRCAND);
    putimage(x, y, img, SRCPAINT);
}