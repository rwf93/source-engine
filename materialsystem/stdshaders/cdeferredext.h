#ifndef C_DEFERRED_EXT
#define C_DEFERRED_EXT

#include "deferred_includes.h"

class CDeferredExtension : public IDeferredExtension
{
public:
	CDeferredExtension();
	~CDeferredExtension();

	virtual void EnableDeferredLighting();
	bool IsDeferredLightingEnabled() const;
	bool IsRadiosityEnabled() const;

	void CommitCommonData( const Vector &origin,
						   const Vector &fwd,
						   const float &zNear, const float &zFar,
						   const VMatrix &matTFrustum
#if DEFCFG_BILATERAL_DEPTH_TEST
						   , const VMatrix &matWorldCameraDepthTex
#endif
														);

	virtual void CommitZScale( const float &zScale );

	virtual void CommitShadowData_Ortho( const int &index, const shadowData_ortho_t &data );
	virtual void CommitShadowData_Proj( const int &index, const shadowData_proj_t &data );
	virtual void CommitShadowData_General( const shadowData_general_t &data );

	virtual void CommitVolumeData( const volumeData_t &data );
	virtual void CommitRadiosityData( const radiosityData_t &data );

	virtual void CommitLightData_Global( const lightData_Global_t &data );
	virtual void CommitLightData_Common( lightDataCommon_t* pData );

	virtual void CommitTexture_General( ITexture *pTexNormals, ITexture *pTexDepth,
#if ( DEFCFG_LIGHTCTRL_PACKING == 0 )
		ITexture *pTexLightingCtrl,
#elif DEFCFG_DEFERRED_SHADING == 1
		ITexture *pTexAlbedo,
		ITexture *pTexSpecular,
#endif
		ITexture *pTexLightAccum );
	virtual void CommitTexture_CascadedDepth( const int &index, ITexture *pTexShadowDepth );
	virtual void CommitTexture_DualParaboloidDepth( const int &index, ITexture *pTexShadowDepth );
	virtual void CommitTexture_ProjectedDepth( const int &index, ITexture *pTexShadowDepth );
	virtual void CommitTexture_Cookie( const int &index, ITexture *pTexCookie );
	virtual void CommitTexture_VolumePrePass( ITexture *pTexVolumePrePass );
	virtual void CommitTexture_ShadowRadOutput_Ortho( ITexture *pAlbedo, ITexture *pNormal );
	virtual void CommitTexture_Radiosity( ITexture *pTexRadBuffer0, ITexture *pTexRadBuffer1,
		ITexture *pTexRadNormal0, ITexture *pTexRadNormal1 );

	inline float *GetOriginBase();
	inline float *GetForwardBase();
	inline const float &GetZDistNear();
	inline const float &GetZDistFar();
	inline float GetZScale();
	inline float *GetFrustumDeltaBase();
#if DEFCFG_BILATERAL_DEPTH_TEST
	inline float *GetWorldToCameraDepthTexBase();
#endif

	inline int GetNumActiveLights_ShadowedCookied();
	inline int GetNumActiveLights_Shadowed();
	inline int GetNumActiveLights_Cookied();
	inline int GetNumActiveLights_Simple();
	inline float *GetActiveLightData();
	inline int GetActiveLights_NumRows();

	inline const shadowData_ortho_t &GetShadowData_Ortho( const int &index );
	inline const shadowData_proj_t &GetShadowData_Proj( const int &index );
	inline const shadowData_general_t &GetShadowData_General();

	inline const volumeData_t &GetVolumeData();
	inline const radiosityData_t &GetRadiosityData();

	inline const lightData_Global_t &GetLightData_Global();

