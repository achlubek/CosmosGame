#pragma once
#include "Components/ComponentTypes.h"
#include "GameObjectTags.h"
class Serializer
{
public:
    Serializer(GameContainer* container);
    ~Serializer();

    SQLiteDatabase* prepareNewDatabase(std::string path);
    SQLiteDatabase* openDatabase(std::string path);
    void closeDatabase(SQLiteDatabase* db);

    void serializeFreeFlightGameStage(SQLiteDatabase* db, FreeFlightGameStage* stage);
    FreeFlightGameStage* deserializeFreeFlightGameStage(SQLiteDatabase* db);

    void serializeStateMachine(SQLiteDatabase* db, std::string name, AbsStateMachine* stateMachine);
    void deserializePlayerMountState(FreeFlightGameStage* stage, SQLiteDatabase* db);

    void serializeGameObject(SQLiteDatabase* db, GameObject* object);
    GameObject* deserializeGameObject(SQLiteDatabase* db, unsigned long id);

    void serializeComponent(SQLiteDatabase* db, AbsComponent* object);
    AbsComponent* deserializeComponent(SQLiteDatabase* db, unsigned long id);
private:
    GameContainer * container;
};

