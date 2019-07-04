#include "Game.h"
#include "Platform.h"

#include "GJK.h"

GLFWwindow* createWindow();

char exe_location[MAX_PATH];

int main(int argc, char** argv) {
    CapsuleHull capsule;
    PolyHull poly;

    capsule.va = {0,0,0};
    capsule.vb = {0,3,0};
    capsule.hullRadius = 1.0f;
    vec3 capsulePos = {0,0,0};

    poly.numVerts = 6;
    poly.vertices = (vec3*)malloc(poly.numVerts*sizeof(vec3));
    poly.vertices[0] = {0,1,0};
    poly.vertices[1] = {1,0,1};
    poly.vertices[2] = {1,0,-1};
    poly.vertices[3] = {-1,0,1};
    poly.vertices[4] = {-1,0,-1};
    poly.vertices[5] = {0,-1,0};
    vec3 polyPos = {0,4.25f,0};

    SphereHull ball1;
    ball1.hullRadius = 4.0f;
    vec3 ball1Pos = {1,1,1};
    SphereHull ball2;
    ball2.hullRadius = 2.0f;
    vec3 ball2Pos = {0,5,0};

    vec3 d = {1, 0, 0}; //initial guess

    GJK_Struct gjk_struct;
    GJK_SupportPoint p;

    p.aid = ball1.supportPoint(d, &p.a);
    p.a = p.a + ball1Pos + Vector::normalized(d)*ball1.hullRadius;
    p.bid = ball2.supportPoint(-d, &p.b);
    p.b = p.b + ball2Pos + Vector::normalized(-d)*ball2.hullRadius;
    p.P = p.a - p.b;
    
    p.a.print("a = ");
    p.b.print("b = ");
    p.P.print("P = ");
//
    d = -p.P;
    d.print("\nd = ");

    p.aid = ball1.supportPoint(d, &p.a);
    p.a = p.a + ball1Pos + Vector::normalized(d)*ball1.hullRadius;
    p.bid = ball2.supportPoint(-d, &p.b);
    p.b = p.b + ball2Pos + Vector::normalized(-d)*ball2.hullRadius;
    p.P = p.a - p.b;
    p.a.print("\na = ");
    p.b.print("b = ");
    p.P.print("P = ");

    return 0;

    while(gjk_iteration(&gjk_struct, p, &d, true)) {
        p.aid = ball1.supportPoint(d, &p.a);
        p.a = p.a + ball1Pos + Vector::normalized(d)*ball1.hullRadius;
        p.bid = ball2.supportPoint(-d, &p.b);
        p.b = p.b + ball2Pos + Vector::normalized(-d)*ball2.hullRadius;
        p.P = p.a - p.b;
    }
    gjk_processResults(&gjk_struct);
    //gjk_processRadius(&gjk_struct, poly.hullRadius, capsule.hullRadius);
    printf("%d GJK iterations\n", gjk_struct.iteration);

    if (gjk_struct.hit) {
        printf("Intersection, proceeding to EPA.\n");
        gjk_struct.simplex[0].P.print("a = ");
        gjk_struct.simplex[1].P.print("b = ");
        gjk_struct.simplex[2].P.print("c = ");
        gjk_struct.simplex[3].P.print("d = ");
        EPA_Struct epa_struct;
        EPA_Face face;
        if (!epa_seed(&epa_struct, &gjk_struct, &face)) {
            printf("Failed to seed EPA from GJK results.\n");
        } else {
            d = face.normal;

            p.aid = ball1.supportPoint(d, &p.a);
            p.a = p.a + ball1Pos + Vector::normalized(d)*ball1.hullRadius;
            p.bid = ball2.supportPoint(-d, &p.b);
            p.b = p.b + ball2Pos + Vector::normalized(-d)*ball2.hullRadius;
            p.P = p.a - p.b;

            while (epa_iteration(&epa_struct, &face, p)) {
                d = face.normal;

                p.aid = ball1.supportPoint(d, &p.a);
                p.a = p.a + ball1Pos + Vector::normalized(d)*ball1.hullRadius;
                p.bid = ball2.supportPoint(-d, &p.b);
                p.b = p.b + ball2Pos + Vector::normalized(-d)*ball2.hullRadius;
                p.P = p.a - p.b;
            }

            epa_struct.contactPoint.P.print("Vector: ");
        }
    } else {
        printf("Shapes are %.3f units apart.\n", gjk_struct.distance);
        gjk_struct.P0.print("P0: ");
        gjk_struct.P1.print("P1: ");
    }

    capsule.cleanup();
    poly.cleanup();

    system("pause");
    return 0;
    /*
    CollisionEntity ent1;
    ent1.collisionHull = new SphereHull;
    ((SphereHull*)ent1.collisionHull)->radius = 4;
    ent1.position = {0,3,0};

    CollisionEntity ent2;
    ent2.collisionHull = new SphereHull;
    ((SphereHull*)ent2.collisionHull)->radius = 2;
    ent2.position = {3,5,0};

    CollisionEvent e = Collision::collisionTest(&ent1, &ent2);

    printf("First test.\n");
    if (e.GJK_Converged) {
        printf("GJK succesful.\n");
        if (e.intersect) {
            printf("Shapes are intersecting.\n");
            if (e.EPA_Converged) {
                printf("EPA succesful\n");
                printf("Shapes are %.4f units intersecting along ", e.distance);
                e.response.print();

                //shift both in opposite direction
                ent1.position = ent1.position - (e.response*e.distance*.525f);
                ent2.position = ent2.position + (e.response*e.distance*.525f);
            }
        }
        else {
            printf("Shapes are %.4e apart.\n", e.distance);
        }
    }

    printf("Second test.\n");
    e = Collision::collisionTest(&ent1, &ent2);

    if (e.GJK_Converged) {
        printf("GJK succesful.\n");
        if (e.intersect) {
            printf("Shapes are intersecting.\n");
            if (e.EPA_Converged) {
                printf("EPA succesful\n");
                printf("Shapes are %.4f units intersecting along ", e.distance);
                e.response.print();
            }
        }
        else {
            printf("Shapes are %.4e apart.\n", e.distance);
        }
    }

    delete ent1.collisionHull;
    delete ent2.collisionHull;

    //system("pause");
    return 0;
    */

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