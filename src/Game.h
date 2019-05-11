#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h> 	

#include "ShaderProgram.h"
#include "Utils.h"
#include "EntityManager.h"
#include "Input.h"

#include "DynamicFont.h"
#include "Level.h"

#include <thread>
#include <chrono>

static TimeInfo Timer;
static WindowInfo windowInfo;
static bool firstMouse;
static float horizontalSensitivity = 0.0005;
static float verticalSensitivity = 0.0005;

static ShaderProgram shader;
static ShaderProgram lineShader;
static TriangleMesh axis;

static DynamicFont* fpsFont;
static DynamicFont* otherFont;
static char fpsText[32];

static int numRenders = 0;
static int numUpdates = 0;

static const double PhysicsRate = 1 / 60.0;	//60 ups for physics
static const double RenderRate = 1 / 300.0;	//300 fps for rendering	-> max 5 frames per physics update?
										//every loop: render, and calculate the time left, 
										//subtract that from a running counter. When counter is less
										//than zero, run physics update and set counter back to max.

static double fpsUpdateTime = 0;
static double fpsUpdateRate = 0.5;

enum GameStates {
	Normal = 0,
	Menu
};

// Game State Manager
static GLuint currentState = 0;
//void StateUpdate(double dt);
//void StateProcessInput(GLFWwindow *window);
//void StateRender();

//External
void initialize_game(GLFWwindow* window);
void run_game_loop(GLFWwindow* window);

//Internal
void FrameUpdate (double dt);
void FixedUpdate(double dt);

void ProcessInput(GLFWwindow *window);

void Render();

void cleanup();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void handleInputEvent(GLFWwindow* window, int key, int scancode, int action, int mods);