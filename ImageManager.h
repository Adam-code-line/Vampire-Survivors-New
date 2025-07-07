#pragma once
#include <graphics.h>
#include <map>
#include <string>

class ImageManager {
private:
    static ImageManager* instance;
    std::map<std::string, IMAGE*> images;
    
    ImageManager() {}
    
public:
    static ImageManager* GetInstance();
    ~ImageManager();
    
    bool LoadImage(const std::string& key, LPCTSTR filePath);
    IMAGE* GetImage(const std::string& key);
    void ReleaseAll();
    
    // 预加载所有角色图片
    void LoadCharacterImages();
    
    // 透明绘制函数
    void DrawImageWithTransparency(const std::string& key, int x, int y);
    void DrawImageWithTransparency(IMAGE* img, int x, int y);
    
    // 缩放绘制函数
    void DrawImageScaled(IMAGE* img, int x, int y, float scale);
    
    // 水平翻转绘制函数
    void DrawImageFlippedHorizontal(IMAGE* img, int x, int y);
    
    // 背景绘制函数
    void DrawBackground();
    void DrawScaledBackground(int windowWidth, int windowHeight);
};