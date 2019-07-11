#include "Game.h"

CollisionEntity* box;
ActorEntity* player;

void initialize_game(GLFWwindow* window) {
    // Load entities
    Entity::init_entities(windowInfo);
    ModelLoader::loadFile(&axis, "../data/models/axis.modl");
    printf("\n\n");

    // VAO/VBO Skybox Info
    float skyboxVerts[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    //printf("SkyboxVAO: %d\n", (int)skyboxVAO);

    GLuint skyboxVBO;
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), skyboxVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Shaders
    shader.smartLoad("simple.vert", "pbr.frag", "entityShader");
    shader.use();
    shader.setMat4("projection", &Entity::manager.camera.projectionMatrix);
    lineShader.smartLoad("axisSymbol.vert", "axisSymbol.frag", "lineShader");
    lineShader.use();
    lineShader.setMat4("projection", &Entity::manager.camera.projectionMatrix);
    Entity::manager.lineShader = &lineShader;
    skyShader.smartLoad("skybox.vert", "skybox.frag", "skyShader");
    skyShader.use();
    skyShader.setMat4("projection", &Entity::manager.camera.projectionMatrix);
    skyShader.setInt("skybox", 0);
    skybox.loadCubeMap("cybercity", ".png");

    //Fonts
    Font::InitTextRendering(windowInfo);
    fpsFont = Font::newDynamicFont("../data/fonts/Consolas.ttf", 20);
    otherFont = Font::newDynamicFont("../data/fonts/Consolas.ttf", 32);
    bigFont = Font::newDynamicFont("../data/fonts/Consolas.ttf", 50);
    sprintf(fpsText, "Fps: 0");

    // Set OpenGL State
    glViewport(0, 0, windowInfo.width, windowInfo.height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, handleInputEvent);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glPointSize(10);
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glClearColor(0.716f, 0.68f, 0.652f, 1.0f);
    //glClearColor(255.0f/255.0f, 248.0f/255.0f, 231.0f/255.0f, 1.0f);//Cosmic Latte, too bright :(

    // Initialize Input state
    //Input::init();

    //Collision testing
    box = (CollisionEntity*)Entity::createNewEntity(ENT_Collision);
    OBBCol* boxCollider = new OBBCol;
    boxCollider->min = {-4, -1, -4};
    boxCollider->max = { 4,  1,  4};
    
    box->collider = boxCollider;
    box->position = {0,1,4};
    box->scale = {4, 1, 4};
    Quaternion::buildFromAxisAngleD(box->orientation, {1,0,0}, 15);
    box->mesh = Resources::manager.getTriMeshResource("box");

    player = (ActorEntity*)Entity::createNewEntity(ENT_Actor);
    CylinderCol* cylinderCollider = new CylinderCol;
    cylinderCollider->y_base = 0;
    cylinderCollider->y_cap  = 1.5f;
    cylinderCollider->r = 1.25f;
    player->collider = cylinderCollider;
    player->mesh = Resources::manager.getTriMeshResource("bot");
    player->position = {0, 5, 0};

    Entity::manager.Player->processInput = false;
    Entity::manager.Player->shouldRender = false;
}

void run_game_loop(GLFWwindow* window) {
    float timeRate = 1.0f;

    printf("Beginning Game Loop...\n");
    glfwSetTime(0);
    Timer.lastFrameEnd = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        // Increment Time.
        Timer.frameStart = glfwGetTime();
        Timer.deltaTime = Timer.frameStart - Timer.lastFrameEnd;
        Timer.lastFrameEnd = Timer.frameStart;
        Timer.lag += Timer.deltaTime;
        fpsUpdateTime += Timer.deltaTime;

        // Poll input BEFORE any update functions
        ProcessInput(window);

        // Ensure fixedUpDate runs at the specified PhysicsRate
        while (Timer.lag >= PhysicsRate) {
            FixedUpdate(PhysicsRate*timeRate);
            Timer.lag -= PhysicsRate;
            numUpdates++;
        }

        // Update before every render call
        FrameUpdate(Timer.deltaTime*timeRate);
        // Render the entire scene
        Render();

        //check and call events
        glfwPollEvents();
        //swap buffers
        glfwSwapBuffers(window);

        // Limit the loop framerate so that frames are not rendered too often.
        auto now = std::chrono::steady_clock::now(); //@codesmell
        double sleepDur = (RenderRate + Timer.frameStart) - glfwGetTime();
        if (sleepDur > 0) {
            unsigned long microSleep = (unsigned long) (sleepDur * 1000000);
            std::this_thread::sleep_until(now + 
                std::chrono::microseconds(microSleep));
        }
    }

    printf("\nGame Loop Ended...\n");
    printf("   Time elapsed:    %.2f seconds.\n", glfwGetTime());
    printf("   Frames Rendered: %d\n", numRenders);
    printf("   Fixed updates:   %d\n", numUpdates);

    return;
}

void cleanup() {
    free(fpsFont);        fpsFont      = NULL;
    free(otherFont);    otherFont = NULL;
    free(bigFont);        bigFont   = NULL;

    Entity::manager.freeAll();
    Resources::manager.cleanup();
}

