#include "stdafx.h"
#include "Serializer.h"
#include <cctype>
#include <locale>


Serializer::Serializer(GameContainer* container)
    : container(container)
{
}


Serializer::~Serializer()
{
}

SQLiteDatabase* Serializer::prepareNewDatabase(std::string path)
{
    auto db = openDatabase(path);

    db->query("CREATE TABLE IF NOT EXISTS components (\
        id INTEGER NOT NULL, \
        type INTEGER NOT NULL, \
        data TEXT NOT NULL \
    );");

    db->query("CREATE TABLE IF NOT EXISTS game_objects (\
        id INTEGER NOT NULL, \
        tags TEXT NOT NULL, \
        components TEXT NOT NULL \
    );");

    db->query("CREATE TABLE IF NOT EXISTS state_machines (\
        name TEXT NOT NULL, \
        data TEXT NOT NULL \
    );");

    db->query("CREATE TABLE IF NOT EXISTS global_state (\
        time REAL NOT NULL, \
        components_counter_value INTEGER NOT NULL, \
        game_objects_counter_value INTEGER NOT NULL, \
        camera_target INTEGER NOT NULL \
    );");

    db->nonquery("DELETE FROM components;");
    db->nonquery("DELETE FROM game_objects;");
    db->nonquery("DELETE FROM state_machines;");
    db->nonquery("DELETE FROM global_state;");

    return db;
}

SQLiteDatabase * Serializer::openDatabase(std::string path)
{
    return new SQLiteDatabase(path);
}

void Serializer::closeDatabase(SQLiteDatabase * db)
{
    delete db;
}

void Serializer::serializeFreeFlightGameStage(SQLiteDatabase* db, FreeFlightGameStage* stage)
{
    db->nonquery("BEGIN;");
    auto time = stage->getTimeProvider()->getTime();
    auto componentsCounterValue = AbsComponent::getSharedCounterValue();
    auto gameObjectsCounterValue = GameObject::getSharedCounterValue();
    auto cameraTargetId = stage->getViewCamera()->getTarget()->getID();
    db->nonquery("INSERT INTO global_state (time, components_counter_value, game_objects_counter_value, camera_target) VALUES ("
        + std::to_string(time) + ", " + std::to_string(componentsCounterValue) + ", " + std::to_string(gameObjectsCounterValue) + ", " + std::to_string(cameraTargetId) + ");");

    auto objects = stage->getAllGameObjects();
    for (auto object : objects) {
        serializeGameObject(db, object);
    }
    serializeStateMachine(db, "PlayerMountState", stage->getPlayerMountState());
    db->nonquery("COMMIT;");
}

FreeFlightGameStage * Serializer::deserializeFreeFlightGameStage(SQLiteDatabase* db)
{
    auto stage = new FreeFlightGameStage(container);

    auto data = db->query("SELECT time, components_counter_value, game_objects_counter_value, camera_target FROM global_state");

    auto time = std::stod(data[0]["time"]);
    auto componentsCounterValue = std::stol(data[0]["components_counter_value"]);
    auto gameObjectsCounterValue = std::stol(data[0]["game_objects_counter_value"]);
    auto cameraTargetId = std::stol(data[0]["camera_target"]);

    auto gameObjectsIds = db->query("SELECT id FROM game_objects");
    for (auto row : gameObjectsIds) {
        unsigned long id = std::stol(row["id"]);
        auto object = deserializeGameObject(db, id);
        stage->addObject(object);
    }

    stage->getTimeProvider()->setTime(time);
    AbsComponent::restoreSharedCounterValue(componentsCounterValue);
    GameObject::restoreSharedCounterValue(gameObjectsCounterValue);

    auto cameraTarget = stage->getGameObjectById(cameraTargetId);
    stage->getViewCamera()->setStrategy(cameraTarget->hasTag(GameObjectTags::Player) ? new CameraFirstPersonStrategy() : new CameraFirstPersonStrategy());
    stage->getViewCamera()->setFov(66.0);
    stage->getViewCamera()->setTarget(cameraTarget);

    deserializePlayerMountState(stage, db);

    return stage;
}

