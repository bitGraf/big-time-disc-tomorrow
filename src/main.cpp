#include "Game.h"
#include "Platform.h"

#include "CollisionDetection.h"

GLFWwindow* createWindow();

char exe_location[MAX_PATH];

int main(int argc, char** argv) {
    CollisionEntity ent1;
    ent1.collisionHull = new SphereHull;
    ent1.collisionHull->radius = 4;
    ent1.position = {0,3,0};

    CollisionEntity ent2;
    ent2.collisionHull = new SphereHull;
    ent2.collisionHull->radius = 2;
    ent2.position = {3,5,0};

    Collision::collisionTest(&ent1, &ent2);

    delete ent1.collisionHull;
    delete ent2.collisionHull;

    //system("pause");
    //return 0;

    printf("\n-------------------------------------------\n\n");
    get_run_location(exe_location);
    change_working_directory(exe_location);
    print_current_working_directory();
    printf("\n-------------------------------------------\n\n");

    GLFWwindow* window = createWindow();
    if (window == NULL) {
        printf("Window failed to create.\n");
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    printf("\n-------------------------------------------\n\n");

    //After GLFW is initialized, init_game
    initialize_game(window);
    printf("\n-------------------------------------------\n\n");
    run_game_loop(window);
    printf("\n-------------------------------------------\n\n");

    printf("Closing glfwWindow.\n");
    glfwTerminate();

    cleanup();
    printf("\n-------------------------------------------\n\n");
}

void ErrorCallback(int, const char* err_str)
{
    printf("GLFW Error: %s\n", err_str);
}

GLFWwindow* createWindow() {
    // Register error callback first
    glfwSetErrorCallback(ErrorCallback);

    printf("Creating GLFW Window.\n");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowInfo.width, windowInfo.height, "petey", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return NULL;
    }

    //set window position
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, mode->width * .32, mode->height / 2 - windowInfo.height / 2);

    return window;
}