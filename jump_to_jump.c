#include "jump_to_jump.h"

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Window = SDL_CreateWindow("jump to jump", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 700, SDL_WINDOW_SHOWN); // 创建窗口
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);                                                    // 创建渲染器
    srand((unsigned)time(NULL));                                                                                            // 随机数种子                                                                             // 初始化随机数
    LOAD();
    Return_Main();
    printf("MainEvent\n");
    while (SDL_WaitEvent(&MainEvent))
    {
        switch (MainEvent.type)
        {
        case SDL_QUIT: // 退出键
            QUIT();
            return 0;
        case SDL_KEYDOWN:
            switch (MainEvent.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                QUIT();
                return 0;
            case SDLK_RETURN: // Enter
                is_begin = 1;
                Playing();
                Return_Main();
                printf("MainEvent\n");
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (MainEvent.button.x > 267 && MainEvent.button.x < 727 && MainEvent.button.y > 485 && MainEvent.button.y < 535)
            {
                is_begin = 1;
                Playing();
                Return_Main();
                printf("MainEvent\n");
            }
            printf("%d %d\n", MainEvent.button.x, MainEvent.button.y);
            break;
        case SDL_MOUSEMOTION: // 鼠标移动
            break;
        default:
            break;
        }
    }

    QUIT();
    return 0;
}

