#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CControlRoom_Monitor final : public CDynamic_Env
{
private:
	explicit CControlRoom_Monitor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CControlRoom_Monitor(const CControlRoom_Monitor& rhs);
	virtual ~CControlRoom_Monitor() = default;

public:
	void Change_Screen() { ++m_iFrame; if(2 < m_iFrame) { m_iFrame = 2; } }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CTextures*	m_pTextureCom = nullptr;
	_uint		m_iFrame = 0;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CControlRoom_Monitor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END