void Serializer::serializeStateMachine(SQLiteDatabase* db, std::string name, AbsStateMachine * stateMachine)
{
    db->nonquery("INSERT INTO state_machines (name, data) VALUES ('" + name + "', '" + stateMachine->serialize() + "');");
}

void Serializer::deserializePlayerMountState(FreeFlightGameStage* stage, SQLiteDatabase* db)
{
    auto data = db->query("SELECT data FROM state_machines WHERE name = 'PlayerMountState'");

    auto dataStr = data[0]["data"];

    stage->getPlayerMountState()->deserialize(stage, dataStr);
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

void Serializer::serializeGameObject(SQLiteDatabase* db, GameObject* object)
{
    auto id = object->getID();
    auto tags = object->getAllTags();
    auto components = object->getAllComponents();

    std::stringstream tagsStream, componentsStream;

    for (auto tag : tags) {
        tagsStream << static_cast<int>(tag) << " ";
    }
    for (auto component : components) {
        componentsStream << component->getID() << " ";
        serializeComponent(db, component);
    }

    std::string tagsIdsStr = tagsStream.str();
    std::string componentsIdsStr = componentsStream.str();
    rtrim(tagsIdsStr);
    rtrim(componentsIdsStr);

    db->nonquery("INSERT INTO game_objects (id, tags, components) VALUES (" + std::to_string(id) + ", \"" + tagsIdsStr + "\", \"" + componentsIdsStr + "\");");
}

GameObject * Serializer::deserializeGameObject(SQLiteDatabase* db, unsigned long id)
{
    auto data = db->query("SELECT tags, components FROM game_objects WHERE id = " + std::to_string(id));

    auto tagsStr = data[0]["tags"];
    auto componentsStr = data[0]["components"];

    auto tagsArr = splitByChar(tagsStr, ' ');
    auto componentsArr = splitByChar(componentsStr, ' ');

    auto object = new GameObject();

    for (auto tagStr : tagsArr) {
        auto tag = static_cast<GameObjectTags>(std::stoi(tagStr));
        object->addTag(tag);
    }

    for (auto componentStr : componentsArr) {
        auto component = deserializeComponent(db, std::stoi(componentStr));
        object->addComponent(component);
    }

    object->setID(id);

    return object;
}

void Serializer::serializeComponent(SQLiteDatabase* db, AbsComponent * object)
{
    std::string serialized = object->serialize();
    auto type = object->getType();
    auto id = object->getID();

    db->query("INSERT INTO components (id, type, data) VALUES (" + std::to_string(id) + ", " + std::to_string(static_cast<int>(type)) + ", '" + serialized + "');");
}

AbsComponent * Serializer::deserializeComponent(SQLiteDatabase* db, unsigned long id)
{
    auto data = db->query("SELECT type, data FROM components WHERE id = " + std::to_string(id));
    auto typeStr = data[0]["type"];
    ComponentTypes type = static_cast<ComponentTypes>(std::stoi(typeStr));

    auto dataStr = data[0]["data"];

    if (type == ComponentTypes::Transformation3D) {
        return Transformation3DComponent::deserialize(dataStr);
    }
    else if (type == ComponentTypes::ThrustGenerator) {
        return ThrustGeneratorComponent::deserialize(container->getModel3dFactory(), dataStr);
    }
    else if (type == ComponentTypes::ThrustController) {
        return ThrustControllerComponent::deserialize(dataStr);
    }
    else if (type == ComponentTypes::ShipManualControls) {
        return ShipManualControlsComponent::deserialize(dataStr);
    }
    else if (type == ComponentTypes::Battery) {
        return BatteryComponent::deserialize(dataStr);
    }
    else if (type == ComponentTypes::SimpleDrawable) {
        return AbsDrawableComponent::deserialize(container->getModel3dFactory(), dataStr);
    }
    else if (type == ComponentTypes::PlayerManualControls) {
        return PlayerManualControlsComponent::deserialize(dataStr);
    }
    else if (type == ComponentTypes::ShipAutopilot) {
        return ShipAutopilotComponent::deserialize(dataStr);
    }
    else if (type == ComponentTypes::Focus) {
        return FocusComponent::deserialize(dataStr);
    }

    return nullptr;
}
