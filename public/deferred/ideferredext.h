#ifndef I_DEFERRED_EXT_H
#define I_DEFERRED_EXT_H

#ifdef CLIENT_DLL
#include "interface.h"
#include "deferred/deferred_shared_common.h"
#else
#include "tier1/interface.h"
#include "deferred_global_common.h"
#endif

struct lightData_Global_t
{
	lightData_Global_t()
	{
		bEnabled = false;
		bShadow = false;
		vecLight.Init( 0, 0, 1 );

		diff.Init();
		ambh.Init();
		ambl.Init();
	};

	Vector4D diff, ambh, ambl;
	bool bEnabled;
	bool bShadow;
	Vector4D vecLight;

	// client logic
	float flFadeTime;
	float flShadowBlend;
};

struct shadowData_ortho_t
{
	VMatrix matWorldToTexture;
#if CSM_USE_COMPOSITED_TARGET
	Vector4D vecUVTransform;
#endif

	Vector4D vecSlopeSettings;
	Vector4D vecOrigin;
	int iRes_x;
	int iRes_y;
};

struct shadowData_proj_t
{
	Vector4D vecForward;
	Vector4D vecSlopeSettings;
	Vector4D vecOrigin;
};

struct shadowData_general_t
{
	shadowData_general_t()
	{
		iDPSM_Res_x = 256;
		iDPSM_Res_y = 256;
		iPROJ_Res = 256;

#if DEFCFG_ADAPTIVE_SHADOWMAP_LOD
		iDPSM_Res_x_LOD1 = 128;
		iDPSM_Res_y_LOD1 = 128;
		iDPSM_Res_x_LOD2 = 64;
		iDPSM_Res_y_LOD2 = 64;
		iPROJ_Res_LOD1 = 128;
		iPROJ_Res_LOD2 = 64;
#endif
	};
	int iDPSM_Res_x;
	int iDPSM_Res_y;

	int iPROJ_Res;

#if DEFCFG_ADAPTIVE_SHADOWMAP_LOD
	int iDPSM_Res_x_LOD1;
	int iDPSM_Res_y_LOD1;
	int iDPSM_Res_x_LOD2;
	int iDPSM_Res_y_LOD2;

	int iPROJ_Res_LOD1;
	int iPROJ_Res_LOD2;
#endif
};

struct volumeData_t
{
	int iDataOffset;
	int iSamplerOffset;
	int iNumRows;

	bool bHasCookie;

#if DEFCFG_ADAPTIVE_VOLUMETRIC_LOD
	int iLOD;
#endif
#if DEFCFG_CONFIGURABLE_VOLUMETRIC_LOD
	int iSamples;
#endif
};

struct radiosityData_t
{
	Vector vecOrigin[2];
};

#include "tier0/memdbgon.h"

struct lightDataCommon_t
{
	lightDataCommon_t( size_t dataCount )
	{
		pFlData = new float[dataCount];
	}
	void DeleteThis()
	{
		delete[] pFlData;
		delete this;
	}

	float* pFlData;
	int numRows;
	int numShadowedCookied;
	int numShadowed;
	int numCookied;
	int numSimple;
};

#include "tier0/memdbgoff.h"

class IDeferredExtension : public IBaseInterface
{
public:
	virtual void EnableDeferredLighting() = 0;

	virtual void CommitCommonData( const Vector &origin,
								   const Vector &fwd,
								   const float &zNear, const float &zFar,
								   const VMatrix &matTFrustum
#if DEFCFG_BILATERAL_DEPTH_TEST
								,  const VMatrix &matWorldCameraDepthTex
#endif
																) = 0;

	virtual void CommitZScale( const float &zScale ) = 0;

	virtual void CommitShadowData_Ortho( const int &index, const shadowData_ortho_t &data ) = 0;
	virtual void CommitShadowData_Proj( const int &index, const shadowData_proj_t &data ) = 0;
	virtual void CommitShadowData_General( const shadowData_general_t &data ) = 0;

	virtual void CommitVolumeData( const volumeData_t &data ) = 0;

	virtual void CommitRadiosityData( const radiosityData_t &data ) = 0;

	virtual void CommitLightData_Global( const lightData_Global_t &data ) = 0;
	virtual void CommitLightData_Common( lightDataCommon_t* pData ) = 0;

	virtual void CommitTexture_General( ITexture *pTexNormals, ITexture *pTexDepth,
#if ( DEFCFG_LIGHTCTRL_PACKING == 0 )
		ITexture *pTexLightingCtrl,
#elif DEFCFG_DEFERRED_SHADING == 1
		ITexture *pTexAlbedo,
		ITexture *pTexSpecular,
#endif
		ITexture *pTexLightAccum ) = 0;
	virtual void CommitTexture_CascadedDepth( const int &index, ITexture *pTexShadowDepth ) = 0;
	virtual void CommitTexture_DualParaboloidDepth( const int &index, ITexture *pTexShadowDepth ) = 0;
	virtual void CommitTexture_ProjectedDepth( const int &index, ITexture *pTexShadowDepth ) = 0;
	virtual void CommitTexture_Cookie( const int &index, ITexture *pTexCookie ) = 0;
	virtual void CommitTexture_VolumePrePass( ITexture *pTexVolumePrePass ) = 0;
	virtual void CommitTexture_ShadowRadOutput_Ortho( ITexture *pAlbedo, ITexture *pNormal ) = 0;
	virtual void CommitTexture_Radiosity( ITexture *pTexRadBuffer0, ITexture *pTexRadBuffer1,
		ITexture *pTexRadNormal0, ITexture *pTexRadNormal1 ) = 0;
};

#define DEFERRED_EXTENSION_VERSION "DeferredExtensionVersion002"

#endif