#include "MusicManager.h"
#include <iostream>

MusicManager* MusicManager::instance = nullptr;

MusicManager::MusicManager() : musicEnabled(true), currentMusic("") {
}

MusicManager* MusicManager::GetInstance() {
    if (instance == nullptr) {
        instance = new MusicManager();
    }
    return instance;
}

MusicManager::~MusicManager() {
    StopMusic();
}

bool MusicManager::PlayBackgroundMusic(const std::string& filename) {
    if (!musicEnabled) return false;
    
    // 先停止当前音乐
    StopMusic();
    
    // 转换为宽字符路径
    std::wstring wFilename(filename.begin(), filename.end());
    
    // 使用MCI命令播放音乐（循环播放）
    std::wstring command = L"open \"" + wFilename + L"\" type mpegvideo alias bgmusic";
    
    MCIERROR result = mciSendString(command.c_str(), NULL, 0, NULL);
    if (result != 0) {
        // 尝试作为WAV文件打开
        command = L"open \"" + wFilename + L"\" type waveaudio alias bgmusic";
        result = mciSendString(command.c_str(), NULL, 0, NULL);
    }
    
    if (result == 0) {
        // 设置循环播放
        mciSendString(L"play bgmusic repeat", NULL, 0, NULL);
        currentMusic = filename;
        return true;
    } else {
        WCHAR errorMsg[256];
        mciGetErrorString(result, errorMsg, 256);
        // 修复：改用标准输出，避免宽字符问题
        printf("无法播放音乐，错误代码：%lu\n", (unsigned long)result);
        return false;
    }
}

void MusicManager::StopMusic() {
    if (!currentMusic.empty()) {
        mciSendString(L"stop bgmusic", NULL, 0, NULL);
        mciSendString(L"close bgmusic", NULL, 0, NULL);
        currentMusic = "";
    }
}

void MusicManager::PauseMusic() {
    if (!currentMusic.empty()) {
        mciSendString(L"pause bgmusic", NULL, 0, NULL);
    }
}

void MusicManager::ResumeMusic() {
    if (!currentMusic.empty()) {
        mciSendString(L"resume bgmusic", NULL, 0, NULL);
    }
}

void MusicManager::SetVolume(int volume) {
    if (!currentMusic.empty()) {
        WCHAR volumeCmd[100];
        swprintf_s(volumeCmd, 100, L"setaudio bgmusic volume to %d", volume);
        mciSendString(volumeCmd, NULL, 0, NULL);
    }
}

void MusicManager::SetMusicEnabled(bool enabled) {
    musicEnabled = enabled;
    if (!enabled) {
        StopMusic();
    }
}

bool MusicManager::IsPlaying() {
    if (currentMusic.empty()) return false;
    
    WCHAR status[128];
    mciSendString(L"status bgmusic mode", status, 128, NULL);
    
    return (wcscmp(status, L"playing") == 0);
}

// 新增：加载游戏音乐的方法
void MusicManager::LoadGameMusic() {
    printf("正在加载游戏音乐...\n");
    
    if (PlayBackgroundMusic("mus\\music1.mp3")) {
        printf("背景音乐 music1.mp3 加载成功！\n");
    } else if (PlayBackgroundMusic("mus\\music1.wav")) {
        printf("背景音乐 music1.wav 加载成功！\n");
    } else {
        printf("警告: 无法加载背景音乐文件\n");
    }
    
    // 设置合适的音量（可选）
    SetVolume(500); // 50% 音量
}

// 修复：播放音效方法
void MusicManager::PlaySoundEffect(const std::string& filename) {
    if (!musicEnabled) return;
    
    // 转换为宽字符路径
    std::wstring wFilename(filename.begin(), filename.end());
    
    // 生成唯一的音效别名
    static int soundId = 0;
    soundId++;
    WCHAR alias[50];
    swprintf_s(alias, 50, L"sound%d", soundId);
    
    // 打开音效文件
    std::wstring command = L"open \"" + wFilename + L"\" type mpegvideo alias ";
    command += alias;
    
    MCIERROR result = mciSendString(command.c_str(), NULL, 0, NULL);
    
    if (result != 0) {
        // 尝试作为WAV文件打开
        command = L"open \"" + wFilename + L"\" type waveaudio alias ";
        command += alias;
        result = mciSendString(command.c_str(), NULL, 0, NULL);
    }
    
    if (result == 0) {
        // 播放音效（不循环）
        command = L"play ";
        command += alias;
        mciSendString(command.c_str(), NULL, 0, NULL);
        
        // 创建一个异步任务来关闭音效（简化版本）
        // 注意：在实际项目中，可能需要更复杂的资源管理
    } else {
        printf("无法播放音效: %s\n", filename.c_str());
    }
}

// 新增：播放剑气音效
void MusicManager::PlaySwordAuraSound() {
    PlaySoundEffect("mus\\player_attack_1.mp3");
}