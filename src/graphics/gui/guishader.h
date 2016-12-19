#ifndef GUISHADER_H
#define GUISHADER_H

#include "../gfxcommon.h"

#include "../material.h"
#include "../geometry.h"
#include "../transform.h"

#include "../camera.h"
#include "../renderflags.h"

namespace gfx {

class GUIShader {
public:
    GUIShader();
    ~GUIShader();

    GLuint getProgramID() const {return mShaderProgramID;}

    struct Uniforms;
    const Uniforms &getUniforms() const {return mUniforms;}

    struct Uniforms
    {
        GLint mv;
        GLint p;
        GLint tex;
        GLint color;
        GLint light_position;
        GLint light_color;
    };

    inline void clearDrawObjects() const { mDrawObjectsVector.clear(); }

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

    inline void drawDrawObjects( const Camera &camera ) const
    {
        // actually draw the batched draw objects
        for (const auto &draw_object : mDrawObjectsVector)
        {
            drawDrawObject(draw_object, camera, false);
        }
    }

private:
    class DrawObject
    {
    public:
        DrawObject(const vmath::Matrix4 &matrix, const Material::DrawData &material_data,
                   const Geometry::DrawData &geometry_data, RenderFlags render_flags) :
            mMatrix(matrix), mMaterialData(material_data), mGeometryData(geometry_data), mRenderFlags(render_flags) {}

        /*DrawObject(vmath::Matrix4 &&matrix, Material::DrawData &&material_data, Geometry &&geometry,
                   RenderFlags &&render_flags) :
            mMatrix(std::move(matrix)), mMaterialData(std::move(material_data)), mGeometry(std::move(geometry)),
            mRenderFlags(render_flags) {}*/

        vmath::Matrix4 mMatrix;
        Material::DrawData mMaterialData;
        Geometry::DrawData mGeometryData;
        RenderFlags mRenderFlags;

    private:
        DrawObject();
    };

    inline void drawDrawObject(const DrawObject &draw_object, const Camera &camera, bool global_wireframe = false) const;

    GLuint mShaderProgramID;

    Uniforms mUniforms;

    mutable std::vector<DrawObject> mDrawObjectsVector;
};

// inline functions

inline void GUIShader::drawDrawObject(const DrawObject &draw_object, const Camera &camera, bool global_wireframe) const
{
    // deconstruct scene_object
    // wouldn't it be nice to write for ({transform, {vertices, primitives}, material} : mSceneObjectsVector)... std::tie?
    const auto &model_matrix = draw_object.mMatrix;
    const auto &geometry_data = draw_object.mGeometryData;
    const auto &material_data = draw_object.mMaterialData;

    vmath::Matrix4 mv = camera.getCamMatrixInverse() * model_matrix;

    // combined model view projection matrix
    vmath::Matrix4 p = camera.mProjectionMatrix;

    glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
    glUniformMatrix4fv(mUniforms.p, 1, false, (const GLfloat*)&(p[0]));
    glUniform4fv(mUniforms.color, 1, (const GLfloat*)&material_data.color);

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

    checkOpenGLErrors("Before draw elements");
    //                                                  | num indices | type of index | wtf is this for?
    glDrawElements(PRIMITIVE_GL_CODE(geometry_data.primitives.mPrimitiveType),
                   geometry_data.primitives.mNumIndices, GL_UNSIGNED_INT, (void*)0 );

    checkOpenGLErrors("After draw elements");
}


}

#endif // GUISHADER_H
