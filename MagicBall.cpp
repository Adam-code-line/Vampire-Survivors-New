#include "MagicBall.h"
#include "ImageManager.h"
#include <cmath>

MagicBall::MagicBall(Vector2 pos, Vector2 dir, float spd)
    : GameObject(pos, 8), speed(spd), lifetime(0), maxLifetime(3.0f), damage(25),  // 半径从3改为8
      animationTimer(0), animationSpeed(0.1f), currentFrame(0), useImageRendering(false),
      rotationAngle(0), rotationSpeed(360.0f), pulseTimer(0), pulseSpeed(3.0f) {
    
    velocity = dir.Normalized() * speed;
    
    // 检查是否有魔法球图片
    ImageManager* imgManager = ImageManager::GetInstance();
    IMAGE* testImg = imgManager->GetImage("magic_attack");
    useImageRendering = (testImg != nullptr);
}

void MagicBall::Update(float deltaTime) {
    // 更新位置
    position = position + velocity * deltaTime;
    lifetime += deltaTime;
    
    // 更新动画
    animationTimer += deltaTime;
    rotationAngle += rotationSpeed * deltaTime;
    pulseTimer += pulseSpeed * deltaTime;
    
    // 保持角度在0-360度范围内
    if (rotationAngle >= 360.0f) {
        rotationAngle -= 360.0f;
    }
    
    // 检查生命周期和边界
    if (lifetime >= maxLifetime ||
        position.x < -50 || position.x > WINDOW_WIDTH + 50 ||
        position.y < -50 || position.y > WINDOW_HEIGHT + 50) {
        active = false;
    }
}

void MagicBall::Render() {
    if (useImageRendering) {
        RenderWithImage();
    } else {
        RenderWithGeometry();
    }
}

void MagicBall::RenderWithImage() {
    ImageManager* imgManager = ImageManager::GetInstance();
    IMAGE* magicImg = imgManager->GetImage("magic_attack");
    
    if (magicImg && magicImg->getwidth() > 0 && magicImg->getheight() > 0) {
        // 调整图片尺寸以匹配半径为8的魔法球
        float scale = 0.3f;  // 从15%增加到30%
        int scaledWidth = (int)(magicImg->getwidth() * scale);
        int scaledHeight = (int)(magicImg->getheight() * scale);
        
        int drawX = (int)position.x - scaledWidth / 2;
        int drawY = (int)position.y - scaledHeight / 2;
        
        // 使用缩放绘制
        imgManager->DrawImageScaled(magicImg, drawX, drawY, scale);
        
        // 添加魔法特效 - 旋转光环
        float pulseScale = 1.0f + 0.15f * sin(pulseTimer);
        int glowRadius = (int)(radius * pulseScale + 3);  // 光环距离从2增加到3
        
        // 外层光环
        setlinecolor(RGB(255, 0, 255));
        setlinestyle(PS_SOLID, 2);  // 线条粗细从1增加到2
        circle((int)position.x, (int)position.y, glowRadius);
        
        // 内层光环
        setlinecolor(RGB(200, 100, 255));
        setlinestyle(PS_SOLID, 1);
        circle((int)position.x, (int)position.y, glowRadius - 2);  // 内外光环距离调整
        
    } else {
        // 如果图片加载失败，使用几何图形
        useImageRendering = false;
        RenderWithGeometry();
    }
}

void MagicBall::RenderWithGeometry() {
    // 脉冲效果
    float pulseScale = 1.0f + 0.3f * sin(pulseTimer);  // 脉冲幅度从0.2f增加到0.3f
    int currentRadius = (int)(radius * pulseScale);
    
    // 主体魔法球 - 渐变紫色
    setfillcolor(RGB(255, 0, 255));
    solidcircle((int)position.x, (int)position.y, currentRadius);
    
    // 内核 - 亮白色
    setfillcolor(RGB(255, 255, 255));
    solidcircle((int)position.x, (int)position.y, currentRadius / 2);
    
    // 旋转的魔法符文效果
    float angleRad = rotationAngle * PI / 180.0f;
    int symbolRadius = currentRadius + 5;  // 符号距离从3增加到5
    
    // 绘制4个旋转的魔法符号
    for (int i = 0; i < 4; i++) {
        float symbolAngle = angleRad + (PI * i / 2);
        int symbolX = (int)(position.x + cos(symbolAngle) * symbolRadius);
        int symbolY = (int)(position.y + sin(symbolAngle) * symbolRadius);
        
        setfillcolor(RGB(200, 0, 255));
        solidcircle(symbolX, symbolY, 2);  // 符号半径从1增加到2
    }
    
    // 外围光环
    setlinecolor(RGB(255, 100, 255));
    setlinestyle(PS_SOLID, 2);  // 线条粗细从1增加到2
    circle((int)position.x, (int)position.y, currentRadius + 4);  // 距离从2增加到4
    
    // 能量轨迹效果
    setlinecolor(RGB(255, 200, 255));
    setlinestyle(PS_SOLID, 1);
    circle((int)position.x, (int)position.y, currentRadius + 8);  // 距离从4增加到8
}