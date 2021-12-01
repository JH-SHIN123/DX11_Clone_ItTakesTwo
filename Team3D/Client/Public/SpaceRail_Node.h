#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CTriggerActor;
END

BEGIN(Client)
class CCody;
class CMay;
class CSpaceRail_Node : public CGameObject
{
public:
	typedef struct tagSpaceRailNode
	{
		_tchar		szRailTag[MAX_PATH] = L"";
		_float4x4	WorldMatrix = MH_XMFloat4x4Identity();
		_uint		iFrameIndex = 0;
	}SPACERAILNODE_DESC;

private:
	explicit CSpaceRail_Node(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpaceRail_Node(const CSpaceRail_Node& rhs) = delete;
	virtual ~CSpaceRail_Node() = default;

public:
	_fmatrix		Get_WorldMatrix() const;
	_fvector		Get_Position() const;
	_uint			Get_FrameIndex() const { return m_tDesc.iFrameIndex; };
	const _tchar*	Get_RailTag() const { return m_tDesc.szRailTag; }

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;

public:
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject);

private:
	/* For.Component */
	CTransform*		m_pTransformCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

private:
	_bool			m_bCollide = false;
	CCody*			m_pCody = nullptr;
	CMay*			m_pMay = nullptr;

private:
	SPACERAILNODE_DESC	m_tDesc;

public:
	static CSpaceRail_Node* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override { return nullptr; };
	virtual void Free() override;
};
END
