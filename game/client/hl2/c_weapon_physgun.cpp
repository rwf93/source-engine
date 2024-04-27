//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#include "cbase.h"
#include "hud.h"
#include "in_buttons.h"
#include "beamdraw.h"
#include "c_weapon__stubs.h"
#include "clienteffectprecachesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CLIENTEFFECT_REGISTER_BEGIN(PrecacheEffectPhysGun)
CLIENTEFFECT_MATERIAL("sprites/physbeam")
CLIENTEFFECT_REGISTER_END()

class C_BeamQuadratic : public CDefaultClientRenderable
{
public:
	C_BeamQuadratic();
	void Update(C_BaseEntity *pOwner);

	// IClientRenderable
	virtual const Vector &GetRenderOrigin(void) { return m_worldPosition; }
	virtual const QAngle &GetRenderAngles(void) { return vec3_angle; }
	virtual bool ShouldDraw(void) { return true; }
	virtual bool IsTransparent(void) { return true; }
	virtual bool ShouldReceiveProjectedTextures(int flags) { return false; }
	virtual int DrawModel(int flags);

	virtual const matrix3x4_t &RenderableToWorldTransform()
	{
		matrix3x4_t b;
		return b;
	};

	// Returns the bounds relative to the origin (render bounds)
	virtual void GetRenderBounds(Vector &mins, Vector &maxs)
	{
		// bogus.  But it should draw if you can see the end point
		mins.Init(-32, -32, -32);
		maxs.Init(32, 32, 32);
	}

	C_BaseEntity *m_pOwner;
	Vector m_targetPosition;
	Vector m_worldPosition;
	int m_active;
	int m_viewModelIndex;
};

class C_WeaponPhysGun : public C_BaseCombatWeapon
{
	DECLARE_CLASS(C_WeaponPhysGun, C_BaseCombatWeapon);

public:
	C_WeaponPhysGun() {}

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	int KeyInput(int down, ButtonCode_t keynum, const char *pszCurrentBinding)
	{
		if (gHUD.m_iKeyBits & IN_ATTACK)
		{
			switch (keynum)
			{
			case MOUSE_WHEEL_UP:
				gHUD.m_iKeyBits |= IN_WEAPON1;
				return 0;

			case MOUSE_WHEEL_DOWN:
				gHUD.m_iKeyBits |= IN_WEAPON2;
				return 0;
			}
		}

		// Allow engine to process
		return BaseClass::KeyInput(down, keynum, pszCurrentBinding);
	}

	void OnDataChanged(DataUpdateType_t updateType)
	{
		BaseClass::OnDataChanged(updateType);
		m_beam.Update(this);
	}

private:
	C_WeaponPhysGun(const C_WeaponPhysGun &);

	C_BeamQuadratic m_beam;

	DECLARE_ACTTABLE();
};

acttable_t	C_WeaponPhysGun::m_acttable[] = 
{
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_PHYSGUN,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_PHYSGUN,					false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_PHYSGUN,			false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_PHYSGUN,			false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PHYSGUN,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_PHYSGUN,		false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_PHYSGUN,					false },
};

IMPLEMENT_ACTTABLE(C_WeaponPhysGun);

STUB_WEAPON_CLASS_IMPLEMENT(weapon_physgun, C_WeaponPhysGun);

IMPLEMENT_CLIENTCLASS_DT(C_WeaponPhysGun, DT_WeaponPhysGun, CWeaponPhysGun)
RecvPropVector(RECVINFO_NAME(m_beam.m_targetPosition, m_targetPosition)),
	RecvPropVector(RECVINFO_NAME(m_beam.m_worldPosition, m_worldPosition)),
	RecvPropInt(RECVINFO_NAME(m_beam.m_active, m_active)),
	RecvPropInt(RECVINFO_NAME(m_beam.m_viewModelIndex, m_viewModelIndex)),
END_RECV_TABLE()

C_BeamQuadratic::C_BeamQuadratic()
{
	m_pOwner = NULL;
}

void C_BeamQuadratic::Update(C_BaseEntity *pOwner)
{
	m_pOwner = pOwner;
	if (m_active)
	{
		if (m_hRenderHandle == INVALID_CLIENT_RENDER_HANDLE)
		{
			ClientLeafSystem()->AddRenderable(this, RENDER_GROUP_TRANSLUCENT_ENTITY);
		}
		else
		{
			ClientLeafSystem()->RenderableChanged(m_hRenderHandle);
		}
	}
	else if (!m_active && m_hRenderHandle != INVALID_CLIENT_RENDER_HANDLE)
	{
		ClientLeafSystem()->RemoveRenderable(m_hRenderHandle);
	}
}

int C_BeamQuadratic::DrawModel(int)
{
	Vector points[3];
	QAngle tmpAngle;

	if (!m_active)
		return 0;

	C_BaseEntity *pEnt = cl_entitylist->GetEnt(m_viewModelIndex);
	if (!pEnt)
		return 0;
	pEnt->GetAttachment(1, points[0], tmpAngle);

	points[1] = 0.5 * (m_targetPosition + points[0]);

	// a little noise 11t & 13t should be somewhat non-periodic looking
	// points[1].z += 4*sin( gpGlobals->curtime*11 ) + 5*cos( gpGlobals->curtime*13 );
	points[2] = m_worldPosition;

	IMaterial *pMat = materials->FindMaterial("sprites/physbeam", TEXTURE_GROUP_CLIENT_EFFECTS);
	Vector color(1, 1, 1);

	float scrollOffset = gpGlobals->curtime - (int)gpGlobals->curtime;
	materials->GetRenderContext()->Bind(pMat);
	DrawBeamQuadratic(points[0], points[1], points[2], 13, color, scrollOffset);
	return 1;
}
