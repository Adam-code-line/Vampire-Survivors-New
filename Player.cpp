#include "Player.h"
#include "ImageManager.h"

Player::Player(Vector2 pos, CharacterType type) 
    : GameObject(pos, 15.0f), characterType(type),
      slashAnimationTimer(0), slashAnimationDuration(0.3f),
      facingRight(true), lastMoveDirection(0, 0),
      // 添加行走动画相关变量初始化
      walkAnimationTimer(0), walkAnimationSpeed(0.15f), currentWalkFrame(0),
      isMoving(false) {
    speed = 150.0f;
    level = 1;
    experience = 0;
    experienceToNext = 100;
    attackCooldown = 1.0f;
    attackTimer = 0;
    health = maxHealth = 100;
    SetCharacterType(type);
    LoadCharacterImage();
}

void Player::LoadCharacterImage() {
    // 根据角色类型设置图片键名
    switch (characterType) {
        case CharacterType::WARRIOR:
            characterImageKey = "warrior";
            break;
        case CharacterType::MAGE:
            characterImageKey = "mage";
            break;
        case CharacterType::ARCHER:
            characterImageKey = "archer";
            break;
        case CharacterType::ASSASSIN:
            characterImageKey = "assassin";
            break;
        default:
            characterImageKey = "warrior";
            break;
    }
    
    // 检查图片是否已加载
    ImageManager* imgManager = ImageManager::GetInstance();
    IMAGE* img = imgManager->GetImage(characterImageKey);
    useImageRendering = (img != nullptr);
}

void Player::SetCharacterType(CharacterType type) {
    characterType = type;
    
    // 设置角色颜色（作为备用方案）
    switch (type) {
        case CharacterType::WARRIOR: 
            characterColor = RED; 
            break;
        case CharacterType::MAGE: 
            characterColor = BLUE; 
            break;
        case CharacterType::ARCHER: 
            characterColor = GREEN; 
            break;
        case CharacterType::ASSASSIN: 
            characterColor = RGB(128, 0, 128); 
            break;
        default:
            characterColor = WHITE;
            break;
    }
    
    // 重新加载角色图片
    LoadCharacterImage();
}

void Player::ApplyCharacterStats(const CharacterData& data) {
    speed = data.speed;
    health = maxHealth = data.health;
    attackCooldown = 1.0f / data.attackSpeed;
}

void Player::Update(float deltaTime) {
    HandleInput(deltaTime);
    attackTimer -= deltaTime;

    // 更新斩击动画计时器
    if (slashAnimationTimer > 0) {
        slashAnimationTimer -= deltaTime;
        if (slashAnimationTimer < 0) {
            slashAnimationTimer = 0;
        }
    }
    
    // 更新行走动画
    UpdateWalkAnimation(deltaTime);
    
    // 边界检测
    position.x = (float)max((float)radius, min((float)(WINDOW_WIDTH - radius), position.x));
    position.y = (float)max((float)radius, min((float)(WINDOW_HEIGHT - radius), position.y));
}

void Player::HandleInput(float deltaTime) {
    Vector2 moveDir(0, 0);

    if (GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP)) moveDir.y -= 1;
    if (GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN)) moveDir.y += 1;
    if (GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT)) moveDir.x -= 1;
    if (GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT)) moveDir.x += 1;

    if (moveDir.Length() > 0) {
        moveDir = moveDir.Normalized();
        position = position + moveDir * speed * deltaTime;
        
        // 更新朝向：只有在水平移动时才改变朝向
        if (moveDir.x != 0) {
            facingRight = moveDir.x > 0;
        }
        
        lastMoveDirection = moveDir;
        isMoving = true;  // 设置移动状态
    } else {
        isMoving = false;  // 停止移动
    }
}

void Player::UpdateWalkAnimation(float deltaTime) {
    if (isMoving) {
        // 更新行走动画计时器
        walkAnimationTimer += deltaTime;
        
        // 切换动画帧
        if (walkAnimationTimer >= walkAnimationSpeed) {
            walkAnimationTimer = 0;
            currentWalkFrame = (currentWalkFrame + 1) % 4; // 4帧动画循环
        }
    } else {
        // 停止移动时重置动画
        walkAnimationTimer = 0;
        currentWalkFrame = 0;
    }
}

