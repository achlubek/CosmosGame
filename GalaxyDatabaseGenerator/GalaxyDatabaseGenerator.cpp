// GalaxyDatabaseGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GalaxyGenerator.h"
#include "SQLiteDatabase.h"

#define asstring(a) std::to_string(a)
#define asdouble(a) std::stod(a)
#define asint(a) std::stoi(a)
#define asint64(a) std::stoll(a)
#define asuint64(a) std::stoull(a)

int64_t getLastId(SQLiteDatabase &db) {
    return asint64(db.query("select last_insert_rowid() as result;")[0]["result"]);
}

int main()
{
    SQLiteDatabase db = SQLiteDatabase("galaxy.db");
    db.query("CREATE TABLE IF NOT EXISTS stars (\
        id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
        x INTEGER NOT NULL, \
        y INTEGER NOT NULL, \
        z INTEGER NOT NULL, \
        seed INTEGER NOT NULL, \
        radius REAL NOT NULL, \
        color_r REAL NOT NULL, \
        color_g REAL NOT NULL, \
        color_b REAL NOT NULL, \
        age REAL NOT NULL, \
        spots_intensity REAL NOT NULL, \
        rotation_speed REAL NOT NULL, \
        orbit_plane_x REAL NOT NULL, \
        orbit_plane_y REAL NOT NULL, \
        orbit_plane_z REAL NOT NULL \
    );");

    db.query("CREATE TABLE IF NOT EXISTS planets (\
        id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
        starid INTEGER NOT NULL REFERENCES stars(id), \
        radius REAL NOT NULL, \
        terrain_max REAL NOT NULL, \
        fluid_max REAL NOT NULL, \
        star_distance REAL NOT NULL, \
        habitable_chance REAL NOT NULL, \
        orbit_speed REAL NOT NULL, \
        base_color_r REAL NOT NULL, \
        base_color_g REAL NOT NULL, \
        base_color_b REAL NOT NULL, \
        atmosphere_height REAL NOT NULL, \
        atmosphere_absorption_strength REAL NOT NULL, \
        atmosphere_absorption_r REAL NOT NULL, \
        atmosphere_absorption_g REAL NOT NULL, \
        atmosphere_absorption_b REAL NOT NULL \
    );");

    db.query("CREATE TABLE IF NOT EXISTS moons (\
        id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
        starid INTEGER NOT NULL REFERENCES stars(id), \
        planetid INTEGER NOT NULL REFERENCES planets(id), \
        radius REAL NOT NULL, \
        terrain_max REAL NOT NULL, \
        fluid_max REAL NOT NULL, \
        planet_distance REAL NOT NULL, \
        habitable_chance REAL NOT NULL, \
        orbit_speed REAL NOT NULL, \
        base_color_r REAL NOT NULL, \
        base_color_g REAL NOT NULL, \
        base_color_b REAL NOT NULL, \
        atmosphere_height REAL NOT NULL, \
        atmosphere_absorption_strength REAL NOT NULL, \
        atmosphere_absorption_r REAL NOT NULL, \
        atmosphere_absorption_g REAL NOT NULL, \
        atmosphere_absorption_b REAL NOT NULL, \
        orbit_plane_x REAL NOT NULL, \
        orbit_plane_y REAL NOT NULL, \
        orbit_plane_z REAL NOT NULL \
    );");

    db.query("BEGIN;");
        
    GalaxyGenerator* gen = new GalaxyGenerator();
    int64_t galaxyedge = 12490000000;
    int64_t galaxythickness = 1524900000;
    
    uint32_t seed = 0;
    for (uint32_t seed = 0; seed < 100000; seed++) {
        auto system = gen->generateStar(galaxyedge, galaxythickness, 1.0, seed);
        db.query("INSERT INTO stars (x,y,z,seed,radius,color_r,color_g,color_b,age,spots_intensity,rotation_speed ,orbit_plane_x,orbit_plane_y,orbit_plane_z) values (\
            " + asstring(system.star.x) + ",\
            " + asstring(system.star.y) + ",\
            " + asstring(system.star.z) + ",\
            " + asstring(system.star.seed) + ",\
            " + asstring(system.star.radius) + ",\
            " + asstring(system.star.color.x) + ",\
            " + asstring(system.star.color.y) + ",\
            " + asstring(system.star.color.z) + ",\
            " + asstring(system.star.age) + ",\
            " + asstring(system.star.spotsIntensity) + ",\
            " + asstring(system.star.rotationSpeed) + ",\
            " + asstring(system.star.orbitPlane.x) + ",\
            " + asstring(system.star.orbitPlane.y) + ",\
            " + asstring(system.star.orbitPlane.z) + "\
        );");
        auto starid = getLastId(db);
        if (seed % 1000 == 0)printf("Currently at star ID %d\n", starid);
        for (int pid = 0; pid < system.planets.size(); pid++) {
            auto planet = system.planets[pid];
            db.query("INSERT INTO planets (starid,radius,terrain_max,fluid_max,star_distance,habitable_chance,orbit_speed,base_color_r,base_color_g,base_color_b,\
            atmosphere_height,atmosphere_absorption_strength,atmosphere_absorption_r,atmosphere_absorption_g,atmosphere_absorption_b) values (\
                " + asstring(starid) + ",\
                " + asstring(planet.radius) + ",\
                " + asstring(planet.terrainMaxLevel) + ",\
                " + asstring(planet.fluidMaxLevel) + ",\
                " + asstring(planet.starDistance) + ",\
                " + asstring(planet.habitableChance) + ",\
                " + asstring(planet.orbitSpeed) + ",\
                " + asstring(planet.preferredColor.x) + ",\
                " + asstring(planet.preferredColor.y) + ",\
                " + asstring(planet.preferredColor.z) + ",\
                " + asstring(planet.atmosphereRadius) + ",\
                " + asstring(planet.atmosphereAbsorbStrength) + ",\
                " + asstring(planet.atmosphereAbsorbColor.x) + ",\
                " + asstring(planet.atmosphereAbsorbColor.y) + ",\
                " + asstring(planet.atmosphereAbsorbColor.z) + "\
            );");
            auto planetid = getLastId(db);
            for (int mid = 0; mid < system.moons.size(); mid++) {
                auto moon = system.moons[mid];
                if (moon.host.planetId == planet.planetId) {
                    db.query("INSERT INTO moons (starid,planetid,radius,terrain_max,fluid_max,planet_distance,habitable_chance,orbit_speed,base_color_r,base_color_g,base_color_b,\
                    atmosphere_height,atmosphere_absorption_strength,atmosphere_absorption_r,atmosphere_absorption_g,atmosphere_absorption_b,orbit_plane_x,orbit_plane_y,orbit_plane_z) values (\
                        " + asstring(starid) + ",\
                        " + asstring(planetid) + ",\
                        " + asstring(moon.radius) + ",\
                        " + asstring(moon.terrainMaxLevel) + ",\
                        " + asstring(moon.fluidMaxLevel) + ",\
                        " + asstring(moon.planetDistance) + ",\
                        " + asstring(moon.habitableChance) + ",\
                        " + asstring(moon.orbitSpeed) + ",\
                        " + asstring(moon.preferredColor.x) + ",\
                        " + asstring(moon.preferredColor.y) + ",\
                        " + asstring(moon.preferredColor.z) + ",\
                        " + asstring(moon.atmosphereRadius) + ",\
                        " + asstring(moon.atmosphereAbsorbStrength) + ",\
                        " + asstring(moon.atmosphereAbsorbColor.x) + ",\
                        " + asstring(moon.atmosphereAbsorbColor.y) + ",\
                        " + asstring(moon.atmosphereAbsorbColor.z) + ",\
                        " + asstring(moon.orbitPlane.x) + ",\
                        " + asstring(moon.orbitPlane.y) + ",\
                        " + asstring(moon.orbitPlane.z) + "\
                    );");
                }
            }
        }
    }
    printf("Generating indexes\n");
    db.query("CREATE INDEX IF NOT EXISTS pos_star ON stars(x,y,z);");
    db.query("CREATE INDEX IF NOT EXISTS planet_host ON planets(starid);");
    db.query("CREATE INDEX IF NOT EXISTS moon_host_planet ON moons(planetid);");
    db.query("CREATE INDEX IF NOT EXISTS moon_host_star ON moons(starid);");
    printf("Commmiting\n");
    db.query("COMMIT;");

    return 0;
}

