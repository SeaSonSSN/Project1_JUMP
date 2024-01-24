#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#undef main

void QUIT();                           // 清除加载并退出
void LOAD();                           // 加载图片
void Play_Start();                     // 开始游戏
void Playing();                        // 游戏中
void add_box(double angle);            // 随机加载平台
void cartoon(int is_press, int temp);  // 表示按时常的动画
int jump(int interval);                // 主要功能
void move(int distance, double angle); // 保持人物在中心位置
int judge();                           // 判断输赢
void PrintCount();                     // 打印分数
void lose();                           // 游戏结束
void restart();                        // 重开
void Return_Main();                    // 回到首页
int Auto_Mode();                       // 自动跳

SDL_Window *Window = NULL;                 // 窗口
SDL_Renderer *Renderer = NULL;             // 渲染器
SDL_Surface *MainBackGroundSurface = NULL; // 开始界面
SDL_Surface *PlayBackGroundSurface = NULL; // 游戏背景界面
SDL_Surface *PlatformSurface[6] = {NULL};  // 平台表面
SDL_Surface *PersonSurface = NULL;         // 人物表面
SDL_Surface *ScoreSurface = NULL;          // 字体表面
SDL_Surface *GameOverSurface = NULL;       // 结束界面表面
SDL_Rect MainBackGroundRect;               // 开始界面的矩形区域
SDL_Rect GameOverRect;                     // 结束界面的矩形区域
SDL_Rect PlatformRect;                     // 平台的矩形区域
SDL_Rect PersonRect;                       // 人物的矩形区域
SDL_Rect GameOverRect;                     // 结束界面的矩形区域
SDL_Rect SaveRect;                         // 目前背景的矩形区域
SDL_Rect ScoreRect;                        // 字体的矩形区域
TTF_Font *ScoreFont = NULL;                // 字体
SDL_Color FontColor = {0, 0, 0, 255};      // 字体颜色为白色
SDL_Event MainEvent;                       // 开始事件
SDL_Event PlayEvent;                       // 游戏事件
SDL_Texture *SaveTexture = NULL;           // 目前背景的纹理
SDL_Texture *PlayBackGroundTexture = NULL; // 游戏背景界面的纹理
SDL_Texture *ScoreTexture = NULL;          // 字体的纹理

#define MOVE_SPEED 300 // 移动速度，单位为像素/秒
#define PI 3.1415926   // 定义Pi

int is_begin = 0;                       // 判断是否开始
int fall = 0;                           // 游戏结束
int last_space_time = 0;                // 按下空格时间
int space_up_time = 0;                  // 放开空格时间
int interval = 0;                       // 放开按下空格时间间隔
int character_position[2] = {360, 415}; // 人物开始的位置
int Platform_position[2] = {480, 310};  // 平台初始的位置
int exPlatform_position[2] = {0, 0};    // 前一个平台的位置
int Save_position[2] = {0, 0};          // 界面开始的位置
int Platform_size[2] = {196, 141};      // 平台开始的大小，不断变小
int front_or_left = 1;                  // 前方生成，还是左边生成
int exf_or_l = 1;                       // 前一个是前方生成，还是左边生成
int RandomCreate = 2;                   // 随机生成的平台
int Score = 0;                          // 目前分数
char ScoreChar[15] = {0};               // 分数的字符串
int the_best_score = 0;                 // 最好成绩
int height = 130;                       // 人物的高度