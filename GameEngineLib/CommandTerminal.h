#pragma once

class CommandTerminal
{
public:
    CommandTerminal(VEngine::UserInterface::UIRenderer* renderer, VEngine::Input::Keyboard* keyboards, VEngine::FileSystem::Media* media);
    ~CommandTerminal();
    void printMessage(VEngine::UserInterface::UIColor color, std::string msg);
    void clear();
    void enableInput();
    void disableInput();
    bool isInputEnabled();
    VEngine::Utilities::EventHandler<std::string> onSendText;

private:
    VEngine::UserInterface::UIRenderer* renderer;
    VEngine::Input::Keyboard* keyboard;

    void updateRenders();
    int cursorPosition = 0;
    int ignoreCharsCount = 0;
    std::vector<std::string> linesStrings;
    std::vector<VEngine::UserInterface::UIColor> linesColors;
    std::string inputString = "";
    bool inputEnabled = false;
    std::vector<VEngine::UserInterface::UIText*> lines;
    VEngine::UserInterface::UIText* input;
};

