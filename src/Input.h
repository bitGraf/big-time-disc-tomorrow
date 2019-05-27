#pragma once

#include <GLFW/glfw3.h>

#include "stdio.h"

struct keybind {
    float value = 0;
    GLuint keymap = -1;
};

struct Manager {
    keybind move_forward  = {0, GLFW_KEY_W};
    keybind move_backward = {0, GLFW_KEY_S};
    keybind move_left     = {0, GLFW_KEY_A};
    keybind move_right    = {0, GLFW_KEY_D};
	keybind move_strafe_left = { 0, GLFW_KEY_Q };
	keybind move_strafe_right = { 0, GLFW_KEY_E };
    keybind move_strafe   = {0, GLFW_KEY_LEFT_SHIFT};
    keybind move_jump     = {0, GLFW_KEY_SPACE};
};

namespace Input {
    extern Manager manager;

    void update(GLFWwindow* window);
    void checkKey(GLFWwindow* window, keybind* kb);
}
