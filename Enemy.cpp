#include "Enemy.h"
#include "ImageManager.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include "MagicBall.h"  // 确保包含魔法球头文件
#include <cmath>

Enemy::Enemy(Vector2 pos, Player* player, int enemyLevel)
    : GameObject(pos, 15), speed(80.0f), target(player), experienceValue(10), 
      level(enemyLevel), sizeMultiplier(1.0f),
      animationTimer(0), animationSpeed(0.2f), currentFrame(0),
      facingRight(true), isMoving(false), useImageRendering(false),
      // 4级怪物专用变量初始化（简化）
      currentState(Monster4State::MOVING), stateTimer(0),
      attackCooldown(3.0f), attackTimer(0),
      hoverOffset(0), hoverSpeed(2.0f), attackExecuted(false),
      bullets(nullptr), meleeAttacks(nullptr), magicBalls(nullptr) {  // 初始化魔法球容器
    
    // 根据等级调整属性
    SetLevel(level);
    
    // 检查是否有对应的图片资源
    CheckImageResources();
}

void Enemy::SetGameContainers(std::vector<std::unique_ptr<Bullet>>* b, 
                             std::vector<std::unique_ptr<MeleeAttack>>* m,
                             std::vector<std::unique_ptr<MagicBall>>* mb) {
    bullets = b;
    meleeAttacks = m;
    magicBalls = mb;  // 设置魔法球容器
}

void Enemy::SetLevel(int newLevel) {
    level = newLevel;
    
    // 根据等级调整属性
    sizeMultiplier = 1.0f;  // 固定大小，不随等级变化
    radius = 15.0f;         // 固定半径
    
    if (level == 4) {
        // 4级怪物特殊属性（大幅提升难度）
        speed = 70.0f;                    // 提升移动速度（从50.0f提升到70.0f）
        health = maxHealth = 300;         // 大幅提升血量（从100提升到150）
        experienceValue = 50;             // 更多经验（从40提升到50）
		animationSpeed = 0.15f;         // 动画速度（0.15f）
        attackCooldown = 2.5f;            // 增加攻击冷却(从1.5增加到2.5)
        hoverSpeed = 2.5f;                // 更快的浮动速度（从2.0f提升到2.5f）
    } else {
        // 大幅提升其他等级怪物的属性
        speed = 100.0f + (level - 1) * 15.0f;          // 提升基础速度和等级加成（从80+10改为100+15）
        health = maxHealth = 60 + (level - 1) * 40;  // 提升血量（从50+20改为60+40）
        experienceValue = 15 + (level - 1) * 8;        // 提升经验值（从10+5改为15+8）
        
        if (level == 2) {
            animationSpeed = 0.12f;  // 更快动画（从0.15f改为0.12f）
        } else if (level == 3) {
            animationSpeed = 0.10f;  // 更快动画（从0.12f改为0.10f）
        } else {
            animationSpeed = 0.15f;  // 1级怪物也提升动画速度（从0.2f改为0.15f）
        }
    }
    
    CheckImageResources();
}

void Enemy::CheckImageResources() {
    ImageManager* imgManager = ImageManager::GetInstance();
    
    if (level == 1) {
        IMAGE* testImg = imgManager->GetImage("monster1_right1");
        useImageRendering = (testImg != nullptr);
    } else if (level == 2) {
        IMAGE* testImg = imgManager->GetImage("monster2_right1");
        useImageRendering = (testImg != nullptr);
    } else if (level == 3) {
        IMAGE* testImg = imgManager->GetImage("monster3_right1");
        useImageRendering = (testImg != nullptr);
    } else if (level == 4) {
        IMAGE* testImg = imgManager->GetImage("monster4_right1");
        useImageRendering = (testImg != nullptr);
    } else {
        useImageRendering = false;
    }
}

