#ifndef GFXSCENEMANAGER_H
#define GFXSCENEMANAGER_H

#include "../createscene.h"
#include "../graphics/camera.h"
#include "../graphics/scenenode.h"
#include "../events/immediateevents.h"
#include "../common/macro/debuglog.h"
#include "../common/procedural/boxgeometry.h"
#include "../common/procedural/planegeometry.h"



struct GFXSceneManager
{
    inline GFXSceneManager(gfx::Camera &&cam, gfx::SceneNode &&scene_node);

    gfx::Camera                 mCamera;              // init order important!
    gfx::SceneNode              mGFXSceneRoot;       // main game scene
};

inline GFXSceneManager::GFXSceneManager(gfx::Camera &&cam, gfx::SceneNode &&scene_node) :
    mCamera(std::move(cam)), mGFXSceneRoot(std::move(scene_node))
{
    //ctor

    // register callbacks
    events::Immediate::add_callback<events::CreateSceneEvent>(
        [this] (const events::CreateSceneEvent &evt) {

            const vmath::Vector3 &point_above = evt.scene_creation_info->anchor_pos;
            Ptr::ReadPtr<state::MacroState> scene_data = evt.scene_creation_info->macro_state_ptr;

            // add stuff to scene
            gfx::SceneNodeHandle world_node = mGFXSceneRoot.addSceneNode();
            float cam_view_distance;
            createScene(world_node, scene_data, cam_view_distance);

            // use that point as basis for our 'MicroScene'
            gfx::SceneNodeHandle micro_node = mGFXSceneRoot.addSceneNode();
            micro_node->transform.position = point_above;

            // set the sun light to be on this side of the planet
            vmath::Vector3 local_up = vmath::normalize(scene_data->planet_base_shape->getGradDir(point_above));
            vmath::Vector3 sun_pos = point_above + 50000.0f * local_up;
            gfx::SceneNodeHandle sun_node = mGFXSceneRoot.addSceneNode();
            sun_node->addLight(vmath::Vector4(sun_pos[0], sun_pos[1], sun_pos[2], 1.0f), vmath::Vector4(0.85f, 0.85f, 0.85f, 1.0f));

            DEBUG_LOG("local_up: " << local_up[0] << ", " << local_up[1] << ", " << local_up[2]);
            DEBUG_LOG("point_above: " << point_above[0] << ", " << point_above[1] << ", " << point_above[2]);

            // add a box geometry..
            Procedural::Geometry box = Procedural::boxPlanes(1.0f, 1.0f, 1.0f);
            gfx::SceneNodeHandle player_node = micro_node->addSceneNode();
            player_node->addSceneObject(gfx::Geometry(gfx::Vertices(box.points, box.normals),
                                                      gfx::Primitives(box.triangles)),
                                        gfx::Material(vmath::Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
            vmath::Vector3 box_relative_pos = vmath::Vector3(0.0, 0.0, -6.0f);
            player_node->transform.position = box_relative_pos;

            // set the camera to look at the box
            mCamera.mTransform.lookAt(point_above, point_above + box_relative_pos, local_up);

    });
}

#endif // GFXSCENEMANAGER_H
