#ifndef USER_H
#define USER_H

#include <string>

struct UserProfile {
    std::string username;
    std::string since;
    std::string favoriteColor;
    std::string avatar;
};

extern UserProfile currentUser;

#endif