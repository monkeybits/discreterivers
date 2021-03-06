#ifndef SHADER_H
#define SHADER_H

#include "gfxcommon.h"

#include "material.h"
#include "geometry.h"
#include "transform.h"

#include "camera.h"
#include "renderflags.h"
#include "light.h"

namespace gfx {

class Shader {
public:
    Shader();
    ~Shader();

    GLuint getProgramID() const {return mShaderProgramID;}

    struct Uniforms;
    const Uniforms &getUniforms() const {return mUniforms;}

    struct Uniforms
    {
        GLint mv;
        GLint p;
        GLint z_offset;
        //GLint z_near;
        //GLint z_far;
        //GLint C;
        GLint f_coef;
        GLint tex;
        GLint color;
        GLint num_lights;
        GLint light_position_array;
        GLint light_color_array;
    };

    inline void clearDrawObjects() const
    {
        mDrawObjectsVector.clear();
    }

    inline void addDrawObject(vmath::Matrix4 &&world_matrix,
                              Material::DrawData &&material_data,
                              Geometry::DrawData &&geometry_data,
                              RenderFlags render_flags) const
    {
         mDrawObjectsVector.emplace_back(std::move(world_matrix),
                                         std::move(material_data),
                                         std::move(geometry_data),
                                         render_flags);
    }

    inline void clearLightObjects() const
    {
        mLightObjectsVector.clear();
    }

    inline void addLightObject(const vmath::Vector4 &world_pos, const vmath::Vector4 &color) const
    {
        mLightObjectsVector.push_back({world_pos, color});
    }

    inline void drawDrawObjects( const Camera &camera ) const
    {
        drawLights(camera);
        setCamUniforms(camera);

        // actually draw the batched draw objects
        for (const auto &draw_object : mDrawObjectsVector)
        {
            drawDrawObject(draw_object, camera);
        }
    }

    inline void drawLights(const Camera &camera) const;

private:
    class DrawObject
    {
    public:
        /*DrawObject(const vmath::Matrix4 &matrix, const Material::DrawData &material_data,
                   const Geometry::DrawData &geometry_data, RenderFlags render_flags) :
            mMatrix(matrix), mMaterialData(material_data), mGeometryData(geometry_data), mRenderFlags(render_flags) {}*/

        DrawObject(vmath::Matrix4 &&matrix, Material::DrawData &&material_data, Geometry::DrawData &&geometry_data,
                   RenderFlags render_flags) :
            mMatrix(std::move(matrix)), mMaterialData(std::move(material_data)), mGeometryData(std::move(geometry_data)),
            mRenderFlags(render_flags) {}

        vmath::Matrix4 mMatrix;
        Material::DrawData mMaterialData;
        Geometry::DrawData mGeometryData;
        RenderFlags mRenderFlags;

    private:
        DrawObject();
    };

    struct LightObject
    {
        vmath::Vector4 world_pos;
        vmath::Vector4 color;
    };

    inline void drawDrawObject(const DrawObject &draw_object, const Camera &camera) const;
    inline void setCamUniforms(const Camera &camera) const;

    GLuint mShaderProgramID;

    Uniforms mUniforms;

    mutable std::vector<DrawObject>  mDrawObjectsVector;
    mutable std::vector<LightObject> mLightObjectsVector;
};

// inline functions

inline void Shader::drawDrawObject(const DrawObject &draw_object, const Camera &camera) const
{
    // deconstruct scene_object
    // wouldn't it be nice to write for ({transform, {vertices, primitives}, material} : mSceneObjectsVector)... std::tie?
    const auto &model_matrix = draw_object.mMatrix;
    const auto &geometry_data = draw_object.mGeometryData;
    const auto &material_data = draw_object.mMaterialData;

    vmath::Matrix4 mv = camera.getCamMatrixInverse() * model_matrix;

    // combined model view projection matrix
    vmath::Matrix4 p = camera.getProjectionMatrix();

    glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
    glUniformMatrix4fv(mUniforms.p, 1, false, (const GLfloat*)&(p[0]));
    glUniform4fv(mUniforms.color, 1, (const GLfloat*)&material_data.color);
    glUniform1f(mUniforms.z_offset, (const GLfloat)material_data.z_offset);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material_data.texID);

    // Bind vertex array
    glBindVertexArray(geometry_data.vertices.mVertexArrayObject);

    // Bind element array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry_data.primitives.mElementArrayBuffer);

    // wireframe !
    if (draw_object.mRenderFlags.checkFlag(RenderFlags::Wireframe)) // woops branching in tight loop, should fix...
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

    //checkOpenGLErrors("Before draw elements");
    //                                                  | num indices | type of index | wtf is this for?
    glDrawElements(PRIMITIVE_GL_CODE(geometry_data.primitives.mPrimitiveType),
                   geometry_data.primitives.mNumIndices, GL_UNSIGNED_INT, (void*)0 );

    //checkOpenGLErrors("After draw elements");
}

inline void Shader::drawLights(const Camera &camera) const
{
    vmath::Matrix4 v = camera.getCamMatrixInverse();

    int num_lights = std::min((int)(mLightObjectsVector.size()), 10);
    std::array<vmath::Vector4, 10> light_position_array;
    std::array<vmath::Vector4, 10> light_color_array;

    for (int i = 0; i<num_lights; i++)
    {
        light_position_array[i] = v * mLightObjectsVector[i].world_pos;
        light_color_array[i]    = mLightObjectsVector[i].color;
    }

    glUniform1i(mUniforms.num_lights, (GLint)num_lights);
    glUniform4fv(mUniforms.light_position_array, num_lights, (const GLfloat*)&light_position_array);
    glUniform4fv(mUniforms.light_color_array, num_lights, (const GLfloat*)&light_color_array);
}

inline void Shader::setCamUniforms(const Camera &camera) const
{
    float z_far = camera.getZFar();
    float Fcoef = 2.0f / std::log2f(z_far + 1.0f);
    glUniform1f(mUniforms.f_coef, Fcoef);
}


}

#endif // SHADER_H
