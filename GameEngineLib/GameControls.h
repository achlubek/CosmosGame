#pragma once
#include "EventHandler.h"
#include "Mouse.h"
#include "Keyboard.h"
class KeyValuePair {
public:
    int key;
    double value;
    KeyValuePair(int k, double v) {
        key = k;
        value = v;
    }
};
class ControlKeyAxis {
public:
    std::vector<KeyValuePair> binds;
    double currentValue = 0;
    bool keepOnRelease = false;
    ControlKeyAxis() {
    }
    ControlKeyAxis(std::vector<KeyValuePair> b, bool keepValue) {
        binds = b;
        keepOnRelease = keepValue;
    }
    void onKeyDown(int key) {
        for (int i = 0; i < binds.size(); i++) {
            if (key == binds[i].key) {
                currentValue = binds[i].value;
            }
        }
    }
    void onKeyUp(int key) {
        if (!keepOnRelease) {
            for (int i = 0; i < binds.size(); i++) {
                if (key == binds[i].key) {
                    currentValue = 0;
                    break;
                }
            }
        }
    }
};
class GameControls
{
public:
    GameControls(Keyboard* keyboard, Mouse* mouse, std::string inifile);
    ~GameControls();
    EventHandler<std::string> onKeyDown;
    EventHandler<std::string> onKeyUp;
    bool isKeyDown(std::string name);
    double readAxisValue(std::string name);
    glm::ivec2 getCursorPosition();
    Keyboard* getRawKeyboard();
    Mouse* getRawMouse();
private:
    Keyboard * keyboard;
    Mouse* mouse;
    std::map<std::string, int> simpleKeyBinds;
    std::map<std::string, ControlKeyAxis> keysAxisBinds;
};

