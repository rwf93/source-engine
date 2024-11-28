//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef LIGHTMAPPEDGENERIC_DX11_HELPER_H
#define LIGHTMAPPEDGENERIC_DX11_HELPER_H

#include <string.h>
#include "BaseVSShader.h"


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CBaseVSShader;
class IMaterialVar;
class IShaderDynamicAPI;
class IShaderShadow;

CREATE_CONSTANT_BUFFER(LightmappedGeneric)
{
	// Vertex shader
	Vector4D cBaseTexCoordTransform[2];
	Vector4D cDetailOrBumpTexCoordTransform[2];
	Vector4D cEnvmapMaskOrBump2TexCoordTransform[2];
	Vector4D cBlendMaskTexCoordTransform[2];
	Vector4D cSeamlessMappingScale;
	Vector4D cModulationColor;

	// Pixel shader
	Vector4D g_OutlineParams[2];
	Vector4D g_EnvmapTint;
	Vector4D g_FresnelReflectionReg;
	Vector4D g_SelfIllumTint;
	Vector4D g_DetailTint_and_BlendFactor;
	Vector4D g_TintValuesAndLightmapScale;
	Vector4D g_SoftEdgeParams;
	Vector4D g_EnvmapContrast;
	Vector4D g_EnvmapSaturation;

	IntVector4D g_DetailBlendMode;

	Vector4D g_FogParams;
	Vector4D g_FogColor;

	float g_AlphaTestRef;
};

//-----------------------------------------------------------------------------
// Init params/ init/ draw methods
//-----------------------------------------------------------------------------
struct LightmappedGeneric_DX11_Vars_t
{
	LightmappedGeneric_DX11_Vars_t()
	{
		memset( this, 0xFF, sizeof( LightmappedGeneric_DX11_Vars_t ) );
	}

	int m_nBaseTexture;
	int m_nBaseTextureFrame;
	int m_nBaseTextureTransform;
	int m_nAlbedo;
	int m_nSelfIllumTint;

	int m_nAlpha2; // Hack for DoD srgb blend issues on overlays

	int m_nDetail;
	int m_nDetailFrame;
	int m_nDetailScale;
	int m_nDetailTextureCombineMode;
	int m_nDetailTextureBlendFactor;
	int m_nDetailTint;

	int m_nEnvmap;
	int m_nEnvmapFrame;
	int m_nEnvmapMask;
	int m_nEnvmapMaskFrame;
	int m_nEnvmapMaskTransform;
	int m_nEnvmapTint;
	int m_nBumpmap;
	int m_nBumpFrame;
	int m_nBumpTransform;
	int m_nEnvmapContrast;
	int m_nEnvmapSaturation;
	int m_nFresnelReflection;
	int m_nNoDiffuseBumpLighting;
	int m_nBumpmap2;
	int m_nBumpFrame2;
	int m_nBumpTransform2;
	int m_nBumpMask;
	int m_nBaseTexture2;
	int m_nBaseTexture2Frame;
	int m_nBaseTextureNoEnvmap;
	int m_nBaseTexture2NoEnvmap;
	int m_nDetailAlphaMaskBaseTexture;
	int m_nFlashlightTexture;
	int m_nFlashlightTextureFrame;
	int m_nLightWarpTexture;
	int m_nBlendModulateTexture;
	int m_nMaskedBlending;
	int m_nBlendMaskTransform;
	int m_nSelfShadowedBumpFlag;
	int m_nSeamlessMappingScale;
	int m_nAlphaTestReference;

	int m_nSoftEdges;
	int m_nEdgeSoftnessStart;
	int m_nEdgeSoftnessEnd;

	int m_nOutline;
	int m_nOutlineColor;
	int m_nOutlineAlpha;
	int m_nOutlineStart0;
	int m_nOutlineStart1;
	int m_nOutlineEnd0;
	int m_nOutlineEnd1;

	int m_nEnvmapParallax;
	int m_nEnvmapOrigin;

	int m_nPhong;
	int m_nPhongBoost;
	int m_nPhongFresnelRanges;
	int m_nPhongExponent;
};

enum PhongMaskVariant_t
{
	PHONGMASK_NONE,
	PHONGMASK_BASEALPHA,
	PHONGMASK_NORMALALPHA,
	PHONGMASK_STANDALONE,
};

struct LightmappedAdvFlashlight_DX11_Vars_t
{
	LightmappedAdvFlashlight_DX11_Vars_t()
	{
		memset( this, 0xFF, sizeof( LightmappedAdvFlashlight_DX11_Vars_t ) );
	}

	bool m_bBump;
	bool m_bLightmappedGeneric;
	bool m_bWorldVertexTransition;
	bool m_bTeeth;
	int m_nBumpmapVar;
	int m_nBumpmapFrame;
	int m_nBumpTransform;
	int m_nFlashlightTextureVar;
	int m_nFlashlightTextureFrameVar;
	int m_nBaseTexture2Var;
	int m_nBaseTexture2FrameVar;
	int m_nBumpmap2Var;
	int m_nBumpmap2Frame;
	int m_nBump2Transform;
	int m_nDetailVar;
	int m_nDetailScale;
	int m_nDetailTextureCombineMode;
	int m_nDetailTextureBlendFactor;
	int m_nDetailTint;
	int m_nTeethForwardVar;
	int m_nTeethIllumFactorVar;
	int m_nAlphaTestReference;
	bool m_bSSBump;
	float m_fSeamlessScale;								// 0.0 = not seamless
	int m_nPhong;
	int m_nPhongBoost;
	int m_nPhongFresnelRanges;
	int m_nPhongExponent;
	int m_nPhongMask;
	int m_nPhongMaskFrame;
};

void InitParamsLightmappedGeneric_DX11( CBaseVSShader *pShader, IMaterialVar **params, const char *pMaterialName, LightmappedGeneric_DX11_Vars_t &info );
void InitLightmappedGeneric_DX11( CBaseVSShader *pShader, IMaterialVar **params, LightmappedGeneric_DX11_Vars_t &info );
void DrawLightmappedGeneric_DX11( CBaseVSShader *pShader, IMaterialVar **params,
				 IShaderDynamicAPI *pShaderAPI, IShaderShadow *pShaderShadow,
				 LightmappedGeneric_DX11_Vars_t &info, CBasePerMaterialContextData **pContextDataPtr, ConstantBufferHandle_t constantBuffer );


#endif // LIGHTMAPPEDGENERIC_DX11_HELPER_H