	inline ITexture *GetTexture_Normals();
	inline ITexture *GetTexture_Depth();
	inline ITexture *GetTexture_LightAccum();
#if ( DEFCFG_LIGHTCTRL_PACKING == 0 )
	inline ITexture *GetTexture_LightCtrl();
#elif DEFCFG_DEFERRED_SHADING == 1
	inline ITexture *GetTexture_Albedo();
	inline ITexture *GetTexture_Specular();
#endif
	inline ITexture *GetTexture_ShadowDepth_Ortho( const int &index );
	inline ITexture *GetTexture_ShadowDepth_DP( const int &index );
	inline ITexture *GetTexture_ShadowDepth_Proj( const int &index );
	inline ITexture *GetTexture_Cookie( const int &index );
	inline ITexture *GetTexture_VolumePrePass();
	inline ITexture *GetTexture_ShadowRad_Ortho_Albedo();
	inline ITexture *GetTexture_ShadowRad_Ortho_Normal();
	inline ITexture *GetTexture_RadBuffer( const int &index );
	inline ITexture *GetTexture_RadNormal( const int &index );

private:
	bool m_bDefLightingEnabled;

	Vector4D m_vecOrigin;
	Vector4D m_vecForward;
	float m_flZDists[3];
	VMatrix m_matTFrustumD;
#if DEFCFG_BILATERAL_DEPTH_TEST
	VMatrix m_matWorldCameraDepthTex;
#endif

	shadowData_ortho_t m_dataOrtho[ SHADOW_NUM_CASCADES ];
	shadowData_proj_t m_dataProj[ MAX_SHADOW_PROJ ];
	shadowData_general_t m_dataGeneral;

	volumeData_t m_dataVolume;
	radiosityData_t m_dataRadiosity;

	lightData_Global_t m_globalLight;
	lightDataCommon_t *m_pflCommonLightData;
	int m_iCommon_NumRows;
	int m_iNumCommon_ShadowedCookied;
	int m_iNumCommon_Shadowed;
	int m_iNumCommon_Cookied;
	int m_iNumCommon_Simple;

	ITexture *m_pTexNormals;
	ITexture *m_pTexDepth;
	ITexture *m_pTexLightAccum;
#if ( DEFCFG_LIGHTCTRL_PACKING == 0 )
	ITexture *m_pTexLightCtrl;
#elif DEFCFG_DEFERRED_SHADING == 1
	ITexture *m_pTexAlbedo;
	ITexture *m_pTexSpecular;
#endif
	ITexture *m_pTexShadowDepth_Ortho[ MAX_SHADOW_ORTHO ];
	ITexture *m_pTexShadowDepth_DP[ MAX_SHADOW_DP ];
	ITexture *m_pTexShadowDepth_Proj[ MAX_SHADOW_PROJ ];
	ITexture *m_pTexCookie[ NUM_COOKIE_SLOTS ];
	ITexture *m_pTexVolumePrePass;
	ITexture *m_pTexShadowRad_Ortho[ 2 ];
	ITexture *m_pTexRadBuffer[ 2 ];
	ITexture *m_pTexRadNormal[ 2 ];
};

float *CDeferredExtension::GetOriginBase()
{
	return m_vecOrigin.Base();
}
float *CDeferredExtension::GetForwardBase()
{
	return m_vecForward.Base();
}
const float &CDeferredExtension::GetZDistNear()
{
	return m_flZDists[0];
}
const float &CDeferredExtension::GetZDistFar()
{
	return m_flZDists[1];
}
float CDeferredExtension::GetZScale()
{
	return m_flZDists[2];
}
float *CDeferredExtension::GetFrustumDeltaBase()
{
	return m_matTFrustumD.Base();
}

#if DEFCFG_BILATERAL_DEPTH_TEST
float *CDeferredExtension::GetWorldToCameraDepthTexBase()
{
	return m_matWorldCameraDepthTex.Base();
}
#endif
const shadowData_ortho_t &CDeferredExtension::GetShadowData_Ortho( const int &index )
{
	Assert( index >= 0 && index < SHADOW_NUM_CASCADES );
	return m_dataOrtho[ index ];
}
const shadowData_proj_t &CDeferredExtension::GetShadowData_Proj( const int &index )
{
	Assert( index >= 0 && index < MAX_SHADOW_PROJ );
	return m_dataProj[ index ];
}
const shadowData_general_t &CDeferredExtension::GetShadowData_General()
{
	return m_dataGeneral;
}

