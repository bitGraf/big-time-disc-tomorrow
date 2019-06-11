#include "Game.h"
#include "Platform.h"

#include "CollisionDetection.h"

GLFWwindow* createWindow();

char exe_location[MAX_PATH];

int
line_support(vec3 *support, const vec3 d,
    const vec3 a, const vec3 b)
{
    int i = 0;
    if (Vector::dot(a, d) < Vector::dot(b, d)) {
        *support = b; i = 1;
    } else *support = a;
    return i;
}
int
polyhedron_support(vec3 *support, const vec3 d,
    const vec3 *verts, int cnt)
{
    int imax = 0;
    float dmax = Vector::dot(verts[0], d);
    for (int i = 1; i < cnt; ++i) {
        /* find vertex with max dot product in direction d */
        float dot = Vector::dot(verts[i], d);
        if (dot < dmax) continue;
        imax = i, dmax = dot;
    } *support = verts[imax];
    return imax;
}
gjk_result
polyhedron_intersect_capsule(const vec3 *verts, int cnt,
    const vec3 ca, const vec3 cb, float cr)
{
    /* initial guess */
    vec3 d;
    gjk_support s = {0};
    s.a = verts[0];
    s.b = ca;
    d = s.b - s.a;
    int numIts = 0;

    /* run gjk algorithm */
    gjk_simplex gsx = {0};
    while (gjk(&gsx, &s, &d)) {
        vec3 n = -d;
        s.aid = polyhedron_support(&s.a, n, verts, cnt);
        s.bid = line_support(&s.b, d, ca, cb);
        s.a.print(" a = ");
        s.b.print(" b = ");
        d = s.b - s.a;
        numIts++;
    }
    /* check distance between closest points */
    gjk_result res;
    gjk_analyze(&res, &gsx);
    gjk_quad(&res, 1, cr);
    printf("%d iteration\n", numIts);
    return res;
}

int main(int argc, char** argv) {
    CollisionEntity capsule;
    capsule.position = { 0,0,0 };
    capsule.collisionHull = new CapsuleHull;
    CapsuleHull* caps = (CapsuleHull*)capsule.collisionHull;
    caps->a = {0,0,0};
    caps->b = {0,5,0};
    caps->radius = 2;

    CollisionEntity polyhedron;
    polyhedron.position = {5, 3, 0};
    polyhedron.collisionHull = new PolyHull;
    PolyHull* poly = (PolyHull*)polyhedron.collisionHull;
    const int numVerts = 6;
    vec3 verts[numVerts];
    verts[0] = {-2,  0,  0};
    verts[1] = { 2,  0,  0};
    verts[2] = { 0, -2,  0};
    verts[3] = { 0,  2,  0};
    verts[4] = { 0,  0, -2};
    verts[5] = { 0,  0,  2};
    poly->numVerts = numVerts;
    poly->vertices = (vec3*)verts;
    poly->radius = 0;

    CollisionEvent e = Collision::collisionTest(&capsule, &polyhedron);

    if (e.intersect) {
        printf("intersecting %f deep\n", (e.distance));
    } else {
        printf("%f units away\n", (e.distance));
    }
    e.response1.print("p0: ");
    e.response2.print("p1: ");
    printf("%f actual distance.\n", Vector::magnitude(e.response1-e.response2));

    delete capsule.collisionHull;
    delete polyhedron.collisionHull;

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
    glfwSetWindowPos(window, mode->width * .32, mode->height/2 - windowInfo.height/2);

    return window;
}