void Player::Render() {
    ImageManager* imgManager = ImageManager::GetInstance();
    
    // 检查是否在播放斩击动画
    if (slashAnimationTimer > 0 && characterType == CharacterType::WARRIOR) {
        // 战士斩击动画期间 - 只显示斩击动画，不显示角色模型
        float animationProgress = (slashAnimationDuration - slashAnimationTimer) / slashAnimationDuration;
        int frameIndex = (int)(animationProgress * 3); // 3帧动画
        frameIndex = min(frameIndex, 2); // 确保不超过最大帧数
        
        std::string frameName;
        if (facingRight) {
            // 面向右边使用 _right 图片
            switch (frameIndex) {
                case 0: frameName = "cut1_right"; break;
                case 1: frameName = "cut2_right"; break;
                default: frameName = "cut3_right"; break;
            }
        } else {
            // 面向左边使用普通图片
            switch (frameIndex) {
                case 0: frameName = "cut1"; break;
                case 1: frameName = "cut2"; break;
                default: frameName = "cut3"; break;
            }
        }
        
        // 绘制斩击动画帧 - 以玩家位置为中心
        IMAGE* slashImg = imgManager->GetImage(frameName);
        if (slashImg) {
            int drawX = (int)position.x - slashImg->getwidth() / 2;
            int drawY = (int)position.y - slashImg->getheight() / 2;
            imgManager->DrawImageWithTransparency(slashImg, drawX, drawY);
        } else {
            // 如果斩击图片加载失败，显示一个特殊效果
            setfillcolor(RGB(255, 255, 100)); // 黄色闪光效果
            fillcircle((int)position.x, (int)position.y, 25);
        }
        
        // 斩击动画期间不绘制角色模型，直接跳到血条绘制
    } else {
        // 正常状态 - 显示角色模型（包括行走动画）
        std::string imageKey;
        
        if (isMoving && characterType == CharacterType::WARRIOR) {
            // 播放行走动画
            if (facingRight) {
                // 面向右边的行走动画
                switch (currentWalkFrame) {
                    case 0: imageKey = "move_right1"; break;
                    case 1: imageKey = "move_right2"; break;
                    case 2: imageKey = "move_right3"; break;
                    case 3: imageKey = "move_right4"; break;
                    default: imageKey = "move_right1"; break;
                }
            } else {
                // 面向左边的行走动画
                switch (currentWalkFrame) {
                    case 0: imageKey = "move_left1"; break;
                    case 1: imageKey = "move_left2"; break;
                    case 2: imageKey = "move_left3"; break;
                    case 3: imageKey = "move_left4"; break;
                    default: imageKey = "move_left1"; break;
                }
            }
        } else {
            // 静止状态 - 使用默认角色图片
            if (facingRight) {
                imageKey = characterImageKey + "_right";
            } else {
                imageKey = characterImageKey;
            }
        }
        
        IMAGE* characterImg = imgManager->GetImage(imageKey);
        
        if (characterImg) {
            int drawX = (int)position.x - characterImg->getwidth() / 2;
            int drawY = (int)position.y - characterImg->getheight() / 2;
            imgManager->DrawImageWithTransparency(characterImg, drawX, drawY);
        } else {
            // 备用绘制：如果图片加载失败，使用几何图形
            setfillcolor(characterColor);
            fillcircle((int)position.x, (int)position.y, 15);
        }
    }
    
    // 生命值条始终显示
    DrawHealthBar();
}

void Player::DrawHealthBar() {
    int barWidth = 60;
    int barHeight = 8;
    int barX = (int)(position.x - barWidth / 2);
    int barY = (int)(position.y - radius - 15);

    // 阴影效果
    setfillcolor(RGB(0, 0, 0));
    solidrectangle(barX + 1, barY + 1, barX + barWidth + 1, barY + barHeight + 1);

    // 背景
    setfillcolor(RGB(64, 64, 64));
    solidrectangle(barX, barY, barX + barWidth, barY + barHeight);

    // 血量 - 渐变效果
    int healthWidth = (barWidth * health) / maxHealth;
    if (healthWidth > 0) {
        // 根据血量调整颜色
        COLORREF healthColor;
        float healthPercent = (float)health / maxHealth;
        if (healthPercent > 0.6f) {
            healthColor = RGB(0, 200, 0); // 绿色
        } else if (healthPercent > 0.3f) {
            healthColor = RGB(255, 255, 0); // 黄色
        } else {
            healthColor = RGB(255, 0, 0); // 红色
        }
        
        setfillcolor(healthColor);
        solidrectangle(barX, barY, barX + healthWidth, barY + barHeight);
        
        // 高光效果
        setfillcolor(RGB(255, 255, 255));
        solidrectangle(barX, barY, barX + healthWidth, barY + 2);
    }
    
    // 边框
    setlinecolor(WHITE);
    setlinestyle(PS_SOLID, 1);
    rectangle(barX, barY, barX + barWidth, barY + barHeight);
}

bool Player::CanAttack() const {
    return attackTimer <= 0;
}

void Player::Attack() {
    attackTimer = attackCooldown;
}

void Player::GainExperience(int exp) {
    experience += exp;
    if (experience >= experienceToNext) {
        LevelUp();
    }
}

void Player::LevelUp() {
    level++;
    experience = 0;
    experienceToNext += 20;
    maxHealth += 10;
    health = maxHealth;
}