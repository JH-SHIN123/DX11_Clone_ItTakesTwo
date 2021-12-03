#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CAlienScreen final : public CDynamic_Env
{
private:
	explicit CAlienScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CAlienScreen(const CAlienScreen& rhs);
	virtual ~CAlienScreen() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CStaticActor*	m_pStaticActorCom = nullptr;
	CTextures*		m_pTextureCom = nullptr;

	_float			m_fTimeDelta = 0.f;
private:
	HRESULT Ready_Component(void* pArg);

public:
	static  CAlienScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*  Clone_GameObject(void* pArg) override;
	virtual void		  Free() override;
};
END