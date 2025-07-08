#include "Enemy.h"
#include "ImageManager.h"

Enemy::Enemy(Vector2 pos, Player* player, int enemyLevel)
    : GameObject(pos, 15), speed(80.0f), target(player), experienceValue(10), 
      level(enemyLevel), sizeMultiplier(1.0f),
      animationTimer(0), animationSpeed(0.2f), currentFrame(0),
      facingRight(true), isMoving(false), useImageRendering(false) {
    
    // 根据等级调整属性
    SetLevel(level);
    
    // 检查是否有对应的图片资源
    CheckImageResources();
}

void Enemy::SetLevel(int newLevel) {
    level = newLevel;
    
    // 根据等级调整属性（移除体型增大）
    sizeMultiplier = 1.0f;  // 固定大小，不随等级变化
    radius = 15.0f;         // 固定半径
    
    speed = 80.0f + (level - 1) * 10.0f;          // 每级增加10速度
    health = maxHealth = 20 + (level - 1) * 15;   // 每级增加15血量
    experienceValue = 10 + (level - 1) * 5;       // 每级增加5经验
    
    // 根据等级调整动画速度
    if (level == 2) {
        animationSpeed = 0.15f;  // 2级：更快的动画切换
    } else if (level == 3) {
        animationSpeed = 0.12f;  // 3级：最快的动画切换
    } else {
        animationSpeed = 0.2f;   // 默认速度
    }
    
    // 重新检查图片资源
    CheckImageResources();
}

void Enemy::CheckImageResources() {
    ImageManager* imgManager = ImageManager::GetInstance();
    
    if (level == 1) {
        // 检查1级怪物图片
        IMAGE* testImg = imgManager->GetImage("monster1_right1");
        useImageRendering = (testImg != nullptr);
    } else if (level == 2) {
        // 检查2级怪物图片
        IMAGE* testImg = imgManager->GetImage("monster2_right1");
        useImageRendering = (testImg != nullptr);
    } else if (level == 3) {
        // 检查3级怪物图片
        IMAGE* testImg = imgManager->GetImage("monster3_right1");
        useImageRendering = (testImg != nullptr);
    } else {
        useImageRendering = false;
    }
}

void Enemy::Update(float deltaTime) {
    Vector2 oldPosition = position;
    
    if (target && target->active) {
        // 向玩家移动
        Vector2 direction = (target->position - position).Normalized();
        position = position + direction * speed * deltaTime;
        
        // 修复朝向逻辑：
        // facingRight = true 表示使用 _right 图片（面向右）
        // facingRight = false 表示使用 _left 图片（面向左）
        if (abs(direction.x) > 0.1f) { // 只有在明显的水平移动时才更新朝向
            facingRight = direction.x > 0; // 向右移动时 facingRight = true
        }
        
        // 检查是否在移动
        isMoving = (position - oldPosition).Length() > 0.1f;
    } else {
        isMoving = false;
    }
    
    // 更新动画
    UpdateAnimation(deltaTime);
}

void Enemy::UpdateAnimation(float deltaTime) {
    if (isMoving && (level == 1 || level == 2 || level == 3)) {
        // 更新动画计时器
        animationTimer += deltaTime;
        
        // 切换动画帧
        if (animationTimer >= animationSpeed) {
            animationTimer = 0;
            
            if (level == 1) {
                currentFrame = (currentFrame + 1) % 3; // 1级：3帧动画循环 (0, 1, 2)
            } else if (level == 2) {
                currentFrame = (currentFrame + 1) % 7; // 2级：7帧动画循环 (0-6)
            } else if (level == 3) {
                currentFrame = (currentFrame + 1) % 4; // 3级：4帧动画循环 (0-3)
            }
        }
    } else {
        // 停止移动时重置动画
        animationTimer = 0;
        currentFrame = 0;
    }
}

void Enemy::Render() {
    // 严格按照渲染模式选择，避免混合
    if (useImageRendering && (level == 1 || level == 2 || level == 3)) {
        RenderWithImage();
    } else {
        RenderWithGeometry();
    }
}

