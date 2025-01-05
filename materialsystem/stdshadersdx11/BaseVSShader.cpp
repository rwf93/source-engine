//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
// This is what all vs/ps (dx8+) shaders inherit from.
//===========================================================================//
#if !defined(_STATIC_LINKED) || defined(STDSHADER_DX11_DLL_EXPORT)

#include "basevsshader.h"
#include "mathlib/vmatrix.h"
#include "mathlib/bumpvects.h"
#include "ConVar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar mat_fullbright( "mat_fullbright","0", FCVAR_CHEAT );

// These functions are to be called from the shaders.

//-----------------------------------------------------------------------------
// Helper methods for pixel shader overbrighting
//-----------------------------------------------------------------------------
void CBaseVSShader::EnablePixelShaderOverbright( int reg, bool bEnable, bool bDivideByTwo )
{
	// can't have other overbright values with pixel shaders as it stands.
	float v[4];
	if( bEnable )
	{
		v[0] = v[1] = v[2] = v[3] = bDivideByTwo ? OVERBRIGHT / 2.0f : OVERBRIGHT;
	}
	else
	{
		v[0] = v[1] = v[2] = v[3] = bDivideByTwo ? 1.0f / 2.0f : 1.0f;
	}
	s_pShaderAPI->SetPixelShaderConstant( reg, v, 1 );
}

//-----------------------------------------------------------------------------
// Helper for dealing with modulation
//-----------------------------------------------------------------------------
void CBaseVSShader::SetModulationDynamicState( Vector4D &output )
{
	ComputeModulationColor( output.Base() );
}

void CBaseVSShader::SetModulationDynamicState_LinearColorSpace( Vector4D &output )
{
	float color[4] = { 1.0, 1.0, 1.0, 1.0 };
	ComputeModulationColor( color );
	color[0] = color[0] > 1.0f ? color[0] : GammaToLinear( color[0] );
	color[1] = color[1] > 1.0f ? color[1] : GammaToLinear( color[1] );
	color[2] = color[2] > 1.0f ? color[2] : GammaToLinear( color[2] );

	output.Init( color[0], color[1], color[2], color[3] );
}

void CBaseVSShader::SetModulationDynamicState_LinearColorSpace_LinearScale( Vector4D &output, float flScale )
{
	float color[4] = { 1.0, 1.0, 1.0, 1.0 };
	ComputeModulationColor( color );
	color[0] = ( color[0] > 1.0f ? color[0] : GammaToLinear( color[0] ) ) * flScale;
	color[1] = ( color[1] > 1.0f ? color[1] : GammaToLinear( color[1] ) ) * flScale;
	color[2] = ( color[2] > 1.0f ? color[2] : GammaToLinear( color[2] ) ) * flScale;

	output.Init( color[0], color[1], color[2], color[3] );
}


//-----------------------------------------------------------------------------
// Converts a color + alpha into a vector4
//-----------------------------------------------------------------------------
void CBaseVSShader::ColorVarsToVector( int colorVar, int alphaVar, Vector4D &color )
{
	color.Init( 1.0, 1.0, 1.0, 1.0 ); 
	if ( colorVar != -1 )
	{
		IMaterialVar* pColorVar = s_ppParams[colorVar];
		if ( pColorVar->GetType() == MATERIAL_VAR_TYPE_VECTOR )
		{
			pColorVar->GetVecValue( color.Base(), 3 );
		}
		else
		{
			color[0] = color[1] = color[2] = pColorVar->GetFloatValue();
		}
	}
	if ( alphaVar != -1 )
	{
		float flAlpha = s_ppParams[alphaVar]->GetFloatValue();
		color[3] = clamp( flAlpha, 0.0f, 1.0f );
	}
}

#ifdef _DEBUG
ConVar mat_envmaptintoverride( "mat_envmaptintoverride", "-1" );
ConVar mat_envmaptintscale( "mat_envmaptintscale", "-1" );
#endif

