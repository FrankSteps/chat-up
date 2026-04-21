/*
    Chat App!
*/

// libraries
#include "shared.h"
#include "httplib.h"
#include "user.h"
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

namespace ray {
    #include <raylib.h>
}

std::string text = "";

struct ChatMessage {
    std::string user;
    std::string text;
    std::string color;
    std::string avatar;
};

std::vector<ChatMessage> chatMessages;
httplib::Client* client = nullptr;                  
std::string serverUrl = "http://localhost:5000";
bool serverConnected = false;
std::mutex messagesMutex;


// parse JSON response from server
void parseMessages(const std::string& jsonStr) {
    std::vector<ChatMessage> msgs;
    
    size_t pos = 0;
    while ((pos = jsonStr.find("{", pos)) != std::string::npos) {
        size_t end = jsonStr.find("}", pos);
        if(end == std::string::npos) break;
        
        std::string obj = jsonStr.substr(pos, end - pos + 1);
        ChatMessage messager;
        
        // extract fields from JSON object
        size_t userPos = obj.find("\"user\":\"") + 8;
        messager.user = obj.substr(userPos, obj.find("\"", userPos) - userPos);
        
        size_t textPos = obj.find("\"text\":\"") + 8;
        messager.text = obj.substr(textPos, obj.find("\"", textPos) - textPos);
        
        size_t colorPos = obj.find("\"color\":\"") + 9;
        messager.color = obj.substr(colorPos, obj.find("\"", colorPos) - colorPos);
        
        size_t avatarPos = obj.find("\"avatar\":\"") + 10;
        messager.avatar = obj.substr(avatarPos, obj.find("\"", avatarPos) - avatarPos);
        
        msgs.push_back(messager);
        pos = end + 1;
    }
    
    // update global messages with mutex lock
    std::lock_guard<std::mutex> lock(messagesMutex);
    chatMessages = msgs;
}


// fetch messages from server
void loadMessages() {
    if(!client) {
        return;
    }
    
    auto response = client->Get("/messages");
    if(response && response->status == 200) {
        serverConnected = true;
        parseMessages(response->body);
    } else {
        serverConnected = false;
    }
}


// send message to server
void sendMessage(const std::string& msg) {
    if(!client || msg.empty()){
        return;
    }
    
    std::thread([msg]() {
        httplib::Params params;
        params.emplace("user", currentUser.username);
        params.emplace("message", msg);
        params.emplace("color", currentUser.favoriteColor);
        params.emplace("avatar", currentUser.avatar);
        
        auto res = client->Post("/send", params);
        if(res && res->status == 200) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            loadMessages();
        }
    }).detach();
}

