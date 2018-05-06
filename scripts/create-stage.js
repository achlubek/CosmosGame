var header = `#pragma once
class AbsGameContainer;
class GameObject;
#include "AbsGameStage.h"
class MainMenuGameStage : public AbsGameStage
{
public:
    MainMenuGameStage(AbsGameContainer* container);
    ~MainMenuGameStage();
    virtual void onDraw() override;
    virtual void onSwitchTo() override;
    virtual void onSwitchFrom() override;
    virtual void onUpdate(double elapsed) override;
private:
    UIText* freeFlightButton;
    UIText* editorButton;
    UIText* quitButton;
    virtual void onUpdateObject(GameObject * object, double elapsed) override;
};`;

var source = `#include "stdafx.h"
#include "MainMenuGameStage.h"
#include "AbsGameContainer.h"
#include "GameContainer.h"
#include "AbsGameStage.h"


MainMenuGameStage::MainMenuGameStage(AbsGameContainer* container)
    : AbsGameStage(container)
{
    
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

}`;

if(process.argc <= 2){
    console.log("Usage node create-stage.js classname");
    return;
}
var name = process.argv[2];

const fs = require('fs');

header = header.replace(/MainMenuGameStage/g, name);
source = source.replace(/MainMenuGameStage/g, name);

fs.writeFile(name + '.h', header, (err) => {
    if (err) throw err;
    console.log('The file 1 has been saved!');
  });
fs.writeFile(name + '.cpp', source, (err) => {
    if (err) throw err;
    console.log('The file 2 has been saved!');
});