void Playing()
{
    Play_Start();
    printf("Playing\n");
    int fail = 0;     // 是否输了
    int is_press = 1; // 是否按下空格
    while (SDL_WaitEvent(&PlayEvent))
    {
        height = 130;
        int temp = character_position[1];
        switch (PlayEvent.type)
        {
        case SDL_QUIT: // 退出键
            return;
        case SDL_KEYDOWN:
            switch (PlayEvent.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                return;
            case SDLK_SPACE:              // 空格
                if (last_space_time == 0) // 只在第一次按下空格键时更新 last_space_time
                {
                    is_press = 0;
                    last_space_time = SDL_GetTicks();
                    while (1)
                    {
                        cartoon(is_press, temp);
                        temp++;
                        // 检测空格键是否放开
                        if (SDL_PollEvent(&PlayEvent) && PlayEvent.type == SDL_KEYUP && PlayEvent.key.keysym.sym == SDLK_SPACE)
                        {
                            is_press = 1;
                            space_up_time = SDL_GetTicks();
                            interval = space_up_time - last_space_time;
                            if (fail == 0)
                            {
                                space_up_time = SDL_GetTicks(); // 放开空格键的时间点
                                // printf("last_space_time is %d\n", last_space_time);
                                // printf("space_up_time is %d\n", space_up_time);
                                if (last_space_time != 0)
                                    interval = space_up_time - last_space_time; // 计算按下和放开空格键的时间间隔
                                fail = jump(interval);
                                last_space_time = 0;
                                if (fail == 1) // 输了
                                    lose();
                            }
                            break; // 跳出循环
                        }
                    }
                }
                break;
            case SDLK_RETURN:
                if (fail == 1)
                    restart();
                break;
            case SDLK_a:
                if (fail == 0)
                {
                    int r = Auto_Mode();
                    fail = jump(r);
                    if (fail == 1) // 输了
                        lose();
                    SDL_Delay(5);
                }
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (fail == 1)
            {
                if (PlayEvent.button.x > 438 && PlayEvent.button.x < 762 && PlayEvent.button.y > 430 && PlayEvent.button.y < 515)
                    restart();
                if (PlayEvent.button.x > 331 && PlayEvent.button.x < 417 && PlayEvent.button.y > 435 && PlayEvent.button.y < 510)
                    return;
            }
            printf("%d %d\n", PlayEvent.button.x, PlayEvent.button.y);
            break;
        case SDL_MOUSEMOTION: // 鼠标移动
            break;
        default:
            break;
        }
    }
}

int Auto_Mode()
{
    int center[2];
    center[0] = Platform_position[0] + 83 - (196 - Platform_size[0]);
    center[1] = Platform_position[1] - 35 - (141 - Platform_size[1]);
    int x = character_position[0] - center[0];      // 水平方向相差的距离
    int y = character_position[0] - center[0];      // 垂直方向相差的距离
    int r = sqrt(x * x + y * y) / 300 * 1000 - 150; // 符合jump函数的interval
    return r;
}

void cartoon(int is_press, int temp)
{
    if (height <= 80)
        return;
    if (is_press == 0) // 一直按空格
    {
        SDL_Texture *PersonTexture = NULL;
        PersonTexture = SDL_CreateTextureFromSurface(Renderer, PersonSurface);
        SDL_RenderClear(Renderer);
        SaveRect = (SDL_Rect){Save_position[0], Save_position[1], 1000, 700};
        SDL_RenderCopy(Renderer, SaveTexture, NULL, &SaveRect);                   // 箱子
        PersonRect = (SDL_Rect){character_position[0], temp + 1, 80, height - 1}; // 更新距离
        SDL_RenderCopy(Renderer, PersonTexture, NULL, &PersonRect);
        SDL_RenderPresent(Renderer);
        PrintCount();
        height--;
        SDL_DestroyTexture(PersonTexture);
        SDL_Delay(20);
    }
}

int jump(int interval)
{
    int distance = interval * MOVE_SPEED / 1000; // 移动距离
    double angle = atan(3.5 / 5);
    SDL_Texture *PersonTexture = NULL;
    PersonTexture = SDL_CreateTextureFromSurface(Renderer, PersonSurface);
    SDL_RenderClear(Renderer);
    int excenter[2] = {0};
    excenter[0] = exPlatform_position[0] + 83 - (196 - Platform_size[0]);
    excenter[1] = exPlatform_position[1] - 35 - (141 - Platform_size[1]);
    double i = 0, arc_height = 100;
    printf("%d\n", distance);
    if (exf_or_l == 1 && front_or_left == 2)
    {
        for (i = 0; i < distance; i++)
        {
            double rotate = i / distance * 360;
            double value = arc_height * sin((i / distance) * PI);
            SaveRect = (SDL_Rect){Save_position[0], Save_position[1], 1000, 700};
            SDL_RenderCopy(Renderer, SaveTexture, NULL, &SaveRect);                                                   // 箱子
            PersonRect = (SDL_Rect){excenter[0] - (i * cos(angle)), excenter[1] - (i * sin(angle)) - value, 80, 130}; // 更新距离
            SDL_RenderCopyEx(Renderer, PersonTexture, NULL, &PersonRect, -rotate, NULL, SDL_FLIP_NONE);
            SDL_RenderPresent(Renderer);
        }
        // printf("1\n");
        character_position[0] = excenter[0] - (i * cos(angle));
        character_position[1] = excenter[1] - (i * sin(angle));
        if (judge(distance) == 1)
        {
            SDL_Delay(200);
            PrintCount();
            return 1; // 输了
        }
    }
    else if (exf_or_l == 2 && front_or_left == 1)
    {
        for (i = 0; i < distance; i++)
        {
            double rotate = i / distance * 360;
            double value = arc_height * sin((i / distance) * PI);
            SaveRect = (SDL_Rect){Save_position[0], Save_position[1], 1000, 700};
            SDL_RenderCopy(Renderer, SaveTexture, NULL, &SaveRect);                                                   // 箱子
            PersonRect = (SDL_Rect){excenter[0] + (i * cos(angle)), excenter[1] - (i * sin(angle)) - value, 80, 130}; // 更新距离
            SDL_RenderCopyEx(Renderer, PersonTexture, NULL, &PersonRect, rotate, NULL, SDL_FLIP_NONE);
            SDL_RenderPresent(Renderer);
        }
        // printf("2\n");
        character_position[0] = excenter[0] + (i * cos(angle));
        character_position[1] = excenter[1] - (i * sin(angle));
        if (judge(distance) == 1)
        {
            SDL_Delay(200);
            PrintCount();
            return 1; // 输了
        }
    }
    else if (front_or_left == 1)
    {
        for (i = 0; i < distance; i++)
        {
            double rotate = i / distance * 360;
            double value = arc_height * sin((i / distance) * PI);
            SaveRect = (SDL_Rect){Save_position[0], Save_position[1], 1000, 700};
            SDL_RenderCopy(Renderer, SaveTexture, NULL, &SaveRect);                                                                       // 箱子
            PersonRect = (SDL_Rect){character_position[0] + (i * cos(angle)), character_position[1] - (i * sin(angle)) - value, 80, 130}; // 更新距离
            SDL_RenderCopyEx(Renderer, PersonTexture, NULL, &PersonRect, rotate, NULL, SDL_FLIP_NONE);
            SDL_RenderPresent(Renderer);
        }
        // printf("3\n");
        character_position[0] = character_position[0] + (i * cos(angle));
        character_position[1] = character_position[1] - (i * sin(angle));
        if (judge(distance) == 1)
        {
            SDL_Delay(200);
            PrintCount();
            return 1; // 输了
        }
    }
    else if (front_or_left == 2)
    {
        for (i = 0; i < distance; i++)
        {
            double rotate = i / distance * 360;
            double value = arc_height * sin((i / distance) * PI);
            SaveRect = (SDL_Rect){Save_position[0], Save_position[1], 1000, 700};
            SDL_RenderCopy(Renderer, SaveTexture, NULL, &SaveRect);                                                                       // 箱子
            PersonRect = (SDL_Rect){character_position[0] - (i * cos(angle)), character_position[1] - (i * sin(angle)) - value, 80, 130}; // 更新距离
            SDL_RenderCopyEx(Renderer, PersonTexture, NULL, &PersonRect, -rotate, NULL, SDL_FLIP_NONE);
            SDL_RenderPresent(Renderer);
        }
        // printf("4\n");
        character_position[0] = character_position[0] - (i * cos(angle));
        character_position[1] = character_position[1] - (i * sin(angle));
        if (judge(distance) == 1)
        {
            SDL_Delay(200);
            PrintCount();
            return 1; // 输了
        }
    }
    SDL_DestroyTexture(PersonTexture);
    Score++; // 过了加分
    if (RandomCreate == 5)
        Score += 2;
    SDL_Delay(500);
    move(distance, angle);
    PrintCount();
    SDL_Delay(300);
    exf_or_l = front_or_left;
    front_or_left = rand() % 2 + 1; // 1是在前方生成，2是左方
    add_box(angle);
    SDL_Delay(300);
    return 0; // 游戏继续
}

void lose()
{
    if (Score > the_best_score)
        the_best_score = Score;
    SDL_Texture *GameOverTexture = NULL;
    GameOverTexture = SDL_CreateTextureFromSurface(Renderer, GameOverSurface);
    GameOverRect = (SDL_Rect){270, 250, 500, 300};
    SDL_SetTextureAlphaMod(GameOverTexture, 200);
    SDL_RenderCopy(Renderer, GameOverTexture, NULL, &GameOverRect);
    SDL_RenderPresent(Renderer);
    SDL_DestroyTexture(GameOverTexture);
    sprintf(ScoreChar, "%d", the_best_score);
    ScoreSurface = TTF_RenderUTF8_Blended(ScoreFont, ScoreChar, FontColor);
    ScoreTexture = SDL_CreateTextureFromSurface(Renderer, ScoreSurface);
    SDL_Rect ScoreRect = {494, 282, ScoreSurface->w, ScoreSurface->h};
    SDL_RenderCopy(Renderer, ScoreTexture, NULL, &ScoreRect);
    SDL_RenderPresent(Renderer);
    SDL_DestroyTexture(ScoreTexture);
    SDL_DestroyTexture(GameOverTexture);
}

void restart()
{
    is_begin = 0;
    fall = 0;
    last_space_time = 0;
    space_up_time = 0;
    interval = 0;
    character_position[0] = 360;
    character_position[1] = 415;
    Platform_position[0] = 480;
    Platform_position[1] = 310;
    Save_position[0] = 0;
    Save_position[1] = 0;
    Platform_size[0] = 196;
    Platform_size[1] = 141;
    front_or_left = 1;
    exf_or_l = 1;
    RandomCreate = 2;
    Score = 0;
    for (int i = 0; i < 15; i++)
        ScoreChar[i] = 0;
    SDL_DestroyTexture(SaveTexture);
    printf("restart\n");
    Playing();
}

void add_box(double angle)
{
    RandomCreate = rand() % 6;
    SDL_Texture *RandomCreateTexture = NULL;
    RandomCreateTexture = SDL_CreateTextureFromSurface(Renderer, PlatformSurface[RandomCreate]);
    SDL_Rect RandomCreateRect;
    int new_x = 0;
    int new_y = 0;
    int gap = rand() % 150 + 200;
    if (front_or_left == 1)
    {
        gap = rand() % 150 + 200;
        new_x = Platform_position[0] + (gap * cos(angle));
        new_y = Platform_position[1] - (gap * sin(angle));
    }
    else if (front_or_left == 2)
    {
        gap = rand() % 100 + 200;
        new_x = Platform_position[0] - (gap * cos(angle));
        new_y = Platform_position[1] - (gap * sin(angle));
    }
    // printf("new_x :%d new_y :%d\n", new_x, new_y);
    SDL_Texture *PersonTexture = SDL_CreateTextureFromSurface(Renderer, PersonSurface);
    int flag = 0;
    for (int i = 0; i < new_y; i++)
    {
        SaveRect = (SDL_Rect){Save_position[0], Save_position[1], 1000, 700};
        SDL_RenderCopy(Renderer, SaveTexture, NULL, &SaveRect);
        PersonRect = (SDL_Rect){character_position[0], character_position[1], 80, 130};
        SDL_RenderCopy(Renderer, PersonTexture, NULL, &PersonRect);
        RandomCreateRect = (SDL_Rect){new_x, i, Platform_size[0], Platform_size[1]};
        SDL_RenderCopy(Renderer, RandomCreateTexture, NULL, &RandomCreateRect);
        PrintCount();
        if (i == new_y - 1 && flag == 0)
        {
            i = new_y - 50;
            flag = 1;
        }
        if (flag == 1)
            SDL_Delay(10);
    }
    Platform_size[0] = Platform_size[0] - 2;
    Platform_size[1] = Platform_size[1] - 2;
    exPlatform_position[0] = Platform_position[0];
    exPlatform_position[1] = Platform_position[1];
    Platform_position[0] = new_x;
    Platform_position[1] = new_y;
    SDL_SetRenderTarget(Renderer, SaveTexture);
    SDL_RenderCopy(Renderer, RandomCreateTexture, NULL, &RandomCreateRect);
    SDL_SetRenderTarget(Renderer, NULL);
    SDL_DestroyTexture(PersonTexture);
    SDL_DestroyTexture(RandomCreateTexture);
}

int judge()
{
    int center1[2] = {0}, center2[2] = {0}, person[2] = {0}, center3[2] = {0}, center4[4] = {0};
    person[0] = character_position[0] + 35; // 人物中心位置
    person[1] = character_position[1] + 85;
    if (front_or_left == 1)
    {
        center1[0] = Platform_position[0] + 90 - (196 - Platform_size[0]) / 2; // 对应直线的两个点
        center2[0] = Platform_position[0] + 155 - (196 - Platform_size[0]) / 2;
        // person[0] -= 5;
        if (RandomCreate == 3)
        {
            center1[0] -= 15;
            center2[0] -= 15;
        }
        if (RandomCreate == 5)
        {
            center1[0] -= 10;
            center2[0] -= 10;
        }
        printf("center1: %d center2: %d\n", center1[0], center2[0]);
        printf("person: %d\n", person[0]);
        if (person[0] > center1[0] && person[0] < center2[0])
            return 0; // 在平台上
        else
            return 1; // 超出平台
    }
    else if (front_or_left == 2)
    {
        center4[0] = Platform_position[0] + 150 - (196 - Platform_size[0]) / 2;
        center3[0] = Platform_position[0] + 88 - (196 - Platform_size[0]) / 2;
        center3[0] += 5;
        person[0] += 5;
        if (RandomCreate == 3)
        {
            center3[0] -= 10;
            center4[0] -= 10;
        }
        if (RandomCreate == 5)
        {
            center1[0] -= 10;
            center2[0] -= 10;
        }
        printf("center3: %d center4: %d\n", center3[0], center4[0]);
        printf("person: %d\n", person[0]);
        if (person[0] > center3[0] && person[0] < center4[0])
            return 0; // 在平台上
        else
            return 1; // 超出平台
    }
    return 2;
}

void move(int distance, double angle)
{
    int i = 0;
    SDL_Texture *PersonTexture = PersonTexture = SDL_CreateTextureFromSurface(Renderer, PersonSurface);
    if (front_or_left == 1)
    {
        for (i = 0; i < distance; i++)
        {
            SaveRect = (SDL_Rect){Save_position[0] - (i * cos(angle)), Save_position[1] + (i * sin(angle)), 1000, 700};
            SDL_RenderCopy(Renderer, SaveTexture, NULL, &SaveRect); // 背景移动
            PersonRect = (SDL_Rect){character_position[0] - (i * cos(angle)), character_position[1] + (i * sin(angle)), 80, 130};
            SDL_RenderCopy(Renderer, PersonTexture, NULL, &PersonRect); // 人物移动
            SDL_RenderPresent(Renderer);
        }
        Save_position[0] = Save_position[0] - (i * cos(angle));
        Save_position[1] = Save_position[1] + (i * sin(angle));
        character_position[0] = character_position[0] - (i * cos(angle));
        character_position[1] = character_position[1] + (i * sin(angle));
        Platform_position[0] = Platform_position[0] - (i * cos(angle));
        Platform_position[1] = Platform_position[1] + (i * sin(angle));
    }
    if (front_or_left == 2)
    {
        for (i = 0; i < distance; i++)
        {
            SaveRect = (SDL_Rect){Save_position[0] + (i * cos(angle)), Save_position[1] + (i * sin(angle)), 1000, 700};
            SDL_RenderCopy(Renderer, SaveTexture, NULL, &SaveRect); // 背景移动
            PersonRect = (SDL_Rect){character_position[0] + (i * cos(angle)), character_position[1] + (i * sin(angle)), 80, 130};
            SDL_RenderCopy(Renderer, PersonTexture, NULL, &PersonRect); // 人物移动
            SDL_RenderPresent(Renderer);
        }
        Save_position[0] = Save_position[0] + (i * cos(angle));
        Save_position[1] = Save_position[1] + (i * sin(angle));
        character_position[0] = character_position[0] + (i * cos(angle));
        character_position[1] = character_position[1] + (i * sin(angle));
        Platform_position[0] = Platform_position[0] + (i * cos(angle));
        Platform_position[1] = Platform_position[1] + (i * sin(angle));
    }

    SaveRect = (SDL_Rect){Save_position[0], Save_position[1], 1000, 700};
    SDL_Texture *tempTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1000, 700);
    PlayBackGroundTexture = SDL_CreateTextureFromSurface(Renderer, PlayBackGroundSurface);
    SDL_SetRenderTarget(Renderer, tempTexture);
    SDL_RenderCopy(Renderer, SaveTexture, NULL, NULL); // 将SaveTexture绘制到tempTexture上
    SDL_SetRenderTarget(Renderer, NULL);
    SDL_SetRenderTarget(Renderer, SaveTexture);
    SDL_RenderCopy(Renderer, PlayBackGroundTexture, NULL, NULL);
    SDL_RenderCopy(Renderer, tempTexture, NULL, &SaveRect);
    SDL_SetRenderTarget(Renderer, NULL);
    Save_position[0] = 0;
    Save_position[1] = 0;
    SDL_DestroyTexture(tempTexture);
    SDL_DestroyTexture(PersonTexture);
    SDL_DestroyTexture(PlayBackGroundTexture);
}

