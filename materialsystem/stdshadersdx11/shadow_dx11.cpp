#include "BaseVSShader.h"

#include "shadow_ps50.inc"
#include "shadow_vs50.inc"

CREATE_CONSTANT_BUFFER( Shadow )
{
    Vector4D BaseTexCoordTransform[2];
    Vector4D TextureJitter[2];
    vec_t Color[4];
};

BEGIN_VS_SHADER_FLAGS( Shadow, "Help for Shadow", SHADER_NOT_EDITABLE )
	BEGIN_SHADER_PARAMS
	END_SHADER_PARAMS

	SHADER_INIT_PARAMS()
	{
	}

    SHADER_FALLBACK
    {
        return 0;
    }

    SHADER_INIT
    {
        LoadTexture( BASETEXTURE );
    }

    DECLARE_CONSTANT_BUFFER( Shadow )

    SHADER_INIT_GLOBAL
    {
        INIT_CONSTANT_BUFFER( Shadow );
    }

    SHADER_DRAW
    {
        SHADOW_STATE
        {
            EnableAlphaBlending( SHADER_BLEND_ZERO, SHADER_BLEND_SRC_COLOR );

			unsigned int flags = VERTEX_POSITION | VERTEX_COLOR;
			int numTexCoords = 1;
			pShaderShadow->VertexShaderVertexFormat( flags, numTexCoords, 0, 0 );

            SetInternalVertexShaderConstantBuffers();
            SetVertexShaderConstantBuffer( 3, CONSTANT_BUFFER( Shadow ) );

            SetInternalPixelShaderConstantBuffers();
            SetPixelShaderConstantBuffer( 3, CONSTANT_BUFFER( Shadow ) );

            DECLARE_STATIC_VERTEX_SHADER( shadow_vs50 );
			SET_STATIC_VERTEX_SHADER( shadow_vs50 );

            DECLARE_STATIC_PIXEL_SHADER( shadow_ps50 );
			SET_STATIC_PIXEL_SHADER( shadow_ps50 );

            FogToWhite();
        }

        DYNAMIC_STATE
        {
            BindTexture( SHADER_SAMPLER0, BASETEXTURE, FRAME );

            ALIGN16 Shadow_CBuffer_t shadow;
            memset( &shadow, 0, sizeof( Shadow_CBuffer_t ) );

            SetVertexShaderTextureTransform( shadow.BaseTexCoordTransform, BASETEXTURETRANSFORM );
            SetConstantGammaToLinear( shadow.Color, COLOR );

            int nWidth = 16;
			int nHeight = 16;
			ITexture *pTexture = params[BASETEXTURE]->GetTextureValue();
			if (pTexture)
			{
				nWidth = pTexture->GetActualWidth();
				nHeight = pTexture->GetActualHeight();
			}

            Vector4D vecJitter( 1.0 / nWidth, 1.0 / nHeight, 0.0, 0.0 );

            shadow.TextureJitter[0] = vecJitter;
            shadow.TextureJitter[1] = vecJitter;
            shadow.TextureJitter[1].y *= -1.0f;

            UPDATE_CONSTANT_BUFFER( Shadow, shadow );

			MaterialFogMode_t fogType = pShaderAPI->GetSceneFogMode();
			int fogIndex = ( fogType == MATERIAL_FOG_LINEAR_BELOW_FOG_Z ) ? 1 : 0;

            DECLARE_DYNAMIC_VERTEX_SHADER( shadow_vs50 );
			SET_DYNAMIC_VERTEX_SHADER_COMBO( DOWATERFOG, fogIndex );
            SET_DYNAMIC_VERTEX_SHADER( shadow_vs50 );

            DECLARE_DYNAMIC_PIXEL_SHADER( shadow_ps50 );
            SET_DYNAMIC_PIXEL_SHADER_COMBO( PIXELFOGTYPE, pShaderAPI->GetPixelFogCombo() );
			SET_DYNAMIC_PIXEL_SHADER( shadow_ps50 );
        }

        Draw();
    }

END_SHADER