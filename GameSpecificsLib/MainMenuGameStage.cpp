#include "stdafx.h"
#include "MainMenuGameStage.h"
#include "AbsGameContainer.h"
#include "GameContainer.h"
#include "AbsGameStage.h"


MainMenuGameStage::MainMenuGameStage(AbsGameContainer* container)
    : AbsGameStage(container)
{
    auto ui = getUIRenderer();
    freeFlightButton = new UIText(ui, 0.01, 0.2, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("coolvetica.ttf"), 33, "Free flight");
    ui->addDrawable(freeFlightButton);

    editorButton = new UIText(ui, 0.01, 0.24, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("coolvetica.ttf"), 33, "Celestial bodies preview");
    ui->addDrawable(editorButton);

    quitButton = new UIText(ui, 0.01, 0.28, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("coolvetica.ttf"), 33, "Quit game");
    ui->addDrawable(quitButton);

    freeFlightButton->onMouseDown.add([&](int key) {
        AbsGameContainer::getInstance()->setCurrentStage("freeflight");
    });

    editorButton->onMouseDown.add([&](int key) {
        AbsGameContainer::getInstance()->setCurrentStage("preview");
    });

    quitButton->onMouseDown.add([&](int key) {
        AbsGameContainer::getInstance()->setShouldClose(true);
    });
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

void MainMenuGameStage::onUpdateObject(GameObject * object, double elapsed)
{
}