//-----------------------------------------------------------------------------
// Helpers for dealing with envmap tint
//-----------------------------------------------------------------------------
// set alphaVar to -1 to ignore it.
void CBaseVSShader::SetEnvMapTintPixelShaderDynamicState( int pixelReg, int tintVar, int alphaVar, bool bConvertFromGammaToLinear )
{
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	if( g_pConfig->bShowSpecular && mat_fullbright.GetInt() != 2 )
	{
		IMaterialVar* pAlphaVar = NULL;
		if( alphaVar >= 0 )
		{
			pAlphaVar = s_ppParams[alphaVar];
		}
		if( pAlphaVar )
		{
			color[3] = pAlphaVar->GetFloatValue();
		}

		IMaterialVar* pTintVar = s_ppParams[tintVar];
#ifdef _DEBUG
		pTintVar->GetVecValue( color, 3 );

		float envmapTintOverride = mat_envmaptintoverride.GetFloat();
		float envmapTintScaleOverride = mat_envmaptintscale.GetFloat();

		if( envmapTintOverride != -1.0f )
		{
			color[0] = color[1] = color[2] = envmapTintOverride;
		}
		if( envmapTintScaleOverride != -1.0f )
		{
			color[0] *= envmapTintScaleOverride;
			color[1] *= envmapTintScaleOverride;
			color[2] *= envmapTintScaleOverride;
		}

		if( bConvertFromGammaToLinear )
		{
			color[0] = color[0] > 1.0f ? color[0] : GammaToLinear( color[0] );
			color[1] = color[1] > 1.0f ? color[1] : GammaToLinear( color[1] );
			color[2] = color[2] > 1.0f ? color[2] : GammaToLinear( color[2] );
		}
#else
		if( bConvertFromGammaToLinear )
		{
			pTintVar->GetLinearVecValue( color, 3 );
		}
		else
		{
			pTintVar->GetVecValue( color, 3 );
		}
#endif
	}
	else
	{
		color[0] = color[1] = color[2] = color[3] = 0.0f;
	}
	s_pShaderAPI->SetPixelShaderConstant( pixelReg, color, 1 );
}

void CBaseVSShader::SetVertexShaderTextureTransform( Vector4D *transform, int transformVar )
{
	IMaterialVar* pTransformationVar = s_ppParams[transformVar];
	if( pTransformationVar && (pTransformationVar->GetType() == MATERIAL_VAR_TYPE_MATRIX) )
	{
		const VMatrix &mat = pTransformationVar->GetMatrixValue();
		transform[0].Init( mat[0][0], mat[0][1], mat[0][2], mat[0][3] );
        transform[1].Init( mat[1][0], mat[1][1], mat[1][2], mat[1][3] );
	}
	else
	{
		transform[0].Init( 1.0f, 0.0f, 0.0f, 0.0f );
        transform[1].Init( 0.0f, 1.0f, 0.0f, 0.0f );
	}
}

void CBaseVSShader::SetVertexShaderTextureScaledTransform( Vector4D *transform, int transformVar, int scaleVar )
{
	IMaterialVar* pTransformationVar = s_ppParams[transformVar];
	if( pTransformationVar && (pTransformationVar->GetType() == MATERIAL_VAR_TYPE_MATRIX) )
	{
		const VMatrix &mat = pTransformationVar->GetMatrixValue();
        transform[0].Init( mat[0][0], mat[0][1], mat[0][2], mat[0][3] );
        transform[1].Init( mat[1][0], mat[1][1], mat[1][2], mat[1][3] );
	}
	else
	{
		transform[0].Init( 1.0f, 0.0f, 0.0f, 0.0f );
        transform[1].Init( 0.0f, 1.0f, 0.0f, 0.0f );
	}

	Vector2D scale( 1, 1 );
	IMaterialVar* pScaleVar = s_ppParams[scaleVar];

	if( pScaleVar )
	{
		if( pScaleVar->GetType() == MATERIAL_VAR_TYPE_VECTOR )
			pScaleVar->GetVecValue( scale.Base(), 2 );
		else if ( pScaleVar->IsDefined() )
			scale[0] = scale[1] = pScaleVar->GetFloatValue();
	}

	transform[0][0] *= scale[0];
    transform[0][1] *= scale[1];
    transform[1][0] *= scale[0];
    transform[1][1] *= scale[1];
    transform[0][3] *= scale[0];
    transform[1][3] *= scale[1];
}

