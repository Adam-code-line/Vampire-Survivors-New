# Vampire Survivors Clone

这是一个使用 C++ 和 EasyX 图形库开发的类吸血鬼幸存者游戏。项目目前处于初步开发阶段，包含以下功能：

## 游戏特色
- **角色系统**: 当前仅实现了战士角色，支持基础攻击和技能。
- **怪物系统**: 包括多种等级的怪物，具有不同的属性和行为。
- **武器系统**: 支持远程攻击、近战攻击和特殊技能。
- **道具系统**: 玩家可以拾取道具以获得增益效果。
- **经验与升级**: 击败敌人可获得经验，角色升级后属性增强。
- **音乐与音效**: 游戏内包含背景音乐和音效，支持动态切换。

## 使用的技术
- **编程语言**: C++
- **图形库**: EasyX
- **音频处理**: 使用 Windows MCI 命令播放音乐和音效

## 文件结构
```
├── Bullet.cpp / Bullet.h          # 子弹相关逻辑
├── Character.h                    # 角色数据定义
├── Enemy.cpp / Enemy.h            # 怪物相关逻辑
├── EnhancedWeaponSystem.cpp / .h  # 武器系统
├── ExperienceGem.cpp / .h         # 经验宝石
├── GameConfig.h                   # 游戏配置
├── GameObject.h                   # 游戏对象基类
├── ImageManager.cpp / .h          # 图片管理
├── Item2.cpp / Item2.h            # 道具系统
├── MainMenu.cpp / MainMenu.h      # 主菜单
├── MusicManager.cpp / .h          # 音乐管理
├── Player.cpp / Player.h          # 玩家角色
├── SkillSystem.cpp / SkillSystem.h# 技能系统
├── StartScreen.cpp / .h           # 开始界面
├── SurvivorGame.cpp / .h          # 游戏主逻辑
├── main.cpp                       # 程序入口
└── README.md                      # 项目说明
```

## 游戏截图
![游戏截图]......

## 如何运行
1. 确保已安装 Visual Studio 或其他支持 C++ 的 IDE。
2. 克隆项目到本地：
   ```bash
   git clone https://github.com/Adam-code-line/VampireSurvivorsClone.git
   ```
3. 打开项目并编译运行。

## 已知问题
- 目前仅实现了战士角色，其他角色尚未开发。
- 游戏内容较为简单，缺乏完整的关卡设计。
- 音效和音乐可能存在兼容性问题。

## 未来计划
- 添加更多角色和技能。
- 增加关卡和敌人种类。
- 优化图形效果和性能。
- 提供更丰富的道具和升级系统。

## 贡献
欢迎提交 Issue 或 Pull Request 来帮助完善项目！

## 开发者
- **开发者**: [Adam-code-line](https://github.com/Adam-code-line)
- **邮箱**: ......

## 许可证
本项目使用 [MIT License](LICENSE) 进行授权。