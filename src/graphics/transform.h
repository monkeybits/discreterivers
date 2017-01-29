#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../common/gfx_primitives.h"

namespace gfx {

struct Transform
{
    Transform() : position(0.0f, 0.0f, 0.0f), rotation(vmath::Quat::identity()), scale(1.0f, 1.0f, 1.0f) {}
    Transform(const Transform &t) : position(t.position), rotation(t.rotation), scale(t.scale) {}

    vmath::Vector3 position;
    vmath::Quat rotation;
    vmath::Vector3 scale;

    inline vmath::Matrix4 getTransformMatrix() const
    {
        return vmath::Matrix4::translation(position) * vmath::Matrix4::rotation(rotation) * vmath::Matrix4::scale(scale);
    }

    inline vmath::Vector3 getForwardDir() const
    {
        vmath::Vector4 v4 = getTransformMatrix() * vmath::Vector4{0.0f, 0.0f, -1.0f, 0.0f};
        return vmath::Vector3(v4[0], v4[1], v4[2]);
    }

    inline vmath::Vector3 getRightDir() const
    {
        vmath::Vector4 v4 = getTransformMatrix() * vmath::Vector4{1.0f, 0.0f, 0.0f, 0.0f};
        return vmath::Vector3(v4[0], v4[1], v4[2]);
    }

    inline vmath::Vector3 getUpDir() const
    {
        vmath::Vector4 v4 = getTransformMatrix() * vmath::Vector4{0.0f, 1.0f, 0.0f, 0.0f};
        return vmath::Vector3(v4[0], v4[1], v4[2]);
    }
};

}

#endif // TRANSFORM_H