void Play_Start()
{
    SaveTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1000, 700);
    PlayBackGroundTexture = SDL_CreateTextureFromSurface(Renderer, PlayBackGroundSurface);
    SDL_RenderClear(Renderer);
    SDL_RenderCopy(Renderer, PlayBackGroundTexture, NULL, NULL);
    SDL_SetRenderTarget(Renderer, SaveTexture);
    SDL_RenderCopy(Renderer, PlayBackGroundTexture, NULL, NULL); // 保存现在的背景
    SDL_SetRenderTarget(Renderer, NULL);
    SDL_DestroyTexture(PlayBackGroundTexture);
    if (SaveTexture == NULL)
        printf("SaveTexture wrong\n");

    SDL_Rect StartBoxRect[2]; // 开始时两个箱子的大小和距离
    StartBoxRect[0] = (SDL_Rect){270, 450, 196, 141};
    StartBoxRect[1] = (SDL_Rect){480, 310, 196, 141};
    for (int i = 0; i < 2; i++)
    {
        SDL_Texture *PlatformTexture = NULL;
        PlatformTexture = SDL_CreateTextureFromSurface(Renderer, PlatformSurface[i]);
        SDL_RenderCopy(Renderer, PlatformTexture, NULL, &StartBoxRect[i]);
        SDL_SetRenderTarget(Renderer, SaveTexture);
        SDL_RenderCopy(Renderer, PlatformTexture, NULL, &StartBoxRect[i]); // 保存现在的平面
        SDL_SetRenderTarget(Renderer, NULL);
        if (SaveTexture == NULL)
            printf("SaveTexture wrong\n");
        SDL_RenderPresent(Renderer);
        SDL_DestroyTexture(PlatformTexture);
    }
    SDL_Texture *PersonTexture = NULL;
    PersonTexture = SDL_CreateTextureFromSurface(Renderer, PersonSurface);
    PersonRect = (SDL_Rect){360, 415, 80, 130};
    SDL_RenderCopy(Renderer, PersonTexture, NULL, &PersonRect);
    SDL_RenderPresent(Renderer);
    SDL_DestroyTexture(PersonTexture);
    PrintCount();
}
void PrintCount()
{
    sprintf(ScoreChar, "Score:%d", Score);
    ScoreSurface = TTF_RenderUTF8_Blended(ScoreFont, ScoreChar, FontColor);
    ScoreTexture = SDL_CreateTextureFromSurface(Renderer, ScoreSurface);
    SDL_Rect ScoreRect = {690 - ScoreSurface->w / 2, 38, ScoreSurface->w, ScoreSurface->h};
    SDL_RenderCopy(Renderer, ScoreTexture, NULL, &ScoreRect);
    SDL_RenderPresent(Renderer);
    SDL_FreeSurface(ScoreSurface);
    SDL_DestroyTexture(ScoreTexture);
}