// start window :D
void startWindow(){
    ray::InitWindow(720, 480, "Chat App! by FrankSteps");

    ray::Image icon = ray::LoadImage("frontend/images/icons/icon_user_aqua-green.png");
    ray::Texture2D avatar = ray::LoadTexture(("frontend/" + currentUser.avatar).c_str());
    ray::SetWindowIcon(icon);

    client = new httplib::Client(serverUrl);
    client->set_connection_timeout(1, 0);

    std::thread(loadMessages).detach();

    float backspaceTimer = 0.0f;
    const float backspaceDelay = 0.05f;
    const int inputMaxWidth = 590;
    const int fontSize = 20;


    // variables to update messages
    float updateTimer = 0.0f;
    const float updateInterval = 2.0f;


    // render window
    while(!ray::WindowShouldClose()){   
        // update messages 
        updateTimer += ray::GetFrameTime();
        if(updateTimer >= updateInterval) {
            std::thread(loadMessages).detach();
            updateTimer = 0.0f;
        }
        
        int key = ray::GetCharPressed();
        while(key > 0) {
            if (key >= 32 && key <= 255) {
                std::string teste = text + (char)key;
                if (ray::MeasureText(teste.c_str(), fontSize) < inputMaxWidth) {
                    text = teste;
                }
            }
            key = ray::GetCharPressed();
        }

        if(ray::IsKeyDown(ray::KEY_BACKSPACE)) {
            backspaceTimer += ray::GetFrameTime();
            if (backspaceTimer >= backspaceDelay && !text.empty()) {
                text.pop_back();
                backspaceTimer = 0.0f;
            }
        } else {
            backspaceTimer = 0.0f;
        }

        if(ray::IsKeyPressed(ray::KEY_ENTER) && !text.empty()) {
            sendMessage(text);
            text.clear();
        }

        ray::BeginDrawing();
        ray::ClearBackground(ray::Color{194, 233, 251, 255});


        // user header
        ray::DrawRectangle(0, 0, 720, 130, ray::Color{255, 255, 255, 240});
        ray::DrawLine(0, 130, 720, 130, ray::Color{200, 200, 200, 255});


        // avatar rendering
        float scale = std::min(100.0f / avatar.width, 100.0f / avatar.height);
        float drawW = avatar.width * scale;
        float drawH = avatar.height * scale;
        ray::Rectangle avatarRect = {10.0f, 10.0f, drawW, drawH};


        // user info
        ray::DrawTexturePro(avatar, ray::Rectangle{0, 0, (float)avatar.width, (float)avatar.height}, avatarRect, ray::Vector2{0, 0}, 0.0f, ray::WHITE);
        ray::DrawRectangleRoundedLinesEx(avatarRect, 0.2f, 10, 4.5f, ray::Color{255, 255, 255, 255});

        ray::DrawText(currentUser.username.c_str(), 120, 25, 30, ray::Color{50, 50, 50, 255});
        ray::DrawText(("Since: " + currentUser.since).c_str(), 120, 60, 20, ray::Color{100, 100, 100, 255});
        ray::DrawText(("Favorite color: " + currentUser.favoriteColor).c_str(), 120, 80, 20, ray::Color{100, 100, 100, 255});


        /*
            Locks mutex to safely access chatMessages
            Limits display to last 4 messages
            Load avatar image once and store in cache for reuse
            Breaks long text into multiple lines based on maxWidth
            Draw username and message text with consistent line spacing
            Stops rendering if messageY exceeds chatMaxY
        */
        {
            std::lock_guard<std::mutex> lock(messagesMutex);

            if(chatMessages.size() > 4) {
                chatMessages.erase(chatMessages.begin(), chatMessages.begin() + (chatMessages.size() - 4));
            }

            float messageY = 140;
            float maxWidth = 500;
            const float fontSize = 20;
            const float chatMaxY = 400;
            const float lineHeight = 26;

            static std::map<std::string, ray::Texture2D> avatarCache;

            for(size_t i = 0; i < chatMessages.size(); i++) {
                auto msg = chatMessages[i];
                std::string text = msg.text;

                if(text.length() > 64) {
                    text = text.substr(0, 64) + "...";
                }

                if(avatarCache.find(msg.avatar) == avatarCache.end()) {
                    avatarCache[msg.avatar] = ray::LoadTexture(("frontend/" + msg.avatar).c_str());
                }

                auto avatar = avatarCache[msg.avatar];

                if(messageY < chatMaxY) {
                    ray::DrawTexturePro(avatar, ray::Rectangle{0, 0, (float)avatar.width, (float)avatar.height}, ray::Rectangle{10, messageY, 45, 45}, ray::Vector2{0, 0}, 0, ray::WHITE);
                }

                std::vector<std::string> lines;
                std::string line = "";

                for(size_t j = 0; j < text.length(); j++) {
                    line += text[j];
                    if(ray::MeasureText(line.c_str(), fontSize) > maxWidth || text[j] == '\n') {
                        lines.push_back(line);
                        line = "";
                    }
                }
                if(!line.empty()) {
                    lines.push_back(line);
                }

                if(messageY < chatMaxY) {
                    ray::DrawText(msg.user.c_str(), 60, messageY, fontSize, ray::Color{50, 50, 50, 255});
                    messageY += lineHeight;
                }

                for(size_t j = 0; j < lines.size(); j++) {
                    if(messageY < chatMaxY) {
                        ray::DrawText(lines[j].c_str(), 60, messageY, fontSize, ray::Color{30, 30, 30, 255});
                        messageY += lineHeight - 10;
                    }
                }

                messageY += 8;
            }
        }


        // input box
        float inputX = 10, inputY = 430, inputW = 630, inputH = 40;
        float btnX = inputX + inputW + 16, btnY = inputY, btnSize = 40;

        ray::DrawRectangleRounded(ray::Rectangle{inputX, inputY, inputW, inputH}, 0.7f, 10, ray::Color{255, 255, 255, 240});
        ray::DrawRectangleRoundedLines(ray::Rectangle{inputX, inputY, inputW, inputH}, 0.7f, 10, ray::Color{200, 200, 200, 255});


        // text input
        if(text.empty()){
            ray::DrawText("Type your message here", inputX + 10, inputY + 10, fontSize, ray::Color{150, 150, 150, 255});
        } else {
            ray::DrawText(text.c_str(), inputX + 10, inputY + 10, fontSize, ray::Color{30, 30, 30, 255});
        }


        // cursor
        if(((int)(ray::GetTime() * 2) % 2) == 0 && !text.empty()) {
            int textWidth = ray::MeasureText(text.c_str(), fontSize);
            ray::DrawText("|", inputX + 10 + textWidth, inputY + 10, fontSize, ray::Color{30, 30, 30, 255});
        }


        // send button
        ray::Vector2 mousePos = ray::GetMousePosition();
        bool isHovered = mousePos.x >= btnX && mousePos.x <= btnX + btnSize && mousePos.y >= btnY && mousePos.y <= btnY + btnSize;

        ray::Color btnColor = isHovered ? ray::Color{80, 200, 255, 255} : ray::Color{0, 157, 214, 255};
        ray::DrawCircle(btnX + btnSize/2, btnY + btnSize/2, btnSize/2, btnColor);
        ray::DrawText(">", btnX + 18, btnY + 8, 24, ray::Color{255, 255, 255, 255});


        // server status
        ray::Color statusColor = serverConnected ? ray::Color{0, 255, 0, 255} : ray::Color{255, 0, 0, 255};
        ray::DrawCircle(710, 10, 5, statusColor);


        // send on button click
        if(isHovered && ray::IsMouseButtonPressed(ray::MOUSE_BUTTON_LEFT)) {
            if(!text.empty()){
                sendMessage(text);
                text.clear();
            }
        }

        ray::EndDrawing();
    }

    // unloads
    ray::UnloadImage(icon);
    ray::UnloadTexture(avatar);
    ray::CloseWindow();
    delete client;
}