#ifndef NEWGAMEMENU_H
#define NEWGAMEMENU_H

#include <functional>

#include "submenus.h"

#include "gui.h"
#include "guistyling.h"
#include "components/createbutton.h"
#include "components/createtoggle.h"
#include "components/worldviewer.h"
#include "components/textinput.h"
#include "../createscene.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"
#include "../common/threads/threadpool.h"
#include "../altplanet/altplanet.h"
#include "../system/async.h"

namespace gui {

using namespace gfx::gui;

struct NewGameMenuState : public GUIStateBase
{
    using PlanetShape = events::GenerateWorldEvent::PlanetShape;
    using PlanetSize  = events::GenerateWorldEvent::PlanetSize;

    PlanetShape planet_shape;
    PlanetSize  planet_size;

    int planet_seed;

    static const int planet_seed_default = 0;

    NewGameMenuState() :
        planet_shape(PlanetShape::Sphere),
        planet_size(PlanetSize::Medium),
        planet_seed(planet_seed_default)
    { /* default constructor */ }
};

void createNewGameMenu(GUI &gui, GUINode &new_game_menu_root)
{
    const GUIFont &font = gui.getDefaultFont();
    const GUIFont &heading_font = gui.getHeadingFont();

    GUINodeHandle newgame_bg_node = new_game_menu_root.addGUINode(
                GUITransform({0.50f, 0.50f}, // position and anchor in middle

                            {SizeSpec(60.0f, Units::Absolute, true),
                             SizeSpec(60.0f, Units::Absolute, true)}));

    newgame_bg_node->addElement( BackgroundElement( styling::colorGuiBase() ) );
    newgame_bg_node->hide();

    GUIStateHandle<NewGameMenuState> state_handle = newgame_bg_node->setState(NewGameMenuState());

    events::Immediate::add_callback<events::NewGameEvent>(
        [newgame_bg_node] (const events::NewGameEvent &evt) { newgame_bg_node->show(); }
    );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [newgame_bg_node] (const events::ToggleMainMenuEvent &evt) { newgame_bg_node->hide(); }
    );

    GUINodeHandle title_node = newgame_bg_node->addGUINode(
        GUITransform( {HorzPos(0.0f, Units::Relative, HorzAnchor::Left),
                       VertPos(0.0f, Units::Relative, VertAnchor::Top)}, {0.15f, 0.10f} ));

    title_node->addElement( TextElement( "New game - World Generation", heading_font));

    createWorldViewer(newgame_bg_node, font,
        {HorzPos(30.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
         VertPos(60.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},

        {SizeSpec(420.0f, Units::Absolute, true),
         SizeSpec(150.0f, Units::Absolute, true)} );

    // Seed
    GUINodeHandle seed_node = textInput(newgame_bg_node, font, GUITransform(
        {HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
         VertPos(60.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},

        {SizeSpec(150.0f, Units::Absolute),
         SizeSpec(30.0f, Units::Absolute)} ),
        [state_handle] (const std::string &s)   // on text input update
        {
            GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
            sw->planet_seed = std::stoi(s);
            /*try { sw->planet_seed = std::stoi(s); }
            catch (...) { sw->planet_seed = NewGameMenuState::planet_seed_default; }*/
        },
        [state_handle] ()                       // on state update
        {
            GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
            return std::to_string(sr->planet_seed);
        },
        "999", // starting text
        12, // max number of characters
        [](int32_t c){ return c>47 && c<58; }); // only accept numeric

    // Toggle world shape
    createToggle(newgame_bg_node, "Disk", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(120.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_shape = NewGameMenuState::PlanetShape::Disk;
                 },
                 [state_handle] ()
                 {
                     GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     return sr->planet_shape == NewGameMenuState::PlanetShape::Disk;
                 });

    createToggle(newgame_bg_node, "Sphere", font,
                 HorzPos(150.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(120.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_shape = NewGameMenuState::PlanetShape::Sphere;
                 },
                 [state_handle] ()
                 {
                      GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                      return sr->planet_shape == NewGameMenuState::PlanetShape::Sphere;
                 });

    createToggle(newgame_bg_node, "Torus", font,
                 HorzPos(270.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(120.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_shape = NewGameMenuState::PlanetShape::Torus;
                 },
                 [state_handle] ()
                 {
                     GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     return sr->planet_shape == NewGameMenuState::PlanetShape::Torus;
                 });


    // Toggle world size
    createToggle(newgame_bg_node, "Small", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(180.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_size = NewGameMenuState::PlanetSize::Small;
                 },
                 [state_handle] ()
                 {
                     GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     return sr->planet_size == NewGameMenuState::PlanetSize::Small;
                 });

    createToggle(newgame_bg_node, "Medium", font,
                 HorzPos(150.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(180.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_size = NewGameMenuState::PlanetSize::Medium;
                 },
                 [state_handle] ()
                 {
                     GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     return sr->planet_size == NewGameMenuState::PlanetSize::Medium;
                 });

    createToggle(newgame_bg_node, "Large", font,
                 HorzPos(270.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(180.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                    GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                    sw->planet_size = NewGameMenuState::PlanetSize::Large;
                 },
                 [state_handle] ()
                 {
                    GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                    return sr->planet_size == NewGameMenuState::PlanetSize::Large;
                 });

    // Bottom buttons
    createButton(newgame_bg_node, "Generate", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                 VertPos(30.0f, Units::Absolute, VertAnchor::Bottom, VertFrom::Bottom),
                 180.0f,
                 [state_handle]()
                 {
                     GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     events::Immediate::broadcast(events::GenerateWorldEvent{sr->planet_shape, sr->planet_size, sr->planet_seed});
                 });

    createButton(newgame_bg_node, "Back", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(30.0f, Units::Absolute, VertAnchor::Bottom, VertFrom::Bottom),
                 180.0f,
                 [](){ events::Immediate::broadcast(events::ToggleMainMenuEvent()); });

    // update the state
    newgame_bg_node->forceStateUpdate();
}

} // namespace gui

#endif // NEWGAMEMENU_H
