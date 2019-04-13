#include "Input.h"

//Manager Input::manager;

void Input::update(GLFWwindow* window) {
    checkKey(window, &manager.move_forward);
    checkKey(window, &manager.move_backward);
    checkKey(window, &manager.move_left);
    checkKey(window, &manager.move_right);
}

void Input::checkKey(GLFWwindow* window, keybind* kb) {
    if (glfwGetKey(window, kb->keymap) == GLFW_KEY_DOWN) {
        kb->value = 1.0f;
    } else {
        kb->value = 0.0f;
    }
}