#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CRotationCylinder final : public CDynamic_Env
{
private:
	explicit CRotationCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRotationCylinder(const CRotationCylinder& rhs);
	virtual ~CRotationCylinder() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CStaticActor*	m_pStaticActorCom = nullptr;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CRotationCylinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END