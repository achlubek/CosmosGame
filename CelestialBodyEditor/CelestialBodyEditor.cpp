#include "stdafx.h"

int main()
{
    printf("1");
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");

    printf("2");
    INIReader* configreader = new INIReader("settings.ini");
    auto vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"));

    auto database = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(database);

    UIRenderer* ui = new UIRenderer(vulkanToolkit, configreader->geti("window_width"), configreader->geti("window_height"));

    auto cosmosRenderer = new CosmosRenderer(vulkanToolkit, galaxy, ui->outputImage, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);
    
    Mouse* mouse = new Mouse(vulkanToolkit->window);
    Keyboard* keyboard = new Keyboard(vulkanToolkit->window);

    keyboard->onKeyPress.add([&](int key) {
        if (key == GLFW_KEY_PAUSE) cosmosRenderer->recompileShaders(true);
    });


    Camera * cam = new Camera();
    cam->createProjectionPerspective(90.0f, configreader->geti("window_width") / configreader->geti("window_height"), 0.01f, 9000000.0f);

    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStarsBuffer();

    while (!glfwWindowShouldClose(vulkanToolkit->window)) {
        ui->draw();
        cosmosRenderer->updateCameraBuffer(cam, glm::dvec3(0.0));
        cosmosRenderer->draw();
        vulkanToolkit->poolEvents();
    }

    return 0;
}

