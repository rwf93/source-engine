//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#if 1
#include "BaseVSShader.h"


#include "writez_vs50.inc"
#include "white_ps50.inc"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DEFINE_FALLBACK_SHADER( WriteZ, WriteZ_DX11 )

BEGIN_VS_SHADER_FLAGS( WriteZ_DX11, "Help for WriteZ", SHADER_NOT_EDITABLE )

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
	}

	SHADER_DRAW
	{
		SHADOW_STATE
		{
			pShaderShadow->EnableColorWrites( false );
			pShaderShadow->EnableAlphaWrites( false );

			SetInternalVertexShaderConstantBuffersNoSkinning();

			DECLARE_STATIC_VERTEX_SHADER( writez_vs50 );
			SET_STATIC_VERTEX_SHADER( writez_vs50 );

			DECLARE_STATIC_PIXEL_SHADER( white_ps50 );
			SET_STATIC_PIXEL_SHADER( white_ps50 );

			// Set stream format (note that this shader supports compression)
			unsigned int flags = VERTEX_POSITION | VERTEX_FORMAT_COMPRESSED;
			int nTexCoordCount = 1;
			int userDataSize = 0;
			pShaderShadow->VertexShaderVertexFormat( flags, nTexCoordCount, NULL, userDataSize );
		}
		DYNAMIC_STATE
		{
			DECLARE_DYNAMIC_VERTEX_SHADER( writez_vs50 );
			SET_DYNAMIC_VERTEX_SHADER( writez_vs50 );

			DECLARE_DYNAMIC_PIXEL_SHADER( white_ps50 );
			SET_DYNAMIC_PIXEL_SHADER( white_ps50 );
		}
		Draw();
	}
END_SHADER
#endif
