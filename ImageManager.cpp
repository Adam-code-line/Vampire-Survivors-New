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
    // 加载战士图片（向右和向左）
    LoadImage("warrior", _T("img\\Knight6.png"));
    LoadImage("warrior_right", _T("img\\Knight6_right.png"));
    
    // 加载战士斩击动画帧（向右和向左）
    LoadImage("cut1", _T("img\\cut1.png"));
    LoadImage("cut2", _T("img\\cut2.png"));
    LoadImage("cut3", _T("img\\cut3.png"));
    LoadImage("cut1_right", _T("img\\cut1_right.png"));
    LoadImage("cut2_right", _T("img\\cut2_right.png"));
    LoadImage("cut3_right", _T("img\\cut3_right.png"));
    
    // 加载剑气图片（四个方向）
    LoadImage("attack_up", _T("img\\attack_up.png"));
    LoadImage("attack_down", _T("img\\attack_down.png"));
    LoadImage("attack_left", _T("img\\attack_left.png"));
    LoadImage("attack_right", _T("img\\attack_right.png"));
    
    // 加载背景图片
    LoadImage("background", _T("img\\background.png"));
    
    // 可以在这里加载其他角色的图片
    // LoadImage("mage", _T("img\\Mage.png"));
    // LoadImage("mage_right", _T("img\\Mage_right.png"));
    // LoadImage("archer", _T("img\\Archer.png"));
    // LoadImage("archer_right", _T("img\\Archer_right.png"));
    // LoadImage("assassin", _T("img\\Assassin.png"));
    // LoadImage("assassin_right", _T("img\\Assassin_right.png"));
}

void ImageManager::DrawImageWithTransparency(const std::string& key, int x, int y) {
    IMAGE* img = GetImage(key);
    if (img) {
        DrawImageWithTransparency(img, x, y);
    }
}

// 更可靠的透明绘制方法
void ImageManager::DrawImageWithTransparency(IMAGE* img, int x, int y) {
    if (!img) return;
    
    // 方法：手动处理像素透明度
    DWORD* pBuf = GetImageBuffer(img);
    if (!pBuf) {
        // 如果无法获取缓冲区，直接绘制
        putimage(x, y, img);
        return;
    }
    
    int width = img->getwidth();
    int height = img->getheight();
    
    // 创建临时图像用于透明处理
    IMAGE tempImg;
    tempImg.Resize(width, height);
    
    // 获取当前屏幕内容作为背景
    getimage(&tempImg, x, y, width, height);
    DWORD* tempBuf = GetImageBuffer(&tempImg);
    
    if (tempBuf) {
        // 逐像素处理透明度
        for (int i = 0; i < width * height; i++) {
            DWORD srcPixel = pBuf[i];
            
            // 检查是否为黑色或深色（当作透明）
            int r = GetRValue(srcPixel);
            int g = GetGValue(srcPixel);
            int b = GetBValue(srcPixel);
            
            // 如果不是黑色/深色，则使用原像素；否则保持背景
            if (r > 20 || g > 20 || b > 20) {
                tempBuf[i] = srcPixel;
            }
            // 黑色像素保持不变（使用背景）
        }
        
        // 绘制处理后的图像
        putimage(x, y, &tempImg);
    } else {
        // 备用方案
        putimage(x, y, img);
    }
}

// 缩放绘制方法
void ImageManager::DrawImageScaled(IMAGE* img, int x, int y, float scale) {
    if (!img || scale <= 0) return;
    
    int originalWidth = img->getwidth();
    int originalHeight = img->getheight();
    int scaledWidth = (int)(originalWidth * scale);
    int scaledHeight = (int)(originalHeight * scale);
    
    if (scale == 1.0f) {
        // 无需缩放，直接绘制
        DrawImageWithTransparency(img, x, y);
        return;
    }
    
    // 创建缩放后的临时图像
    IMAGE scaledImg;
    scaledImg.Resize(scaledWidth, scaledHeight);
    
    DWORD* srcBuf = GetImageBuffer(img);
    DWORD* dstBuf = GetImageBuffer(&scaledImg);
    
    if (srcBuf && dstBuf) {
        // 简单的最近邻缩放
        for (int y = 0; y < scaledHeight; y++) {
            for (int x = 0; x < scaledWidth; x++) {
                int srcX = (int)(x / scale);
                int srcY = (int)(y / scale);
                
                if (srcX < originalWidth && srcY < originalHeight) {
                    dstBuf[y * scaledWidth + x] = srcBuf[srcY * originalWidth + srcX];
                }
            }
        }
        
        DrawImageWithTransparency(&scaledImg, x, y);
    } else {
        // 备用方案：直接绘制原图
        DrawImageWithTransparency(img, x, y);
    }
}

void ImageManager::DrawImageFlippedHorizontal(IMAGE* img, int x, int y) {
    if (!img) return;
    
    int width = img->getwidth();
    int height = img->getheight();
    
    // 创建翻转后的临时图像
    IMAGE flippedImg;
    flippedImg.Resize(width, height);
    
    DWORD* srcBuf = GetImageBuffer(img);
    DWORD* dstBuf = GetImageBuffer(&flippedImg);
    
    if (srcBuf && dstBuf) {
        // 水平翻转像素
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int srcIndex = row * width + col;
                int dstIndex = row * width + (width - 1 - col);
                dstBuf[dstIndex] = srcBuf[srcIndex];
            }
        }
        
        // 绘制翻转后的图像
        DrawImageWithTransparency(&flippedImg, x, y);
    } else {
        // 备用方案：直接绘制原图
        DrawImageWithTransparency(img, x, y);
    }
}

void ImageManager::DrawBackground() {
    IMAGE* bgImg = GetImage("background");
    if (bgImg) {
        putimage(0, 0, bgImg);
    }
}

void ImageManager::DrawScaledBackground(int windowWidth, int windowHeight) {
    IMAGE* bgImg = GetImage("background");
    if (bgImg) {
        int imgWidth = bgImg->getwidth();
        int imgHeight = bgImg->getheight();
        
        if (imgWidth != windowWidth || imgHeight != windowHeight) {
            for (int x = 0; x < windowWidth; x += imgWidth) {
                for (int y = 0; y < windowHeight; y += imgHeight) {
                    putimage(x, y, bgImg);
                }
            }
        } else {
            putimage(0, 0, bgImg);
        }
    }
}