const lightData_Global_t &CDeferredExtension::GetLightData_Global()
{
	return m_globalLight;
}

const volumeData_t &CDeferredExtension::GetVolumeData()
{
	return m_dataVolume;
}

const radiosityData_t &CDeferredExtension::GetRadiosityData()
{
	return m_dataRadiosity;
}

int CDeferredExtension::GetNumActiveLights_ShadowedCookied()
{
	return m_iNumCommon_ShadowedCookied;
}
int CDeferredExtension::GetNumActiveLights_Shadowed()
{
	return m_iNumCommon_Shadowed;
}
int CDeferredExtension::GetNumActiveLights_Cookied()
{
	return m_iNumCommon_Cookied;
}
int CDeferredExtension::GetNumActiveLights_Simple()
{
	return m_iNumCommon_Simple;
}
float *CDeferredExtension::GetActiveLightData()
{
	if ( m_pflCommonLightData )
		return m_pflCommonLightData->pFlData;
	return NULL;
}
int CDeferredExtension::GetActiveLights_NumRows()
{
	return m_iCommon_NumRows;
}
ITexture *CDeferredExtension::GetTexture_Normals()
{
	return m_pTexNormals;
}
ITexture *CDeferredExtension::GetTexture_Depth()
{
	return m_pTexDepth;
}
ITexture *CDeferredExtension::GetTexture_LightAccum()
{
	return m_pTexLightAccum;
}
#if ( DEFCFG_LIGHTCTRL_PACKING == 0 )
ITexture *CDeferredExtension::GetTexture_LightCtrl()
{
	return m_pTexLightCtrl;
}
#elif DEFCFG_DEFERRED_SHADING == 1
ITexture *CDeferredExtension::GetTexture_Albedo()
{
	return m_pTexAlbedo;
}
ITexture *CDeferredExtension::GetTexture_Specular()
{
	return m_pTexSpecular;
}
#endif
ITexture *CDeferredExtension::GetTexture_ShadowDepth_Ortho( const int &index )
{
	Assert( index >= 0 && index < MAX_SHADOW_ORTHO );
	return m_pTexShadowDepth_Ortho[index];
}
ITexture *CDeferredExtension::GetTexture_ShadowDepth_DP( const int &index )
{
	Assert( index >= 0 && index < MAX_SHADOW_DP );
	return m_pTexShadowDepth_DP[index];
}
ITexture *CDeferredExtension::GetTexture_ShadowDepth_Proj( const int &index )
{
	Assert( index >= 0 && index < MAX_SHADOW_PROJ );
	return m_pTexShadowDepth_Proj[index];
}
ITexture *CDeferredExtension::GetTexture_Cookie( const int &index )
{
	Assert( index >= 0 && index < NUM_COOKIE_SLOTS );
	return m_pTexCookie[index];
}
ITexture *CDeferredExtension::GetTexture_VolumePrePass()
{
	return m_pTexVolumePrePass;
}
ITexture *CDeferredExtension::GetTexture_ShadowRad_Ortho_Albedo()
{
	return m_pTexShadowRad_Ortho[0];
}
ITexture *CDeferredExtension::GetTexture_ShadowRad_Ortho_Normal()
{
	return m_pTexShadowRad_Ortho[1];
}
ITexture *CDeferredExtension::GetTexture_RadBuffer( const int &index )
{
	Assert( index >= 0 && index < 2 );
	return m_pTexRadBuffer[index];
}
ITexture *CDeferredExtension::GetTexture_RadNormal( const int &index )
{
	Assert( index >= 0 && index < 2 );
	return m_pTexRadNormal[index];
}

extern CDeferredExtension __g_defExt;
FORCEINLINE CDeferredExtension *GetDeferredExt()
{
	return &__g_defExt;
}

#endif