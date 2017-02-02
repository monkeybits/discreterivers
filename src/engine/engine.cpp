#include "engine.h"

namespace engine {

Engine::Engine(int w, int h, int dpi) :
    mRenderer(w, h),
    mGUI(w, h, dpi),

    // to be moved
    camera(w, h),
    mCameraController(&camera),
    mGUICapturedMouse(false)

{
    gui::createGUI(mGUI);

    // to be moved
    camera.mTransform.position = vmath::Vector3(0.0, 0.0, 10.0);
}


void Engine::handleKeyboardState(const Uint8 *keyboard_state)
{
    // is there any point in trying to eliminate the branches here?
    // or is that taken care of by the optimizer anyway?
    if (keyboard_state[SDL_SCANCODE_LSHIFT])
    {
        mCameraController.sendSignal(mech::CameraController::SpeedUp);
    }
    if (keyboard_state[SDL_SCANCODE_W])
    {
        mCameraController.sendSignal(mech::CameraController::Forward);
    }
    if (keyboard_state[SDL_SCANCODE_S])
    {
        mCameraController.sendSignal(mech::CameraController::Backward);
    }
    if (keyboard_state[SDL_SCANCODE_A])
    {
        mCameraController.sendSignal(mech::CameraController::Left);
    }
    if (keyboard_state[SDL_SCANCODE_D])
    {
        mCameraController.sendSignal(mech::CameraController::Right);
    }
    if (keyboard_state[SDL_SCANCODE_X])
    {
        mCameraController.sendSignal(mech::CameraController::Down);
    }
    if (keyboard_state[SDL_SCANCODE_Z])
    {
        mCameraController.sendSignal(mech::CameraController::Up);
    }
}

void Engine::handleKeyPressEvents(SDL_Keycode k)
{
    switch(k)
    {

    case(SDLK_f):
        mRenderer.toggleWireframe();
        break;
    case(SDLK_h):
        //alt_planet_triangles_so->toggleVisible();
        break;
    case(SDLK_u): {
        // do an iteration of repulsion
//                                AltPlanet::pointsRepulse(alt_planet_points, planet_shape, 0.003f);

//                                // update the scene object geometry
//                                std::vector<vmath::Vector4> position_data;
//                                std::vector<gfx::Point> primitives_data;

//                                for (int i = 0; i < alt_planet_points.size(); i++)
//                                {
//                                    position_data.push_back((const vmath::Vector4&)(alt_planet_points[i]));
//                                    position_data.back().setW(1.0f);
//                                    primitives_data.push_back({i});
//                                }

//                                gfx::Vertices vertices = gfx::Vertices(position_data, position_data /*, texcoords*/);
//                                gfx::Primitives primitives = gfx::Primitives(primitives_data);

//                                alt_planet_points_so->mGeometry = gfx::Geometry(vertices, primitives);
        break;
    }
    case (SDLK_F11): {
        events::Queued::emitEvent(events::Queued::ToggleFullscreenEvent());
        break;
    }
    case(SDLK_ESCAPE):
        // done = true;
        events::Immediate::broadcast(events::ToggleMainMenuEvent());
        break;
    } // switch k
}

void Engine::handleMouseEvent(const SDL_Event &event)
{
    switch(event.type)
    {
        case SDL_MOUSEBUTTONDOWN: {
            mMouseState.lmb_down = event.button.button == SDL_BUTTON_LEFT;
            mMouseState.rmb_down = event.button.button == SDL_BUTTON_RIGHT;
            if (mMouseState.lmb_down)
            {
                mGUICapturedMouse = mGUI.handleMouseClick(event.button.x, event.button.y);
                std::cout << "mouse captured: " << mGUICapturedMouse << std::endl;
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            mMouseState.lmb_down = event.button.button == SDL_BUTTON_LEFT ? false : mMouseState.lmb_down;
            mMouseState.rmb_down = event.button.button == SDL_BUTTON_RIGHT ? false : mMouseState.rmb_down;
            mGUICapturedMouse = false;
            break;
        }
        case SDL_MOUSEMOTION: {
            if ((mMouseState.lmb_down || mMouseState.rmb_down) && !mGUICapturedMouse) {
                int32_t mouse_delta_x = mMouseState.prev_mouse_x - event.motion.x;
                int32_t mouse_delta_y = mMouseState.prev_mouse_y - event.motion.y;
                float mouse_angle_x = static_cast<float>(mouse_delta_x)*0.0062832f; // 2π/1000?
                float mouse_angle_y = static_cast<float>(mouse_delta_y)*0.0062832f;

                mCameraController.sendTurnSignals({mouse_angle_x, mouse_angle_y});
            }

            mGUI.handleMouseMoved(event.motion.x, event.motion.y);

            // update previous mouse position
            mMouseState.prev_mouse_x = event.motion.x;
            mMouseState.prev_mouse_y = event.motion.y;
            break;
        }
        case SDL_MOUSEWHEEL: {
            //camera.mTransform.scale -= vmath::Vector3(0.05f*event.wheel.y);
            break;
        }
    }
}

void Engine::update()
{
    mCameraController.update();
}


} // namespace Engine