void Return_Main()
{
    SDL_Texture *MainBackGroundTexture = SDL_CreateTextureFromSurface(Renderer, MainBackGroundSurface);
    SDL_RenderClear(Renderer);
    SDL_RenderCopy(Renderer, MainBackGroundTexture, NULL, NULL);
    SDL_RenderPresent(Renderer);
    SDL_DestroyTexture(MainBackGroundTexture);
}

void LOAD()
{
    MainBackGroundSurface = IMG_Load("Picture/open.png");
    PlayBackGroundSurface = IMG_Load("Picture/background-1.png");
    PersonSurface = IMG_Load("Picture/character1.png");
    GameOverSurface = IMG_Load("Picture/Gameover.png");
    for (int i = 0; i < 6; i++)
    {
        char box_name[20] = {0};
        sprintf(box_name, "Picture/box-%d.png", i + 1);
        PlatformSurface[i] = IMG_Load(box_name);
    }
    ScoreFont = TTF_OpenFont("Picture/Word.ttf", 50);
}

void QUIT()
{
    SDL_FreeSurface(MainBackGroundSurface);
    SDL_FreeSurface(PlayBackGroundSurface);
    SDL_FreeSurface(ScoreSurface);
    for (int i = 0; i < 2; i++)
        SDL_FreeSurface(PlatformSurface[i]);
    SDL_FreeSurface(PersonSurface);
    SDL_DestroyTexture(SaveTexture);
    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    TTF_CloseFont(ScoreFont);
    SDL_Quit();
}
