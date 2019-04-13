#include "Player.h"

void PlayerEnt::handleInput(int key, int scancode, int action, int mods) {
    vel = {0, 0, 0};

    if (key == movement_forward)
        vel = {0, 0, 1};
    if (key == movement_left)
        vel = {-1, 0, 0};
    if (key == movement_backward)
        vel = {0, 0, -1};
    if (key == movement_right)
        vel = {1, 0, 0};
}

void PlayerEnt::update(double dt) {
    angle += 90 * dt;

    if (angle > 360)
        angle = 0;

    Quaternion::buildFromAxisAngleD(orientation, {0, 1, 0}, angle);
    
    //vec3 Forward = {0, 0, -1};
    //vec3 Right = {1, 0, 0};
    //vec3 vel = (Forward * forwardBackward) + 
    //            (Right   * rightLeft);
    position = position + (vel * dt * speed);

    Matrix::buildFromTRS(&modelMatrix, position, orientation, scale);
    vel = {0, 0, 0};
}
