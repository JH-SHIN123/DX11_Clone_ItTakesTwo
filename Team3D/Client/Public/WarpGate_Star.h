#pragma once

#include "InGameEffect_Model.h"

BEGIN(Client)
class CWarpGate_Star final : public CGameObject
{
private:
	explicit CWarpGate_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWarpGate_Star(const CWarpGate_Star& rhs);
	virtual ~CWarpGate_Star() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void MiniGame_Warp(_bool IsMiniGame) { m_IsMiniGame = IsMiniGame; }
	void Set_Scale(_vector vScale) { m_pTransformCom-> Set_Scale(vScale); }
	void Set_WorldMatrix(_fmatrix WorldMatrix, _fvector vOffSetPos);
	void Set_Activate(_bool IsActivate);

private:
	void Activate_Check(_double TimeDelta);

private:
	_bool m_IsActivate = false;
	_bool m_IsMiniGame = false;

private:
	CTransform*		m_pTransformCom		= nullptr;
	CRenderer*		m_pRendererCom		= nullptr;
	CStaticActor*	m_pStaticActorCom	= nullptr;
	CModel*			m_pModelCom			= nullptr;



public:
	static CWarpGate_Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END