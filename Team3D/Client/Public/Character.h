#pragma once

#include "Client_Defines.h"

BEGIN(Client)
class CSpaceRail;
class CSpaceRail_Node;
class CCharacter abstract : public CGameObject
{
protected:
	explicit CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public: /* Setter */
	void Set_WorldMatrix(_fmatrix WorldMatrix);
	void Set_Position(_fvector WorldPos);

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

protected: /* For.Component */
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	class CPlayerActor*		m_pActorCom = nullptr;

protected:
	_float m_fClockWise = 1.f; // 1이면 시계방향, -1이면 반시계방향.
	//_uint  m_iViewPortNum = 0;

public: /*  WonTaek - Path */
	void Set_SpaceRailNode(CSpaceRail_Node* pRail);
	
protected:
	void Find_TargetSpaceRail(); // LateTick에서 호출되어야함.
	void MoveToTargetRail(_uint eState, _double dTimeDelta);
	_bool TakeRail(_double dTimeDelta, _matrix& WorldMatrix);
	// 타겟 레일을 향해 날라가는함수, 그리고 레일타기시작

protected:
	_bool m_bSearchToRail = false;
	_bool m_bMoveToRail = false;
	_bool m_bOnRail = false;

protected: 
	vector<CSpaceRail_Node*>	m_vecTargetRailNodes;
	CSpaceRail*					m_pTargetRail = nullptr;
	CSpaceRail_Node*			m_pTargetRailNode = nullptr;

public:
	virtual void Free() override;
};

END