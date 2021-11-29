#pragma once

#include "Client_Defines.h"

BEGIN(Client)
class CSpaceRail;
class CCharacter abstract : public CGameObject
{
protected:
	explicit CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	void Set_SpaceRail(CSpaceRail* pRail);

public: /* Getter */
	virtual _fvector Get_Position();

protected:
	_float Compute_Distance(CTransform* pPlayerTransform, CTransform* pDstTransform);
	_float Compute_Degree(CTransform* pPlayerTransform, CTransform* pDstTransform);
	_float Compute_Degree_On_Land(CTransform* pPlayerTransform, CTransform* pDstTransform);

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;

public:
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
public:
	virtual HRESULT Render_ShadowDepth() override;

protected:
	void Find_SpaceRailTarget(); // LateTick에서 호출되어야함.

protected: /* For.Component */
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	class CPlayerActor*		m_pActorCom = nullptr;

protected:
	_float m_fClockWise = 1.f; // 1이면 시계방향, -1이면 반시계방향.
	//_uint  m_iViewPortNum = 0;

protected:
	vector<CSpaceRail*>		m_vecRideOnRails;
	CSpaceRail*				m_pTargetSpaceRail = nullptr;

public:
	virtual void Free() override;
};

END