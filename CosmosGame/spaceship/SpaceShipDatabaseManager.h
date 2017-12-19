#pragma once
class SpaceShip;
class SQLiteDatabase;
class Model3d;
class SpaceShipDatabaseManager
{
public:
    SpaceShipDatabaseManager(SQLiteDatabase* db);
    ~SpaceShipDatabaseManager();
    SQLiteDatabase* db;
    SpaceShip* readSpaceShip(int id);
    Model3d* readModel3d(int id);
};

