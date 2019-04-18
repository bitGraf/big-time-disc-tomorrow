#pragma once

#include <GLFW/glfw3.h>

#include "stdio.h"

struct keybind {
    float value = 0;
    GLuint keymap = -1;
};

struct Manager {
    keybind move_forward  = {0, GLFW_KEY_UP};
    keybind move_backward = {0, GLFW_KEY_DOWN};
    keybind move_left     = {0, GLFW_KEY_LEFT};
    keybind move_right    = {0, GLFW_KEY_RIGHT};
};

namespace Input {
    extern Manager manager;

    void update(GLFWwindow* window);
    void checkKey(GLFWwindow* window, keybind* kb);
}
