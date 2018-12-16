#include "stdafx.h"
#include "MainMenuGameStage.h"


MainMenuGameStage::MainMenuGameStage(GameContainer* container)
    : AbsGameStage(container)
{
    auto ui = getUIRenderer();
    freeFlightButton = new UIText(ui, 0.01, 0.2, UIColor(1.0, 1.0, 1.0, 1.0), "coolvetica.ttf", 33, "Free flight");
    ui->addDrawable(freeFlightButton);

    editorButton = new UIText(ui, 0.01, 0.24, UIColor(1.0, 1.0, 1.0, 1.0), "coolvetica.ttf", 33, "Celestial bodies preview");
    ui->addDrawable(editorButton);

    quitButton = new UIText(ui, 0.01, 0.28, UIColor(1.0, 1.0, 1.0, 1.0), "coolvetica.ttf", 33, "Quit game");
    ui->addDrawable(quitButton);
    /*
    freeFlightButton->onMouseDown.add([&](int key) {
        GameContainer::getInstance()->setCurrentStage("freeflight");
    });

    editorButton->onMouseDown.add([&](int key) {
        GameContainer::getInstance()->setCurrentStage("preview");
    });

    quitButton->onMouseDown.add([&](int key) {
        GameContainer::getInstance()->setShouldClose(true);
    });*/
}


MainMenuGameStage::~MainMenuGameStage()
{
}

void MainMenuGameStage::onDraw()
{
}

void MainMenuGameStage::onSwitchTo()
{
}

void MainMenuGameStage::onSwitchFrom()
{
}

void MainMenuGameStage::onUpdate(double elapsed)
{
}

void MainMenuGameStage::onKeyDown(std::string key)
{
}

void MainMenuGameStage::onKeyUp(std::string key)
{
}

void MainMenuGameStage::onUpdateObject(GameObject * object, double elapsed)
{
}
