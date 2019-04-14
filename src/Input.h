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
};

namespace Input {
    extern Manager manager;

    void update(GLFWwindow* window);
    void checkKey(GLFWwindow* window, keybind* kb);
}
