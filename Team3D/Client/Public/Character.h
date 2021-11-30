#pragma once

#include "Client_Defines.h"

BEGIN(Engine)
class CPath;
END

BEGIN(Client)
class CSpaceRail_Node;
class CCharacter abstract : public CGameObject
{
protected:
	explicit CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public: /* Setter */
	//void Set_SpaceRailNode(CSpaceRail_Node* pRail);
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

//protected:
//	void Find_SpaceRailTarget(); // LateTick에서 호출되어야함.

protected: /* For.Component */
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	class CPlayerActor*		m_pActorCom = nullptr;

protected:
	_float m_fClockWise = 1.f; // 1이면 시계방향, -1이면 반시계방향.
	//_uint  m_iViewPortNum = 0;

protected: /* WonTaek - Path */
	CPath* m_pPathCom = nullptr;


//protected: /* Space Rail */
//	class CSpaceRail* m_pSpaceRailCom = nullptr;
//	vector<CSpaceRail_Node*>	m_vecRideOnRailNodes;
//	CSpaceRail_Node*			m_pTargetSpaceRailNode = nullptr;

public:
	virtual void Free() override;
};

END