void Enemy::Update(float deltaTime) {
    if (level == 4) {
        UpdateMonster4(deltaTime);
    } else {
        // 其他等级怪物的原有逻辑
        Vector2 oldPosition = position;
        
        if (target && target->active) {
            Vector2 direction = (target->position - position).Normalized();
            position = position + direction * speed * deltaTime;
            
            if (abs(direction.x) > 0.1f) {
                facingRight = direction.x > 0;
            }
            
            isMoving = (position - oldPosition).Length() > 0.1f;
        } else {
            isMoving = false;
        }
        
        UpdateAnimation(deltaTime);
    }
}

void Enemy::UpdateMonster4(float deltaTime) {
    // 更新计时器
    stateTimer += deltaTime;
    attackTimer -= deltaTime;
    
    // 更新浮动效果（简化）
    hoverOffset = sin(GetTickCount() * 0.003f * hoverSpeed) * 5.0f;
    
    // 更新状态机
    UpdateMonster4State(deltaTime);
    
    // 根据状态执行不同逻辑
    switch (currentState) {
        case Monster4State::MOVING:
            if (target && target->active) {
                Vector2 direction = (target->position - position).Normalized();
                position = position + direction * speed * deltaTime;
                
                if (abs(direction.x) > 0.1f) {
                    facingRight = direction.x > 0;
                }
                isMoving = true;
            } else {
                isMoving = false;
            }
            break;
            
        case Monster4State::MELEE_ATTACK:
        case Monster4State::RANGE_ATTACK:
            // 攻击状态下不移动
            isMoving = false;
            break;
    }
    
    // 更新动画
    UpdateAnimation(deltaTime);
}

void Enemy::UpdateMonster4State(float deltaTime) {
    if (!target || !target->active) return;
    
    float distanceToPlayer = position.Distance(target->position);
    
    switch (currentState) {
        case Monster4State::MOVING:
            // 检查是否可以攻击 - 增加攻击范围和触发频率
       if (distanceToPlayer <= 350.0f && attackTimer <= 0) {
                // 中远距离 - 远程攻击（范围从300增加到350）
                currentState = Monster4State::RANGE_ATTACK;
                stateTimer = 0;
                attackTimer = attackCooldown;
                attackExecuted = false;
            }
            break;

        case Monster4State::RANGE_ATTACK:
            // 缩短远程攻击蓄力时间：0.3秒后执行攻击，0.7秒后结束（更快）
            if (!attackExecuted && stateTimer >= 0.3f) {
                PerformRangeAttack();
                attackExecuted = true;
            }
            if (stateTimer >= 0.7f) {
                currentState = Monster4State::MOVING;
                stateTimer = 0;
            }
            break;
    }
}

void Enemy::PerformMeleeAttack() {
    if (!meleeAttacks || !target) return;
    
    // 创建更强的近战攻击
    Vector2 direction = (target->position - position).Normalized();
    float angle = atan2(direction.y, direction.x);
    
    auto meleeAttack = std::make_unique<MeleeAttack>(position, nullptr, angle, 85.0f); // 增加攻击范围（从70.0f增加到85.0f）
    // 如果MeleeAttack有SetDamage方法，提升伤害
    meleeAttack->SetDamage(45); // 从30提升到45
    meleeAttacks->push_back(std::move(meleeAttack));
}

void Enemy::PerformRangeAttack() {
    if (!magicBalls || !target) return;
    
    // 发射更快更强的魔法球
    Vector2 direction = (target->position - position).Normalized();
    auto magicBall = std::make_unique<MagicBall>(position, direction, 200.0f); // 魔法球速度（200.0ff）
    magicBall->SetDamage(40); // 魔法球伤害（40）
    magicBalls->push_back(std::move(magicBall));
}

