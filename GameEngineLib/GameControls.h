#pragma once
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
    GameControls(VEngine::Input::Keyboard* keyboard, VEngine::Input::Mouse* mouse, VEngine::FileSystem::Media* media, std::string inifile);
    ~GameControls();
    VEngine::Utilities::EventHandler<std::string> onKeyDown;
    VEngine::Utilities::EventHandler<std::string> onKeyUp;
    bool isKeyDown(std::string name);
    double readAxisValue(std::string name);
    glm::ivec2 getCursorPosition();
    VEngine::Input::Keyboard* getRawKeyboard();
    VEngine::Input::Mouse* getRawMouse();
private:
    VEngine::Input::Keyboard * keyboard;
    VEngine::Input::Mouse* mouse;
    VEngine::FileSystem::Media* media;
    std::map<std::string, int> simpleKeyBinds;
    std::map<std::string, ControlKeyAxis> keysAxisBinds;
};

