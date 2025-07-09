#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "winmm.lib")

class MusicManager {
private:
    static MusicManager* instance;
    bool musicEnabled;
    std::string currentMusic;
    
    MusicManager();
    
public:
    static MusicManager* GetInstance();
    ~MusicManager();
    
    // 播放背景音乐（循环）
    bool PlayBackgroundMusic(const std::string& filename);
    
    // 停止音乐
    void StopMusic();
    
    // 暂停/恢复音乐
    void PauseMusic();
    void ResumeMusic();
    
    // 设置音量（0-1000）
    void SetVolume(int volume);
    
    // 开启/关闭音乐
    void SetMusicEnabled(bool enabled);
    bool IsMusicEnabled() const { return musicEnabled; }
    
    // 检查音乐是否正在播放
    bool IsPlaying();
    
    // 新增：加载游戏音乐
    void LoadGameMusic();
    
    // 新增：播放音效
    void PlaySoundEffect(const std::string& filename);
    void PlaySwordAuraSound(); // 播放剑气音效
};
