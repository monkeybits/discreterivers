#ifndef MACROSTATE_H
#define MACROSTATE_H

#include "../common/gfx_primitives.h"
#include "../altplanet/altplanet.h"

//namespace state {

//namespace macro {

class MacroState {
public:
    std::vector<vmath::Vector3> alt_planet_points;
    std::vector<gfx::Triangle> alt_planet_triangles;

    std::vector<vmath::Vector3> alt_ocean_points;
    std::vector<gfx::Triangle> alt_ocean_triangles;

    std::vector<vmath::Vector3> alt_lake_points;
    std::vector<gfx::Triangle> alt_lake_triangles;
    std::vector<gfx::Line> alt_river_lines;

    std::vector<gfx::TexCoords> alt_planet_texcoords;
    std::vector<gfx::TexCoords> clim_mat_texco;

    const AltPlanet::Shape::BaseShape * planet_base_shape;

    ~MacroState() { delete planet_base_shape; }
};

//}

//}

#endif // MACROSTATE_H