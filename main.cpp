#include <graphics.h>
#include <vector>
#include <memory>
#include <random>
#include <cmath>
#include <algorithm>
#include <conio.h>      
#include <windows.h>    
#include <tchar.h>      

#include "Vector2.h"
#include "GameConfig.h"
#include "GameObject.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "ExperienceGem.h"
#include "WeaponSystem.h"
#include "EnemySpawner.h"
#include "MeleeAttack.h"
#include "SurvivorGame.h"
#include "EnhancedWeaponSystem.h"

int main() {
    // 设置中文编码支持
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 设置Windows系统编码
    system("chcp 65001");

    SurvivorGame game;
    game.Run();
    return 0;
}