void Enemy::UpdateAnimation(float deltaTime) {
    if (level == 4) {
        // 4级怪物特殊动画逻辑（移除技能相关）
        animationTimer += deltaTime;
        
        if (animationTimer >= animationSpeed) {
            animationTimer = 0;
            
            switch (currentState) {
                case Monster4State::MOVING:
                    // 移动状态：只使用 monster4_right1 和 monster4_left1 做上下浮动
                    currentFrame = (currentFrame + 1) % 2;
                    break;
                    
                case Monster4State::MELEE_ATTACK:
                    // 近战攻击：使用 monster4_right2 和 monster4_left2
                    currentFrame = 1; // 固定使用第2帧
                    break;
                    
                case Monster4State::RANGE_ATTACK:
                    // 远程攻击：使用 monster4_right3 和 monster4_left3
                    currentFrame = 2; // 固定使用第3帧
                    break;
            }
        }
    } else if (isMoving && (level == 1 || level == 2 || level == 3)) {
        // 其他等级怪物的原有动画逻辑
        animationTimer += deltaTime;
        
        if (animationTimer >= animationSpeed) {
            animationTimer = 0;
            
            if (level == 1) {
                currentFrame = (currentFrame + 1) % 3;
            } else if (level == 2) {
                currentFrame = (currentFrame + 1) % 7;
            } else if (level == 3) {
                currentFrame = (currentFrame + 1) % 4;
            }
        }
    } else {
        animationTimer = 0;
        currentFrame = 0;
    }
}

void Enemy::Render() {
    if (useImageRendering && (level == 1 || level == 2 || level == 3 || level == 4)) {
        if (level == 4) {
            RenderMonster4();
        } else {
            RenderWithImage();
        }
    } else {
        RenderWithGeometry();
    }
}

