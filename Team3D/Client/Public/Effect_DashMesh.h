#pragma once
#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_DashMesh final : public CInGameEffect_Model
{
private:
	explicit CEffect_DashMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_DashMesh(const CEffect_DashMesh& rhs);
	virtual ~CEffect_DashMesh() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	HRESULT Ready_Instance();

private:
	_float m_fAlphaTime = 0.3f;
	_double m_dActivateTime = 0.0;

public:
	static CEffect_DashMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END