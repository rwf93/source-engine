#include "BaseVSShader.h"

#include "shadowmodel_ps50.inc"
#include "shadowmodel_vs50.inc"

CREATE_CONSTANT_BUFFER( ShadowModel )
{
    Vector4D cShadowTextureMatrix[3];
    Vector4D cTexOrigin;
    Vector4D cTexScale;
    Vector4D cShadowConstants;
    Vector4D cModulationColor;
};

BEGIN_VS_SHADER( ShadowModel, "Help for ShadowModel" )
    BEGIN_SHADER_PARAMS
    END_SHADER_PARAMS

    SHADER_INIT
    {
    }

    SHADER_FALLBACK
    {
        return 0;
    }

    DECLARE_CONSTANT_BUFFER( ShadowModel );

    SHADER_INIT_GLOBAL
    {
        INIT_CONSTANT_BUFFER( ShadowModel );
    }

    SHADER_DRAW
    {
        SHADOW_STATE
        {
		    // Base texture on stage 0
		    pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );

		    // Multiplicative blending state...
		    EnableAlphaBlending( SHADER_BLEND_DST_COLOR, SHADER_BLEND_ZERO );

		    int fmt = VERTEX_POSITION | VERTEX_NORMAL;
		    pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );

            SetInternalVertexShaderConstantBuffers();
            SetVertexShaderConstantBuffer( 3, CONSTANT_BUFFER( ShadowModel ) );

		    DECLARE_STATIC_VERTEX_SHADER( shadowmodel_vs50 );
		    SET_STATIC_VERTEX_SHADER( shadowmodel_vs50 );

		    DECLARE_STATIC_PIXEL_SHADER( shadowmodel_ps50 );
		    SET_STATIC_PIXEL_SHADER( shadowmodel_ps50 );

            FogToWhite();
        }

        DYNAMIC_STATE
        {
            BindTexture(SHADER_SAMPLER0, BASETEXTURE, -1);

            DECLARE_DYNAMIC_VERTEX_SHADER( shadowmodel_vs50 );
		    SET_DYNAMIC_VERTEX_SHADER_COMBO( DOWATERFOG, pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z );
		    SET_DYNAMIC_VERTEX_SHADER_COMBO( SKINNING, pShaderAPI->GetCurrentNumBones() > 0 );
		    SET_DYNAMIC_VERTEX_SHADER( shadowmodel_vs50 );

		    DECLARE_DYNAMIC_PIXEL_SHADER( shadowmodel_ps50 );
		    SET_DYNAMIC_PIXEL_SHADER( shadowmodel_ps50 );

            ALIGN16 ShadowModel_CBuffer_t constants;
            UPDATE_CONSTANT_BUFFER( ShadowModel, constants );
        }

        Draw();
    }
END_SHADER