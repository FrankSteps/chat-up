/*
    Chat Up! App
*/

#include "shared.h"
#include <vector>

namespace ray {
    #include <raylib.h>
}

std::string text = "";
std::vector<std::string> message;

void startWindow(){
    ray::InitWindow(720, 480, "Chat Up! APP");

    ray::Image icon = ray::LoadImage("frontend/images/icons/icon_user_aqua-green.png");
    ray::Texture2D avatar = ray::LoadTexture("frontend/images/icons/icon_user_aqua-green.png");
    ray::SetWindowIcon(icon);

    float backspaceTimer = 0.0f;
    const float backspaceDelay = 0.03f;
    const int inputMaxWidth = 590;
    const int fontSize = 20;

    while(!ray::WindowShouldClose()){       
        // character processor
        int key = ray::GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 255) {
                std::string teste = text + (char)key;
                if (ray::MeasureText(teste.c_str(), fontSize) < inputMaxWidth) {
                    text = teste;
                }
            }
            key = ray::GetCharPressed();
        }

        // Backspace
        if (ray::IsKeyDown(ray::KEY_BACKSPACE)) {
            backspaceTimer += ray::GetFrameTime();
            if (backspaceTimer >= backspaceDelay && !text.empty()) {
                text.pop_back();
                backspaceTimer = 0.0f;
            }
        } else {
            backspaceTimer = 0.0f;
        }

        // send message
        if (ray::IsKeyPressed(ray::KEY_ENTER) && !text.empty()) {
            message.push_back(text);
            text.clear();
        }

        ray::BeginDrawing();
        ray::ClearBackground(ray::Color{194, 233, 251, 255});

        // user information
        ray::DrawRectangle(0, 0, 720, 130, ray::Color{255, 255, 255, 240});
        ray::DrawLine(0, 130, 720, 130, ray::Color{200, 200, 200, 255});

        // Avatar
        const float avatarSize = 100.0f;
        const float avatarX = 10.0f;
        const float avatarY = 10.0f;
        const float border = 4.5f;

        float scale = std::min(
            avatarSize / avatar.width,
            avatarSize / avatar.height
        );
        
        float drawW = avatar.width * scale;
        float drawH = avatar.height * scale;

        ray::Rectangle avatarRect = {avatarX, avatarY, drawW, drawH};

        // draw avatar
        ray::DrawTexturePro(
            avatar,
            ray::Rectangle{0, 0, (float)avatar.width, (float)avatar.height},
            avatarRect,
            ray::Vector2{0, 0},
            0.0f,
            ray::WHITE
        );
        
        ray::DrawRectangleRoundedLinesEx(
            avatarRect,
            0.2f,
            10,
            border,
            ray::Color{255, 255, 255, 255}
        );

        // Profile info
        ray::DrawText("Username", 120, 25, 24, ray::Color{50, 50, 50, 255});
        ray::DrawText("Since: ", 120, 60, 16, ray::Color{100, 100, 100, 255});
        ray::DrawText("Favorite color: ", 120, 80, 16, ray::Color{100, 100, 100, 255});

        // input box
        float inputX = 10;
        float inputY = 430;
        float inputW = 630;
        float inputH = 40;
        float btnX = inputX + inputW + 10;
        float btnY = inputY;
        float btnSize = 40;

        // Input box background
        ray::DrawRectangleRounded(
            ray::Rectangle{inputX, inputY, inputW, inputH},
            0.7f, 10,
            ray::Color{255, 255, 255, 240}
        );

        ray::DrawRectangleRoundedLines(
            ray::Rectangle{inputX, inputY, inputW, inputH},
            0.7f, 10,
            ray::Color{200, 200, 200, 255}
        );

        // Placeholder
        if (text.empty()) {
            ray::DrawText("Digite uma mensagem...", inputX + 10, inputY + 10, fontSize, ray::Color{150, 150, 150, 255});
        } else {
            ray::DrawText(text.c_str(), inputX + 10, inputY + 10, fontSize, ray::Color{30, 30, 30, 255});
        }

        // Cursor
        if (((int)(ray::GetTime() * 2) % 2) == 0 && !text.empty()) {
            int textWidth = ray::MeasureText(text.c_str(), fontSize);
            ray::DrawText("|", inputX + 10 + textWidth, inputY + 10, fontSize, ray::Color{30, 30, 30, 255});
        }

        // send button
        ray::Color btnColor = ray::Color{0, 157, 214, 255}; // aqua
        ray::Color btnHover = ray::Color{80, 200, 255, 255};  // mais claro (hover)

        ray::Vector2 mousePos = ray::GetMousePosition();
        bool isHovered = mousePos.x >= btnX && mousePos.x <= btnX + btnSize &&
                         mousePos.y >= btnY && mousePos.y <= btnY + btnSize;

        ray::DrawCircle(btnX + btnSize/2, btnY + btnSize/2, btnSize/2, isHovered ? btnHover : btnColor);
        

        ray::DrawText(">", btnX + 12, btnY + 8, 24, ray::Color{255, 255, 255, 255});
        
        // click to send
        if (isHovered && ray::IsMouseButtonPressed(ray::MOUSE_BUTTON_LEFT)) {
            if (!text.empty()) {
                message.push_back(text);
                text.clear();
            }
        }

        ray::EndDrawing();
    }

    ray::UnloadImage(icon);
    ray::UnloadTexture(avatar);
    ray::CloseWindow();
}