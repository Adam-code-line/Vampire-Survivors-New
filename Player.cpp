#include "Player.h"
#include "ImageManager.h"

Player::Player(Vector2 pos, CharacterType type) : GameObject(pos, 20), speed(150.0f), level(1),
    experience(0), experienceToNext(100),
    attackCooldown(1.0f), attackTimer(0), characterType(type),
    useImageRendering(false) {
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
    }
}

void Player::Render() {
    if (useImageRendering) {
        // 使用图片渲染
        ImageManager* imgManager = ImageManager::GetInstance();
        IMAGE* characterImage = imgManager->GetImage(characterImageKey);
        
        if (characterImage) {
            int imageWidth = characterImage->getwidth();
            int imageHeight = characterImage->getheight();
            
            // 计算绘制位置（以角色中心为基准）
            int drawX = (int)position.x - imageWidth / 2;
            int drawY = (int)position.y - imageHeight / 2;
            
            // 简单直接绘制，不处理透明
            putimage(drawX, drawY, characterImage);
            
        } else {
            // 图片加载失败，使用备用渲染
            useImageRendering = false;
        }
    }
    
    if (!useImageRendering) {
        // 备用渲染方案：使用原来的圆形
        setfillcolor(characterColor);
        solidcircle((int)position.x, (int)position.y, (int)radius);
        
        // 绘制边框
        setlinecolor(WHITE);
        circle((int)position.x, (int)position.y, (int)radius);
    }

    // 绘制血条
    DrawHealthBar();
}

void Player::DrawHealthBar() {
    int barWidth = 60;
    int barHeight = 8;
    int barX = (int)(position.x - barWidth / 2);
    int barY = (int)(position.y - radius - 15);

    // 背景
    setfillcolor(DARKGRAY);
    solidrectangle(barX, barY, barX + barWidth, barY + barHeight);

    // 血量
    int healthWidth = (barWidth * health) / maxHealth;
    setfillcolor(RED);
    solidrectangle(barX, barY, barX + healthWidth, barY + barHeight);
    
    // 边框
    setlinecolor(WHITE);
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