void Enemy::RenderWithImage() {
    ImageManager* imgManager = ImageManager::GetInstance();
    std::string imageKey;
    
    if (level == 1) {
        // 1级怪物逻辑
        if (isMoving) {
            if (facingRight) {
                switch (currentFrame) {
                    case 0: imageKey = "monster1_right1"; break;
                    case 1: 
                        imageKey = "monster1_right2";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster1_right1";
                        }
                        break;
                    case 2: 
                        imageKey = "monster1_right3";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster1_right1";
                        }
                        break;
                    default: imageKey = "monster1_right1"; break;
                }
            } else {
                switch (currentFrame) {
                    case 0: 
                        imageKey = "monster1_left1";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster1_right1";
                        }
                        break;
                    case 1: 
                        imageKey = "monster1_left2";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster1_right1";
                        }
                        break;
                    case 2: 
                        imageKey = "monster1_left3";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster1_right1";
                        }
                        break;
                    default: imageKey = "monster1_right1"; break;
                }
            }
        } else {
            imageKey = facingRight ? "monster1_right1" : "monster1_left1";
            if (imgManager->GetImage(imageKey) == nullptr) {
                imageKey = "monster1_right1";
            }
        }
    } else if (level == 2) {
        // 2级怪物逻辑
        if (isMoving) {
            if (facingRight) {
                switch (currentFrame) {
                    case 0: imageKey = "monster2_right1"; break;
                    case 1: imageKey = "monster2_right2"; break;
                    case 2: imageKey = "monster2_right3"; break;
                    case 3: imageKey = "monster2_right4"; break;
                    case 4: imageKey = "monster2_right5"; break;
                    case 5: imageKey = "monster2_right6"; break;
                    case 6: imageKey = "monster2_right7"; break;
                    default: imageKey = "monster2_right1"; break;
                }
            } else {
                switch (currentFrame) {
                    case 0: imageKey = "monster2_left1"; break;
                    case 1: imageKey = "monster2_left2"; break;
                    case 2: imageKey = "monster2_left3"; break;
                    case 3: imageKey = "monster2_left4"; break;
                    case 4: imageKey = "monster2_left5"; break;
                    case 5: imageKey = "monster2_left6"; break;
                    case 6: imageKey = "monster2_left7"; break;
                    default: imageKey = "monster2_left1"; break;
                }
            }
        } else {
            imageKey = facingRight ? "monster2_right1" : "monster2_left1";
        }
    } else if (level == 3) {
        // 3级怪物动画逻辑 - 新增
        if (isMoving) {
            if (facingRight) {
                switch (currentFrame) {
                    case 0: imageKey = "monster3_right1"; break;
                    case 1: imageKey = "monster3_right2"; break;
                    case 2: imageKey = "monster3_right3"; break;
                    case 3: imageKey = "monster3_right4"; break;
                    default: imageKey = "monster3_right1"; break;
                }
            } else {
                switch (currentFrame) {
                    case 0: 
                        imageKey = "monster3_left1";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster3_right1";
                        }
                        break;
                    case 1: 
                        imageKey = "monster3_left2";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster3_right2";
                        }
                        break;
                    case 2: 
                        imageKey = "monster3_left3";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster3_right3";
                        }
                        break;
                    case 3: 
                        imageKey = "monster3_left4";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster3_right4";
                        }
                        break;
                    default: 
                        imageKey = "monster3_left1";
                        if (imgManager->GetImage(imageKey) == nullptr) {
                            imageKey = "monster3_right1";
                        }
                        break;
                }
            }
        } else {
            // 静止状态
            if (facingRight) {
                imageKey = "monster3_right1";
            } else {
                imageKey = "monster3_left1";
                if (imgManager->GetImage(imageKey) == nullptr) {
                    imageKey = "monster3_right1";
                }
            }
        }
    }
    
    IMAGE* monsterImg = imgManager->GetImage(imageKey);
    
    if (monsterImg && monsterImg->getwidth() > 0 && monsterImg->getheight() > 0) {
        int drawX = (int)position.x - monsterImg->getwidth() / 2;
        int drawY = (int)position.y - monsterImg->getheight() / 2;
        
        // 使用改进的透明绘制
        imgManager->DrawImageWithTransparency(monsterImg, drawX, drawY);
    } else {
        // 如果连基本图片都没有，切换到几何图形
        useImageRendering = false;
        RenderWithGeometry();
        return;
    }
    
    // 绘制血条（如果血量不满）
    if (health < maxHealth) {
        int barWidth = 30;
        int barHeight = 4;
        int barX = (int)(position.x - barWidth / 2);
        int barY = (int)(position.y - radius - 8);

        // 背景
        setfillcolor(RGB(64, 64, 64));
        solidrectangle(barX, barY, barX + barWidth, barY + barHeight);

        // 血量 - 根据等级调整血条颜色
        COLORREF healthColor;
        if (level == 1) {
            healthColor = RGB(255, 100, 100); // 红色血条
        } else if (level == 2) {
            healthColor = RGB(255, 150, 0);   // 橙色血条
        } else if (level == 3) {
            healthColor = RGB(255, 0, 255);   // 紫色血条
        } else {
            healthColor = RGB(150, 150, 150); // 灰色血条
        }
        
        int healthWidth = (barWidth * health) / maxHealth;
        setfillcolor(healthColor);
        solidrectangle(barX, barY, barX + healthWidth, barY + barHeight);
        
        // 边框
        setlinecolor(WHITE);
        rectangle(barX, barY, barX + barWidth, barY + barHeight);
    }
}