void FrameUpdate (double dt) {
    if (currentState == GameStates::Normal) {
        Entity::manager.camera.update(dt);
        Entity::fixedUpdateAllEntities(dt);
    } else if (currentState == GameStates::Menu) {
        // update menu
    }

    //update fps string every fpsUpdateRate
    if (fpsUpdateTime > fpsUpdateRate) {
        float fps = 1 / Timer.deltaTime;
        sprintf(fpsText, "Fps: %.0f", fps);
        fpsUpdateTime -= fpsUpdateRate;
    }
}

void FixedUpdate(double dt) {
    if (currentState == GameStates::Normal) {
        // game update
        //Collision::Update();
        bool hit_ground = false;
        vec3 mtv;
        if (gjk(player->collider, box->collider, &mtv)) {
            if (!Vector::isZero(mtv)) {
                float slope = rad2deg(acos(Vector::dot(Vector::normalized(mtv), {0,1,0})));
                if (slope < player->max_stand_slope) {
                    hit_ground = true;
                    player->vel.y = 0;
                    player->grounded = true;
                    player->jumping = false;
                }
            }
        }
        player->collider->position = player->collider->position + mtv;

        //grace period for jumping
        {
            static float grace_timer = 0;
            const float grace_time = 0;
            if (!hit_ground && player->position.y>0) {
                grace_timer += dt;
                if (grace_timer>grace_time*1.5*Vector::magnitude(player->vel)/player->top_speed) {
                    grace_timer = 0;
                    player->grounded = false;
                }
            }
        }

        player->position = player->collider->position;
    } else if (currentState == GameStates::Menu) {
        // menu update
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (currentState == GameStates::Normal) {
        //Render skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyShader.use();
        skyShader.setMat4("view", &Entity::manager.camera.viewMatrix);
        glBindVertexArray(skyboxVAO);
        skybox.bindCube(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        
        //Render normal entities
        shader.use();
        shader.setMat4("view", &Entity::manager.camera.viewMatrix);
        shader.setvec3("camPos", &Entity::manager.camera.position);
        Entity::renderAllEntities(&shader);

        mat4 m;
        Matrix::identity(&m);

        //Render line entities
        glDisable(GL_DEPTH_TEST);
        lineShader.use();
        lineShader.setMat4("view", &Entity::manager.camera.viewMatrix);
        lineShader.setMat4("model", &m);
        glBindVertexArray(axis.VAO);
        glDrawElements(GL_LINES, axis.numFaces*2, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

    } else if (currentState == GameStates::Menu) {
        // Render menu
        int x = windowInfo.width/2;
        int y = windowInfo.height * (1 - .85f);
        quat menuColor = {1,1,1,1};
        quat selectColor = {.5f, .8f, 1, 1};

        // Menu
        Font::drawText(bigFont, x, y, menuColor, "Menu", ALIGN_MID_MID);

        y += 50;

        // Levels
        Font::drawText(otherFont, x, y+=35, currentMenuItem == 0 ? selectColor : menuColor, "Level 1", ALIGN_MID_MID);
        Font::drawText(otherFont, x, y+=35, currentMenuItem == 1 ? selectColor : menuColor, "Level 2", ALIGN_MID_MID);
        Font::drawText(otherFont, x, y+=35, currentMenuItem == 2 ? selectColor : menuColor, "Level 3", ALIGN_MID_MID);
        Font::drawText(otherFont, x, y+=35, currentMenuItem == 3 ? selectColor : menuColor, "Unload level", ALIGN_MID_MID);
        Font::drawText(otherFont, x, y+=35, currentMenuItem == 4 ? selectColor : menuColor, "Quit", ALIGN_MID_MID);
	} else if (currentState == GameStates::Inventory) {
		int x = windowInfo.width / 2;
		int y = windowInfo.height * (1 - .85f);
		quat invColor = { 1,1,1,1 };
		Font::drawText(bigFont, x, y, invColor, "Inventory", ALIGN_MID_MID);
	}

    //Render text
    quat fpsColor = {1, 0, 0, 1};
    Font::drawText(fpsFont, 0, 0, fpsColor, fpsText, ALIGN_TOP_LEFT);

    int b = (int)(otherFont->fontSize / 15.0f);
    if (b < 1) b = 1;

    quat fg_color = {1, 1, 1, 1};
    quat bg_color = {0, 0, 0, .5f};
    Font::drawText(otherFont, windowInfo.width/2 - b, -b, bg_color, (char*)LevelLoader::currentLevel.name.c_str(), ALIGN_TOP_MID);
    Font::drawText(otherFont, windowInfo.width/2, 0, fg_color, (char*)LevelLoader::currentLevel.name.c_str(), ALIGN_TOP_MID);


    // Render keyboard input
    char kb_text[64];
    float height_padding = 1.0f;
    float dy = fpsFont->fontSize*height_padding;
    int y = -dy;

    quat kb_color = {.1f, .5, .1f, 1};
    
    sprintf(kb_text, "m_forward:  %.0f", Input::manager.move_forward.value);
    Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);
    sprintf(kb_text, "m_backward: %.0f", Input::manager.move_backward.value);
    Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);
    sprintf(kb_text, "m_left:     %.0f", Input::manager.move_left.value);
    Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);
    sprintf(kb_text, "m_right:    %.0f", Input::manager.move_right.value);
    Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);

    sprintf(kb_text, "Player: ");
    Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);
    sprintf(kb_text, "Grounded: %s", player->grounded ? "true " : "false");
    Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);
    sprintf(kb_text, "Jumping: %s", player->jumping ? "true " : "false");
    Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);
    sprintf(kb_text, "Jump-Button: %s", player->jump_button_pressed ? "true " : "false");
    Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);

    sprintf(kb_text, "Position: (%.2f,%.2f,%.2f)", player->position.x, player->position.y, player->position.z);
    Font::drawText(fpsFont, 0, 0, kb_color, kb_text, ALIGN_TOP_LEFT);

    numRenders++;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow *window) {    
    Input::update(window);

    if (Input::manager.move_forward.value > 0.5f) {
        player->moveForward();
    }
    if (Input::manager.move_left.value > 0.5f) {
        player->strafeLeft();
    }
    if (Input::manager.move_backward.value > 0.5f) {
        player->moveBackward();
    }
    if (Input::manager.move_right.value > 0.5f) {
        player->strafeRight();
    }
    if (Input::manager.move_jump.value > 0.5f) {
        player->jump();
    }
}

void handleInputEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Entity::handleInputEvent(window, key, scancode, action, mods);

    if (currentState == GameStates::Normal) {
        if ((key == GLFW_KEY_X) && (action == GLFW_PRESS)) {
            glfwSetWindowShouldClose(window, true);
        }
        if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
            currentState = GameStates::Menu;
            currentMenuItem = 0;
        }
		if ((key == GLFW_KEY_I) && (action == GLFW_PRESS)) {
			currentState = GameStates::Inventory;
		}
    } else if (currentState == GameStates::Menu) {
        if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
            currentState = GameStates::Normal;
        }
        if ((key == GLFW_KEY_X) && (action == GLFW_PRESS)) {
            glfwSetWindowShouldClose(window, true);
        }

        if ((key == GLFW_KEY_DOWN) && (action == GLFW_PRESS)) {
            currentMenuItem++;
            if (currentMenuItem == 5) // Set to a "menuMax" variable?
                currentMenuItem = 0;
            //printf("Current Menu Item: %d\n", currentMenuItem);
        }
        if ((key == GLFW_KEY_UP) && (action == GLFW_PRESS)) {
            currentMenuItem--;
            if (currentMenuItem == -1)
                currentMenuItem = 4;
            //printf("Current Menu Item: %d\n", currentMenuItem);
        }

        if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
            //Now check current menu state to see what action to do
            switch (currentMenuItem) {
                case 0: {printf("Load level 1\n");LevelLoader::loadLevel("../data/levels/level2.lvl", "../data/levels/entity_maps/level1.enmp");currentState = GameStates::Normal; } break;
                case 1: {printf("Load level 2\n");LevelLoader::loadLevel("../data/levels/level.lvl", "../data/levels/entity_maps/level2.enmp");currentState = GameStates::Normal;} break;
                case 2: {printf("Load level 3\n");LevelLoader::loadLevel("../data/levels/panelTest.lvl", "../data/levels/entity_maps/level3.enmp");currentState = GameStates::Normal;} break;
                case 3: {printf("Unload level\n");LevelLoader::loadLevel(NULL);currentState = GameStates::Normal;} break;
                case 4: {glfwSetWindowShouldClose(window, true);} break;
            }
        }
	}
	else if (currentState == GameStates::Inventory) {
		if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
			currentState = GameStates::Normal;
			currentInventoryItemX = 0;
			currentInventoryItemY = 0;
		}
		if ((key == GLFW_KEY_DOWN) && (action == GLFW_PRESS)) {
			currentInventoryItemX--;
			if (currentInventoryItemX < 0) // Set to a "menuMax" variable?
				currentInventoryItemX = 0;
		}
		if ((key == GLFW_KEY_UP) && (action == GLFW_PRESS)) {
			currentInventoryItemX++;
			if (currentInventoryItemX > currentInventoryXMax)
				currentInventoryItemX = currentInventoryXMax;
		}
		if ((key == GLFW_KEY_LEFT) && (action == GLFW_PRESS)) {
			currentInventoryItemY--;
			if (currentInventoryItemY < 0) // Set to a "menuMax" variable?
				currentInventoryItemY = 0;
		}
		if ((key == GLFW_KEY_RIGHT) && (action == GLFW_PRESS)) {
			currentInventoryItemY++;
			if (currentInventoryItemY > currentInventoryYMax)
				currentInventoryItemY = currentInventoryYMax;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        windowInfo.lastX = xpos;
        windowInfo.lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - windowInfo.lastX;
    float yoffset = windowInfo.lastY - ypos; // reversed since y-coordinates go from bottom to top

    windowInfo.lastX = xpos;
    windowInfo.lastY = ypos;

    //Entity::manager.camera.updateOrientation(-xoffset * horizontalSensitivity, -yoffset * verticalSensitivity);
}