#pragma once

// 游戏窗口配置
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

// 数学常量
#ifndef PI
#define PI 3.14159265359f
#endif

// 游戏配置
const float PLAYER_SPEED = 200.0f;
const int INITIAL_HEALTH = 100;
const float EXPERIENCE_SCALE = 100.0f;

// 资源路径
const char* const PLAYER_TEXTURE_PATH = "assets/player.png";
const char* const BULLET_TEXTURE_PATH = "assets/bullet.png";
const char* const ENEMY_TEXTURE_PATH = "assets/enemy.png";

// 声音路径
const char* const SHOOT_SOUND_PATH = "assets/sounds/shoot.wav";
const char* const EXPLOSION_SOUND_PATH = "assets/sounds/explosion.wav";

// 其他常量
const int MAX_LEVEL = 10;
const int POINTS_PER_ENEMY = 100;
const int POINTS_PER_BULLET = 10;