# Chat Up!
Chat Up! is a real-time chat application with a Frutiger Aero aesthetic, enabling message exchange between devices on the same local network.

## Screenshots

### Browser Client
![browser screenshot](./assets/browser-screenshot.png)

### Desktop App
![app screenshot](./assets/app-screenshot.png)

## Features
- Local C++ HTTP server
- Real-time messaging across network devices
- Frutiger Aero design aesthetic
- Avatar support and user profiles
- Message history (last 4 messages displayed)

## Requirements
- g++ (C++17)
- make
- raylib

## Installation & Usage
```bash
git clone https://github.com/FrankSteps/chat-up.git
cd chat-up
make
make run
make clean
```

Server runs on `http://localhost:5000`


## Project Structure

```bash
chat-up
├── app                 # Desktop application source
│   ├── user.cpp
│   └── window.cpp
├── assets              # Screenshots and project images
│   ├── app-screenshot.png
│   └── browser-screenshot.png
├── backend             # C++ HTTP server with message routing
│   ├── main.cpp
│   └── server.cpp
├── builds              # Compiled executable output directory
│   └── chatup
├── frontend            # Web browser client (HTML, CSS, JavaScript)
│   ├── images
│   │   ├── favicon
│   │   │   └── icon_user.ico
│   │   └── icons
│   │       ├── icon_user_aqua-green.png
│   │       ├── icon_user_black.png
│   │       ├── icon_user_blue.png
│   │       ├── icon_user_orange.png
│   │       ├── icon_user_pink.png
│   │       ├── icon_user_purple.png
│   │       ├── icon_user_red.png
│   │       └── icon_user_white.png
│   ├── index.html
│   ├── script.js
│   └── style.css
├── include             # C++ header files and library declarations
│   ├── httplib.h
│   ├── shared.h
│   └── user.h
├── LICENSE             # MIT License
├── makefile            # Build configuration and compilation rules
└── README.md           # Project documentation

```

## Current Limitations
- No authentication or encryption
- Messages stored in memory (lost on restart)
- Local network only
- Under active development

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.