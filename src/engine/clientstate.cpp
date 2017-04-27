#include "clientstate.h"

ClientState::ClientState(state::SceneCreationInfo &new_game_info) :
   mGFXSceneManager(gfx::Camera(gfx::PerspectiveProjection(new_game_info.aspect_ratio, DR_M_PI_4, 0.0f, 1000000.0f)),
                    gfx::SceneNode(),
                    Ptr::WritePtr<PhysTransformContainer>(&mActorTransforms)),
   mPhysicsManager(Ptr::WritePtr<PhysTransformContainer>(&mActorTransforms)),
   mMechanicsManager(Ptr::WritePtr<gfx::Camera>(&mGFXSceneManager.mCamera), mPhysicsManager.getActorRigidBodyPoolWPtr()),
   mMacroStatePtr(std::move(new_game_info.macro_state_ptr))
{
    // ctor
    mGFXSceneManager.initScene(new_game_info.point_above, mMacroStatePtr.getReadPtr(), new_game_info.actors);
    mPhysicsManager.initScene(new_game_info.land_pos, mMacroStatePtr.getReadPtr(), new_game_info.actors);
    mMechanicsManager.initScene(new_game_info.land_pos, mMacroStatePtr.getReadPtr(), new_game_info.actors);

}


void ClientState::update(float delta_time_sec)
{
    // update mechanics
    mMechanicsManager.update(delta_time_sec);

    // update physics
    mPhysicsManager.stepPhysicsSimulation(delta_time_sec);

    // update gravity
    mPhysicsManager.updateDynamicsGravity(mMacroStatePtr->planet_base_shape);

    // update render jobs
    mActorTransforms.for_all([](PhysTransform &pt){
        pt.scene_node_hdl->transform.position = pt.pos;
        pt.scene_node_hdl->transform.rotation = pt.rot;
    });

    // update graphics camera
    vmath::Quat player_orientation = mMechanicsManager.getPlayerTargetOrientation();
    mGFXSceneManager.updateCamera(player_orientation, mMacroStatePtr->getLocalUp(getActiveCamera().mTransform.position));
}