void CBaseVSShader::SetConstantGammaToLinear( vec_t *transform, int transformVar )
{
	IMaterialVar* pTransformationVar = s_ppParams[transformVar];

	if( pTransformationVar && (pTransformationVar->GetType() == MATERIAL_VAR_TYPE_VECTOR) )
	{
		if (pTransformationVar->GetType() == MATERIAL_VAR_TYPE_VECTOR)
			pTransformationVar->GetVecValue( transform, 4 );
		else
			transform[0] = transform[1] = transform[2] = transform[3] = pTransformationVar->GetFloatValue();

		transform[0] = transform[0] > 1.0f ? transform[0] : GammaToLinear( transform[0] );
		transform[1] = transform[1] > 1.0f ? transform[1] : GammaToLinear( transform[1] );
		transform[2] = transform[2] > 1.0f ? transform[2] : GammaToLinear( transform[2] );
		transform[3] = transform[3] > 1.0f ? transform[3] : GammaToLinear( transform[3] );
	}
	else
	{
		V_memset( transform, 0, sizeof( vec_t ) * 4 );
	}
}

void CBaseVSShader::SetEnvmapTint( Vector4D &transform, int transformVar )
{
	IMaterialVar* pTransformationVar = s_ppParams[transformVar];
	if( pTransformationVar && (pTransformationVar->GetType() == MATERIAL_VAR_TYPE_VECTOR) )
	{
		V_memcpy( &transform, pTransformationVar->GetVecValueFast(), sizeof( Vector4D ) );
	}
	else
	{
		V_memset( &transform, 0, sizeof( Vector4D ) );
	}
}

void CBaseVSShader::SetEnvmapTintGammaToLinear( Vector4D &transform, int transformVar )
{
	IMaterialVar* pTransformationVar = s_ppParams[transformVar];
	if( pTransformationVar && (pTransformationVar->GetType() == MATERIAL_VAR_TYPE_VECTOR) )
	{
		if (pTransformationVar->GetType() == MATERIAL_VAR_TYPE_VECTOR)
			V_memcpy( &transform, pTransformationVar->GetVecValueFast(), sizeof( Vector4D ) );
		else
			transform[0] = transform[1] = transform[2] = transform[3] = pTransformationVar->GetFloatValue();

		transform[0] = transform[0] > 1.0f ? transform[0] : GammaToLinear( transform[0] );
		transform[1] = transform[1] > 1.0f ? transform[1] : GammaToLinear( transform[1] );
		transform[2] = transform[2] > 1.0f ? transform[2] : GammaToLinear( transform[2] );
		transform[3] = transform[3] > 1.0f ? transform[3] : GammaToLinear( transform[3] );
	}
	else
	{
		V_memset( &transform, 0, sizeof( Vector4D ) );
	}
}


void CBaseVSShader::SetAmbientCubeDynamicStateVertexShader( )
{
	s_pShaderAPI->SetVertexShaderStateAmbientLightCube();
}

float CBaseVSShader::GetAmbientLightCubeLuminance( )
{
	return s_pShaderAPI->GetAmbientLightCubeLuminance();
}

