#include "Game.h"

void initialize_game(GLFWwindow* window) {
    // Load entities
    Entity::init_entities(windowInfo);
    ModelLoader::loadFile(&axis, "data/models/axis.modl");
    printf("\n\n");

    //Shaders
	shader.smartLoad("simple.vert", "pbr.frag", "entityShader");
	shader.use();
	shader.setMat4("projection", &Entity::manager.camera.projectionMatrix);
	lineShader.smartLoad("axisSymbol.vert", "axisSymbol.frag", "lineShader");
	lineShader.use();
	lineShader.setMat4("projection", &Entity::manager.camera.projectionMatrix);

	//Fonts
	Font::InitTextRendering(windowInfo);
	fpsFont = Font::newDynamicFont("data/fonts/Consolas.ttf", 20);
	otherFont = Font::newDynamicFont("data/fonts/Consolas.ttf", 32);
    sprintf(fpsText, "Fps: 0");

    //
    // Set OpenGL State
    //
    glViewport(0, 0, windowInfo.width, windowInfo.height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, Entity::handleInputEvent);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glClearColor(1, 1, 1, 1.0f);

    //
    // Initialize Input state
    //
    //Input::init();
}

void run_game_loop(GLFWwindow* window) {
    using clock = std::chrono::steady_clock;
	using cycle = std::chrono::duration<double, std::ratio<1, 250>>;

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
			FixedUpdate(PhysicsRate);
			Timer.lag -= PhysicsRate;
			numUpdates++;
		}

        // Update before every render call
		FrameUpdate(Timer.deltaTime);
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
    free(fpsFont);
	free(otherFont);

    Entity::manager.freeAll();
}

void FrameUpdate (double dt) {
    Entity::manager.camera.updateViewMatrix();
	Entity::fixedUpdateAllEntities(PhysicsRate);

    //update fps string every fpsUpdateRate
    if (fpsUpdateTime > fpsUpdateRate) {
        float fps = 1 / Timer.deltaTime;
        sprintf(fpsText, "Fps: %.0f", fps);
        fpsUpdateTime -= fpsUpdateRate;
    }
}

void FixedUpdate(double dt) {
	//Input::update();
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Render normal entities
	shader.use();
	shader.setMat4("view", &Entity::manager.camera.viewMatrix);
	shader.setvec3("camPos", &Entity::manager.camera.position);
	Entity::renderAllEntities(&shader);

	//Render line entities
	glDisable(GL_DEPTH_TEST);
	lineShader.use();
	lineShader.setMat4("view", &Entity::manager.camera.viewMatrix);
	glBindVertexArray(axis.VAO);
	glDrawElements(GL_LINES, axis.numFaces*2, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);

	//Render text
	quat fpsColor = {1, 0, 0, 1};
	Font::drawText(fpsFont, 0, 0, fpsColor, fpsText, ALIGN_TOP_LEFT);

	int b = (int)(otherFont->fontSize / 15.0f);
	if (b < 1) b = 1;

	quat fg_color = {1, 1, 1, 1};
	quat bg_color = {0, 0, 0, .5f};
	Font::drawText(otherFont, windowInfo.width/2 - b, -b, bg_color, "Running...", ALIGN_TOP_MID);
	Font::drawText(otherFont, windowInfo.width/2, 0, fg_color, "Running...", ALIGN_TOP_MID);

	//
	// Render keyboard input
	//

	char kb_text[64];
    float height_padding = 1.0f;
    float dy = fpsFont->fontSize*height_padding;
    int y = -dy;

	quat kb_color = {.1f, .5, .1f, 1};
    
    /*sprintf(kb_text, "m_forward:  %.0f", Input::manager.move_forward.value);
	Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);
    sprintf(kb_text, "m_backward: %.0f", Input::manager.move_backward.value);
	Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);
    sprintf(kb_text, "m_left:     %.0f", Input::manager.move_left.value);
	Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);
    sprintf(kb_text, "m_right:    %.0f", Input::manager.move_right.value);
	Font::drawText(fpsFont, windowInfo.width, y+=dy, kb_color, kb_text, ALIGN_TOP_RIGHT);*/

	numRenders++;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow *window) {	
    //Input::update(window);

	vec3 velocity;
	float speed = 10;
	bool updateMove = false;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		//velocity = velocity + Entity::manager.camera.Forward;//velocity = {0, 0, -1};
		velocity = velocity + Entity::manager.camera.Up;
		updateMove = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		//velocity = velocity - Entity::manager.camera.Forward;//velocity = {0, 0, 1};
		velocity = velocity - Entity::manager.camera.Up;
		updateMove = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		velocity = velocity - Entity::manager.camera.Right;//velocity = {-1, 0, 0};
		updateMove = true;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		velocity = velocity + Entity::manager.camera.Right;//velocity = {1, 0, 0};
		updateMove = true;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
		velocity = velocity + Entity::manager.camera.Forward;
		updateMove = true;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
		velocity = velocity - Entity::manager.camera.Forward;
		updateMove = true;
	}
	if (updateMove) {
		//Vector::normalize(velocity);
		//Entity::manager.camera.position = Entity::manager.camera.position + velocity * (Timer.deltaTime*speed);
		//Entity::manager.camera.position.print("Position: ");

		//Entity::manager.camera.lookAt({0, 0, -3});
		//Entity::manager.camera.updateVectors();
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		Entity::manager.camera.updateOrientation(-100*Timer.deltaTime, 0);
		//printf("Pitch: %.2f\tYaw: %.2f\n", Entity::manager.camera.pitch, Entity::manager.camera.yaw);
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		Entity::manager.camera.updateOrientation(100*Timer.deltaTime, 0);
		//printf("Pitch: %.2f\tYaw: %.2f\n", Entity::manager.camera.pitch, Entity::manager.camera.yaw);
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		Entity::manager.camera.updateOrientation(0, 100*Timer.deltaTime);
		//printf("Pitch: %.2f\tYaw: %.2f\n", Entity::manager.camera.pitch, Entity::manager.camera.yaw);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		Entity::manager.camera.updateOrientation(0, -100*Timer.deltaTime);
		//printf("Pitch: %.2f\tYaw: %.2f\n", Entity::manager.camera.pitch, Entity::manager.camera.yaw);
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

	Entity::manager.camera.updateOrientation(-xoffset * horizontalSensitivity, -yoffset * verticalSensitivity);
}