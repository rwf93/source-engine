#include "BaseVSShader.h"

#include "depthwrite_ps50.inc"
#include "depthwrite_vs50.inc"

CREATE_CONSTANT_BUFFER( DepthWrite )
{
    Vector4D cMorphDimensions;
	Vector4D cMorphSubrect;
};

BEGIN_VS_SHADER_FLAGS( DepthWrite, "Help for Depth Write", SHADER_NOT_EDITABLE )
    BEGIN_SHADER_PARAMS
		SHADER_PARAM( ALPHATESTREFERENCE, SHADER_PARAM_TYPE_FLOAT, "", "Alpha reference value" )
		SHADER_PARAM( COLOR_DEPTH, SHADER_PARAM_TYPE_BOOL, "0", "Write depth as color")
	END_SHADER_PARAMS

    SHADER_INIT_PARAMS()
	{
		SET_FLAGS2( MATERIAL_VAR2_SUPPORTS_HW_SKINNING );
	}

    SHADER_FALLBACK
	{
		return 0;
	}

    SHADER_INIT
	{
	}

    DECLARE_CONSTANT_BUFFER( DepthWrite );

    SHADER_INIT_GLOBAL
    {
        INIT_CONSTANT_BUFFER( DepthWrite );
    }

    SHADER_DRAW
    {
        bool bAlphaClip = IS_FLAG_SET( MATERIAL_VAR_ALPHATEST );
		int nColorDepth = GetIntParam( COLOR_DEPTH, params, 0 );

        SHADOW_STATE
        {
            unsigned int flags = VERTEX_POSITION | VERTEX_FORMAT_COMPRESSED;
			int nTexCoordCount = 1;
			int userDataSize = 0;
			pShaderShadow->VertexShaderVertexFormat( flags, nTexCoordCount, NULL, userDataSize );

            pShaderShadow->EnableColorWrites( ( nColorDepth == 1 ) );
            pShaderShadow->EnableCulling( IS_FLAG_SET(MATERIAL_VAR_ALPHATEST) && !IS_FLAG_SET(MATERIAL_VAR_NOCULL) );

            SET_FLAGS2( MATERIAL_VAR2_USES_VERTEXID );

            SetInternalVertexShaderConstantBuffers();
            SetVertexShaderConstantBuffer( USER_CBUFFER_REG_0, CONSTANT_BUFFER( DepthWrite ) );

            DECLARE_STATIC_VERTEX_SHADER( depthwrite_vs50 );
            SET_STATIC_VERTEX_SHADER_COMBO( COLOR_DEPTH, nColorDepth );
            SET_STATIC_VERTEX_SHADER( depthwrite_vs50 );

            DECLARE_STATIC_PIXEL_SHADER( depthwrite_ps50 );
            SET_STATIC_PIXEL_SHADER_COMBO( COLOR_DEPTH, nColorDepth );
            SET_STATIC_PIXEL_SHADER( depthwrite_ps50 );
        }

        DYNAMIC_STATE
        {
            BindTexture( SHADER_SAMPLER0, BASETEXTURE, FRAME );

            ALIGN16 DepthWrite_CBuffer_t constants;
            memset( &constants, 0, sizeof( DepthWrite_CBuffer_t ) );
            SetHWMorphVertexShaderState( constants.cMorphDimensions, constants.cMorphSubrect, SHADER_VERTEXTEXTURE_SAMPLER0 );
            UPDATE_CONSTANT_BUFFER( DepthWrite, constants );

            DECLARE_DYNAMIC_VERTEX_SHADER( depthwrite_vs50 );
            SET_DYNAMIC_VERTEX_SHADER_COMBO( COMPRESSED_VERTS, (int)vertexCompression );
            SET_DYNAMIC_VERTEX_SHADER_COMBO( SKINNING, pShaderAPI->GetCurrentNumBones() > 0 );
            SET_DYNAMIC_VERTEX_SHADER( depthwrite_vs50 );

            DECLARE_DYNAMIC_PIXEL_SHADER( depthwrite_ps50 );
            SET_DYNAMIC_PIXEL_SHADER_COMBO( ALPHACLIP, bAlphaClip );
            SET_DYNAMIC_PIXEL_SHADER( depthwrite_ps50 );
        }

        Draw();
    }

END_SHADER