//-----------------------------------------------------------------------------
// Sets up hw morphing state for the vertex shader
//-----------------------------------------------------------------------------
void CBaseVSShader::SetHWMorphVertexShaderState( Vector4D &dimensions, Vector4D &subrect, VertexTextureSampler_t morphSampler )
{
	if ( !s_pShaderAPI->IsHWMorphingEnabled() )
		return;

	int nMorphWidth, nMorphHeight;
	s_pShaderAPI->GetStandardTextureDimensions( &nMorphWidth, &nMorphHeight, TEXTURE_MORPH_ACCUMULATOR );

	int nDim = s_pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_MORPH_ACCUMULATOR_4TUPLE_COUNT );
	float pMorphAccumSize[4] = { (float)nMorphWidth, (float)nMorphHeight, nDim, 0.0f };
	dimensions.Init( pMorphAccumSize[0], pMorphAccumSize[1], pMorphAccumSize[2], pMorphAccumSize[3] );

	int nXOffset = s_pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_MORPH_ACCUMULATOR_X_OFFSET );
	int nYOffset = s_pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_MORPH_ACCUMULATOR_Y_OFFSET );
	int nWidth = s_pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_MORPH_ACCUMULATOR_SUBRECT_WIDTH );
	int nHeight = s_pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_MORPH_ACCUMULATOR_SUBRECT_HEIGHT );
	float pMorphAccumSubrect[4] = { (float)nXOffset, (float)nYOffset, (float)nWidth, (float)nHeight };
	subrect.Init( pMorphAccumSubrect[0], pMorphAccumSubrect[1], pMorphAccumSubrect[2], pMorphAccumSubrect[3] );

	s_pShaderAPI->BindStandardVertexTexture( morphSampler, TEXTURE_MORPH_ACCUMULATOR );
}

//-----------------------------------------------------------------------------
// GR - translucency query
//-----------------------------------------------------------------------------
BlendType_t CBaseVSShader::EvaluateBlendRequirements( int textureVar, bool isBaseTexture,
													  int detailTextureVar )
{
	// Either we've got a constant modulation
	bool isTranslucent = IsAlphaModulating();

	// Or we've got a vertex alpha
	isTranslucent = isTranslucent || (CurrentMaterialVarFlags() & MATERIAL_VAR_VERTEXALPHA);

	// Or we've got a texture alpha (for blending or alpha test)
	isTranslucent = isTranslucent || ( TextureIsTranslucent( textureVar, isBaseTexture ) &&
		                               !(CurrentMaterialVarFlags() & MATERIAL_VAR_ALPHATEST ) );

	if ( ( detailTextureVar != -1 ) && ( ! isTranslucent ) )
	{
		isTranslucent = TextureIsTranslucent( detailTextureVar, isBaseTexture );
	}

	if ( CurrentMaterialVarFlags() & MATERIAL_VAR_ADDITIVE )
	{	
		return isTranslucent ? BT_BLENDADD : BT_ADD;	// Additive
	}
	else
	{
		return isTranslucent ? BT_BLEND : BT_NONE;		// Normal blending
	}
}

#endif // !_STATIC_LINKED || STDSHADER_DX8_DLL_EXPORT

void CBaseVSShader::DrawEqualDepthToDestAlpha( void )
{
#ifdef STDSHADER_DX11_DLL_EXPORT
	{
		bool bMakeActualDrawCall = false;
		if( s_pShaderShadow )
		{
			s_pShaderShadow->EnableColorWrites( false );
			s_pShaderShadow->EnableAlphaWrites( true );
			s_pShaderShadow->EnableDepthWrites( false );
			s_pShaderShadow->EnableAlphaTest( false );
			s_pShaderShadow->EnableBlending( false );

			s_pShaderShadow->DepthFunc( SHADER_DEPTHFUNC_EQUAL );

			SetVertexShaderConstantBuffer( 0, SHADER_CONSTANTBUFFER_PERMODEL );
			SetVertexShaderConstantBuffer( 1, SHADER_CONSTANTBUFFER_PERFRAME );
			SetVertexShaderConstantBuffer( 2, SHADER_CONSTANTBUFFER_PERSCENE );

			s_pShaderShadow->SetVertexShader( "depthtodestalpha_vs50", 0 );
			s_pShaderShadow->SetPixelShader( "depthtodestalpha_ps50", 0 );
		}
		if( s_pShaderAPI )
		{
			s_pShaderAPI->SetVertexShaderIndex( 0 );
			s_pShaderAPI->SetPixelShaderIndex( 0 );

			bMakeActualDrawCall = s_pShaderAPI->ShouldWriteDepthToDestAlpha();
		}
		Draw( bMakeActualDrawCall );
	}
#else
	Assert( 0 ); //probably just needs a shader update to the latest
#endif
}