void Enemy::RenderMonster4() {
    ImageManager* imgManager = ImageManager::GetInstance();
    std::string imageKey;
    
    // 简化图片选择逻辑
    switch (currentState) {
        case Monster4State::MOVING:
            imageKey = facingRight ? "monster4_right1" : "monster4_left1";
            break;
        case Monster4State::MELEE_ATTACK:
            imageKey = facingRight ? "monster4_right2" : "monster4_left2";
            break;
        case Monster4State::RANGE_ATTACK:
            imageKey = facingRight ? "monster4_right3" : "monster4_left3";
            break;
    }
    
    // 检查左侧图片是否存在，不存在则使用右侧图片
    IMAGE* monsterImg = imgManager->GetImage(imageKey);
    if (!monsterImg && !facingRight) {
        std::string rightKey = imageKey;
        size_t pos = rightKey.find("_left");
        if (pos != std::string::npos) {
            rightKey.replace(pos, 5, "_right");
            monsterImg = imgManager->GetImage(rightKey);
        }
    }
    
    if (monsterImg && monsterImg->getwidth() > 0 && monsterImg->getheight() > 0) {
        // 应用浮动效果
        float drawY = position.y + hoverOffset;
        
        int drawX = (int)position.x - monsterImg->getwidth() / 2;
        int finalDrawY = (int)drawY - monsterImg->getheight() / 2;
        
        // 使用透明绘制，像其他怪物一样处理
        imgManager->DrawImageWithTransparency(monsterImg, drawX, finalDrawY);
        
    } else {
        // 备用方案
        useImageRendering = false;
        RenderWithGeometry();
        return;
    }
    
    // 绘制血条（如果血量不满） - 使用与其他怪物相同的样式
    if (health < maxHealth) {
        int barWidth = 30;  // 改为与其他怪物一致的宽度
        int barHeight = 4;  // 改为与其他怪物一致的高度
        int barX = (int)(position.x - barWidth / 2);
        int barY = (int)(position.y + hoverOffset - radius - 8); // 保持浮动效果但使用一致的位置

        // 背景
        setfillcolor(RGB(64, 64, 64));
        solidrectangle(barX, barY, barX + barWidth, barY + barHeight);

        // 血量 - 4级怪物使用深紫色血条
        int healthWidth = (barWidth * health) / maxHealth;
        setfillcolor(RGB(128, 0, 255)); // 深紫色血条，与其他等级保持一致的颜色系统
        solidrectangle(barX, barY, barX + healthWidth, barY + barHeight);
        
        // 边框
        setlinecolor(WHITE);
        rectangle(barX, barY, barX + barWidth, barY + barHeight);
    }
    
    // 移除状态文字显示
    // 注释掉原来的状态文本代码
    /*
    TCHAR stateText[20];
    switch (currentState) {
        case Monster4State::MOVING:
            _tcscpy_s(stateText, _T("移动"));
            break;
        case Monster4State::MELEE_ATTACK:
            _tcscpy_s(stateText, _T("近战"));
            break;
        case Monster4State::RANGE_ATTACK:
            _tcscpy_s(stateText, _T("远程"));
            break;
    }
    
    settextcolor(RGB(255, 255, 0));
    settextstyle(10, 0, _T("SimHei"));
    outtextxy((int)position.x - 10, (int)(position.y + hoverOffset + radius + 10), stateText);
    */
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
        } else if (level == 4) {
            healthColor = RGB(128, 0, 255);   // 深紫色血条
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
            enemyColor = RGB(128, 0, 255);  // 深紫色（与血条颜色一致）
            break;      
        default: 
            enemyColor = RGB(50, 50, 50);   // 深灰色
            break;      
    }
    
    // 4级怪物的浮动效果
    float drawY = (level == 4) ? position.y + hoverOffset : position.y;
    
    // 绘制敌人主体
    setfillcolor(enemyColor);
    solidcircle((int)position.x, (int)drawY, (int)radius);
    
    // 添加边框
    setlinecolor(WHITE);
    circle((int)position.x, (int)drawY, (int)radius);
    
    // 4级怪物特殊效果
    if (level == 4) {
        switch (currentState) {
            case Monster4State::MELEE_ATTACK:
                // 近战状态 - 红色光环
                setlinecolor(RGB(255, 100, 100));
                setlinestyle(PS_SOLID, 2);
                circle((int)position.x, (int)drawY, (int)(radius + 5));
                break;
                
            case Monster4State::RANGE_ATTACK:
                // 远程状态 - 蓝色光环
                setlinecolor(RGB(100, 150, 255));
                setlinestyle(PS_SOLID, 2);
                circle((int)position.x, (int)drawY, (int)(radius + 8));
                break;
        }
    }
    
    // 添加其他等级的动画效果...（保持原有逻辑）
    
    // 绘制等级标识
    if (level > 1) {
        settextcolor(WHITE);
        settextstyle(12, 0, _T("Arial"));
        TCHAR levelText[10];
        _stprintf_s(levelText, _T("%d"), level);
        outtextxy((int)position.x - 5, (int)drawY - 5, levelText);
    }

    // 绘制血条
    if (health < maxHealth) {
        int barWidth = 30;
        int barHeight = 4;
        int barX = (int)(position.x - barWidth / 2);
        int barY = (int)(drawY - radius - 8); // 使用浮动后的Y坐标

        setfillcolor(RGB(64, 64, 64));
        solidrectangle(barX, barY, barX + barWidth, barY + barHeight);

        int healthWidth = (barWidth * health) / maxHealth;
        
        COLORREF healthBarColor;
        if (level == 1) {
            healthBarColor = RGB(255, 100, 100); // 红色
        } else if (level == 2) {
            healthBarColor = RGB(255, 150, 0);   // 橙色
        } else if (level == 3) {
            healthBarColor = RGB(255, 0, 255);   // 紫色
        } else if (level == 4) {
            healthBarColor = RGB(128, 0, 255);   // 深紫色
        } else {
            healthBarColor = RGB(150, 150, 150); // 灰色
        }
        
        setfillcolor(healthBarColor);
        solidrectangle(barX, barY, barX + healthWidth, barY + barHeight);
        
        setlinecolor(WHITE);
        rectangle(barX, barY, barX + barWidth, barY + barHeight);
    }
}