void Enemy::RenderWithGeometry() {
    // 根据等级设置颜色
    COLORREF enemyColor;
    switch (level) {
        case 1: 
            enemyColor = RGB(200, 50, 50);  // 深红色
            break;
        case 2: 
            enemyColor = RGB(255, 100, 0);  // 橙色
            break;      
        case 3: 
            enemyColor = RGB(255, 0, 255);  // 紫色
            break;      
        case 4: 
            enemyColor = RGB(100, 0, 255);  // 深紫色
            break;      
        default: 
            enemyColor = RGB(50, 50, 50);   // 深灰色
            break;      
    }
    
    // 绘制敌人主体
    setfillcolor(enemyColor);
    solidcircle((int)position.x, (int)position.y, (int)radius);
    
    // 添加边框
    setlinecolor(WHITE);
    circle((int)position.x, (int)position.y, (int)radius);
    
    // 添加不同等级的动画效果
    if (isMoving) {
        if (level == 1) {
            // 1级：简单的眼睛闪烁
            if (currentFrame == 1) {
                setfillcolor(WHITE);
                solidcircle((int)(position.x - 5), (int)(position.y - 3), 2);
                solidcircle((int)(position.x + 5), (int)(position.y - 3), 2);
                
                setfillcolor(BLACK);
                solidcircle((int)(position.x - 5), (int)(position.y - 3), 1);
                solidcircle((int)(position.x + 5), (int)(position.y - 3), 1);
            }
        } else if (level == 2) {
            // 2级：眼睛发光效果
            if (currentFrame == 2 || currentFrame == 5) {
                setfillcolor(RGB(255, 255, 0));
                solidcircle((int)(position.x - 6), (int)(position.y - 4), 3);
                solidcircle((int)(position.x + 6), (int)(position.y - 4), 3);
                
                setfillcolor(RGB(255, 0, 0));
                solidcircle((int)(position.x - 6), (int)(position.y - 4), 1);
                solidcircle((int)(position.x + 6), (int)(position.y - 4), 1);
            }
            
            if (currentFrame == 3 || currentFrame == 6) {
                setlinecolor(RGB(255, 150, 0));
                setlinestyle(PS_SOLID, 2);
                circle((int)position.x, (int)position.y, (int)(radius + 3));
            }
        } else if (level == 3) {
            // 3级：更高级的动画效果
            if (currentFrame == 1 || currentFrame == 3) {
                // 紫色发光眼睛
                setfillcolor(RGB(255, 0, 255));
                solidcircle((int)(position.x - 7), (int)(position.y - 5), 4);
                solidcircle((int)(position.x + 7), (int)(position.y - 5), 4);
                
                setfillcolor(RGB(255, 255, 255));
                solidcircle((int)(position.x - 7), (int)(position.y - 5), 2);
                solidcircle((int)(position.x + 7), (int)(position.y - 5), 2);
            }
            
            if (currentFrame == 2) {
                // 双重脉冲环
                setlinecolor(RGB(255, 0, 255));
                setlinestyle(PS_SOLID, 3);
                circle((int)position.x, (int)position.y, (int)(radius + 5));
                circle((int)position.x, (int)position.y, (int)(radius + 8));
            }
            
            // 角上的装饰
            if (currentFrame == 0 || currentFrame == 2) {
                setfillcolor(RGB(150, 0, 150));
                solidcircle((int)(position.x - 8), (int)(position.y - 8), 3);
                solidcircle((int)(position.x + 8), (int)(position.y - 8), 3);
            }
        }
    }
    
    // 绘制等级标识
    if (level > 1) {
        settextcolor(WHITE);
        settextstyle(12, 0, _T("Arial"));
        TCHAR levelText[10];
        _stprintf_s(levelText, _T("%d"), level);
        outtextxy((int)position.x - 5, (int)position.y - 5, levelText);
    }

    // 绘制血条
    if (health < maxHealth) {
        int barWidth = 30;
        int barHeight = 4;
        int barX = (int)(position.x - barWidth / 2);
        int barY = (int)(position.y - radius - 8);

        setfillcolor(DARKGRAY);
        solidrectangle(barX, barY, barX + barWidth, barY + barHeight);

        int healthWidth = (barWidth * health) / maxHealth;
        
        COLORREF healthBarColor;
        if (level == 1) {
            healthBarColor = GREEN;
        } else if (level == 2) {
            healthBarColor = RGB(255, 165, 0); // 橙色
        } else if (level == 3) {
            healthBarColor = RGB(255, 0, 255); // 紫色
        } else {
            healthBarColor = RGB(128, 128, 128); // 灰色
        }
        
        setfillcolor(healthBarColor);
        solidrectangle(barX, barY, barX + healthWidth, barY + barHeight);
    }
}