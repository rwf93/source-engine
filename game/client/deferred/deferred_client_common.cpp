
#include "cbase.h"
#include "deferred/deferred_shared_common.h"

#include "tier0/memdbgon.h"

ConVar r_deferred_rt_shadowspot_res( "deferred_rt_shadowspot_res", "1024", FCVAR_HIDDEN | FCVAR_RELOAD_TEXTURES | FCVAR_RELOAD_MATERIALS );
#if DEFCFG_ADAPTIVE_SHADOWMAP_LOD
ConVar r_deferred_rt_shadowspot_lod1_res( "deferred_rt_shadowspot_lod1_res", "512", FCVAR_HIDDEN | FCVAR_RELOAD_TEXTURES | FCVAR_RELOAD_MATERIALS );
ConVar r_deferred_rt_shadowspot_lod2_res( "deferred_rt_shadowspot_lod2_res", "256", FCVAR_HIDDEN | FCVAR_RELOAD_TEXTURES | FCVAR_RELOAD_MATERIALS );
#endif
ConVar r_deferred_rt_shadowpoint_res( "deferred_rt_shadowpoint_res", "1024", FCVAR_HIDDEN | FCVAR_RELOAD_TEXTURES | FCVAR_RELOAD_MATERIALS );
#if DEFCFG_ADAPTIVE_SHADOWMAP_LOD
ConVar r_deferred_rt_shadowpoint_lod1_res( "deferred_rt_shadowpoint_lod1_res", "512", FCVAR_HIDDEN | FCVAR_RELOAD_TEXTURES | FCVAR_RELOAD_MATERIALS );
ConVar r_deferred_rt_shadowpoint_lod2_res( "deferred_rt_shadowpoint_lod2_res", "256", FCVAR_HIDDEN | FCVAR_RELOAD_TEXTURES | FCVAR_RELOAD_MATERIALS );
#endif

ConVar r_deferred_lightmanager_debug( "r_deferred_lightmanager_debug", "0" );

ConVar r_deferred_override_global_light_enable( "r_deferred_override_global_light_enable", "0" );
ConVar r_deferred_override_global_light_shadow_enable( "r_deferred_override_global_light_shadow_enable", "1" );
ConVar r_deferred_override_global_light_diffuse( "r_deferred_override_global_light_diffuse", "1 1 1" );
ConVar r_deferred_override_global_light_ambient_high( "r_deferred_override_global_light_ambient_high", "0 0.04 0.08" );
ConVar r_deferred_override_global_light_ambient_low( "r_deferred_override_global_light_ambient_low", "0 0.07 0.09" );

ConVar r_deferred_radiosity_enable( "r_deferred_radiosity_enable", "0" );
ConVar r_deferred_radiosity_propagate_count( "r_deferred_radiosity_propagate_count", "1" ); // 1
ConVar r_deferred_radiosity_propagate_count_far( "r_deferred_radiosity_propagate_count_far", "0" );
ConVar r_deferred_radiosity_blur_count( "r_deferred_radiosity_blur_count", "2" ); // 2
ConVar r_deferred_radiosity_blur_count_far( "r_deferred_radiosity_blur_count_far", "1" ); // 1
ConVar r_deferred_radiosity_debug( "r_deferred_radiosity_debug", "0" );

void OnCookieTableChanged( void *object, INetworkStringTable *stringTable, int stringNumber, const char *newString, void const *newData )
{
	if ( !newString || Q_strlen( newString ) < 1 )
		return;

	GetLightingManager()->OnCookieStringReceived( newString, stringNumber );
}

void CalcBoundaries( Vector *list, const int &num, Vector &min, Vector &max )
{
	Assert( num > 0 );

#if DEFCFG_USE_SSE && 0
	fltx4 vTestPoint = _mm_set_ps( list[0].x, list[0].y, list[0].z, 0 );
	fltx4 vMin = vTestPoint;
	fltx4 vMax = vTestPoint;

	for( int i = 1; i < num; i++ )
	{
		vTestPoint = _mm_set_ps( list[i].x, list[i].y, list[i].z, 0 );
		vMin = _mm_min_ps( vMin, vTestPoint );
		vMax = _mm_max_ps( vMax, vTestPoint );
	}

	min = Vector( SubFloat( vMin, 0 ), SubFloat( vMin, 1 ), SubFloat( vMin, 2 ) );
	max = Vector( SubFloat( vMax, 0 ), SubFloat( vMax, 1 ), SubFloat( vMax, 2 ) );
#else
	min = *list;
	max = *list;

	for ( int i = 1; i < num; i++ )
	{
		for ( int x = 0; x < 3; x++ )
		{
			min[ x ] = Min( min[ x ], list[ i ][ x ] );
			max[ x ] = Max( max[ x ], list[ i ][ x ] );
		}
	}
#endif
}