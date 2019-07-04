#include "Actor.h"
#include <assert.h>

void ActorEntity::onCreate() {
    collider = NULL;
    moveable = false;
}

void ActorEntity::update(double dt) {
    //update using new velocity
    EntityBase::update(dt);
}

void ActorEntity::onDestroy() {
    if (collider) {
        free(collider);
        collider = NULL;
    }
}

bool ActorEntity::testCollision(Collider* col, vec3* out_mtv, float* out_slope) {
    assert(this->collider);
    assert(col);
    bool result = false;
    if (col && this->collider) {
        vec3 mtv;
        float slope = 0;
        if(gjk(this->collider, col, &mtv)) {
            slope = rad2deg(acos(Vector::dot(Vector::normalized(mtv), {0,1,0})));
            result = true;
        }
        if (out_mtv)    *out_mtv = mtv;
        if (out_slope)  *out_slope = slope;
    }

    return result;
}