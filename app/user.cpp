#include "user.h"
#include <iostream>
#include <string>

UserProfile currentUser;

void initUser() {
    std::string username, since, color, avatar;
    
    std::cout << "Username: ";
    std::getline(std::cin, username);
    
    std::cout << "Color: ";
    std::getline(std::cin, color);
    
    
    currentUser = {
        username,
        "Invalid Date", 
        color,
        "images/icons/icon_user_aqua-green.png"
    };
}