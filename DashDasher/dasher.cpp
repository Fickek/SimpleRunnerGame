#include "raylib.h"


struct AnimData 
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight) 
{
    return data.pos.y >= windowHeight - data.rec.height;   
}



AnimData updateAnimaData(AnimData data, float deltaTime, int maxFrame) 
{
    // update running time
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime) 
    {
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main(void)
{
    // array 
    int windowDimensions[2];
    windowDimensions[0] = 512;
    windowDimensions[1] = 380;


    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher");

    SetTargetFPS(60); 

    int velocity{ 0 };

    // acceleration due to gravity (pixels/s) / s
    const int gravity{ 1'000 };

    // jump velocity (pixel/second )
    const int jumpVelocity{ -600 };

    // is the rectangle in the air?
    bool isInAir{};

    // scarfy variables
    Texture2D scarfy = LoadTexture("Textures/scarfy.png");
    
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width / 6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0] / 2 - scarfyData.rec.width / 2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0 / 12.0;
    scarfyData.runningTime = 0.0;
    

    // nebula variables
    Texture2D nebula = LoadTexture("Textures/12_nebula_spritesheet.png");

    // nebula x velocity (pixels / second)
    int nebulaVel{ -200 };

    const int sizeOfNebulae{ 6 };
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height / 8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0;
        nebulae[i].updateTime = 0;
        nebulae[i].pos.x = windowDimensions[0] + (i * 300);
    }

    float finishLine{ nebulae[sizeOfNebulae - 1].pos.x };

    // background
    Texture2D background = LoadTexture("Textures/far-buildings.png");
    float bgX{};

    // midground
    Texture2D midground = LoadTexture("Textures/back-buildings.png");
    float mgX{};

    // foreground
    Texture2D foreground = LoadTexture("Textures/foreground.png");
    float fgX{};

    bool collision{};

    while (!WindowShouldClose())
    {

        const float dT{ GetFrameTime() };

        // start drawing
        BeginDrawing();

        ClearBackground(WHITE);

        bgX -= 20 * dT;
        mgX -= 40 * dT;
        fgX -= 80 * dT;

        // scroll background
        if(bgX <= -background.width * 2)
        {
            bgX = 0.0;
        }

        // scroll the midground
        if (mgX <= -midground.width * 2)
        {
            mgX = 0.0;
        }

        // scroll the foreground
        if (fgX <= -foreground.width * 2)
        {
            fgX = 0.0;
        }

        // draw the background
        Vector2 bg1Pos{ bgX, 0.0 };
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{ bgX + background.width * 2, 0.0 };
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        // draw the midground
        Vector2 mg1Pos{ mgX, 0.0 };
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{ mgX + midground.width * 2, 0.0 };
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        // draw the foreground
        Vector2 fg1Pos{ fgX, 0.0 };
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{ fgX + foreground.width * 2, 0.0 };
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);


        // ground check
        if (isOnGround(scarfyData, windowDimensions[1]))
        {
            velocity = 0;
            isInAir = false;
        }
        else 
        {
            // apply gravity
            velocity += gravity * dT;
            isInAir = true;
        }

        // jump check
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVelocity;
        }

        // update scarfy position
        scarfyData.pos.y += velocity * dT;

        for(int i = 0; i < sizeOfNebulae; i++)
        {
            // update position for each nebula
            nebulae[i].pos.x += nebulaVel * dT;
        }

        // update finishLine
        finishLine += nebulaVel * dT;

        // update scarfy's animationm frame
        if (!isInAir)
        {
            scarfyData = updateAnimaData(scarfyData, dT, 5);
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            // update nebula animationm frame
            nebulae[i] = updateAnimaData(nebulae[i], dT, 7);
        }

        for (AnimData nebul : nebulae)
        {
            float pad{ 50 };
            Rectangle nebRec
            {
                nebul.pos.x + pad,
                nebul.pos.y + pad,
                nebul.rec.width - 2 * pad,
                nebul.rec.height - 2 * pad,
            };
            Rectangle scarfyRec
            {
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height,
            };
            if(CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }

        if(collision)
        {
            // lose the game
            DrawText("GAME OVER", windowDimensions[0]/4, windowDimensions[1]/2, 40, WHITE);
        }
        else if(scarfyData.pos.x >= finishLine)
        {
            DrawText("YOU WIN", windowDimensions[0] / 4, windowDimensions[1] / 2, 40, WHITE);
        }
        else 
        {
            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);

            for (int i = 0; i < sizeOfNebulae; i++)
            {
                // draw nebula
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
        }

        // end drawing
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();

}