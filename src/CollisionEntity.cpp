#include "CollisionEntity.h"
#include <assert.h>

void CollisionEntity::onCreate() {
    collider = NULL;
    moveable = false;
}

void CollisionEntity::update(double dt) {
    collider->position = position;
    collider->matRS_inverse = {Left.x, Up.x, Forward.x, Left.y, Up.y, Forward.y, Left.z, Up.z, Forward.z};
    collider->matRS         = {Left.x, Left.y, Left.z, Up.x, Up.y, Up.z, Forward.x, Forward.y, Forward.z};

    EntityBase::update(dt);
}

void CollisionEntity::onDestroy() {
    if (collider) {
        free(collider);
        collider = NULL;
    }
}

bool CollisionEntity::testCollision(Collider* col, vec3* out_mtv, float* out_slope) {
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