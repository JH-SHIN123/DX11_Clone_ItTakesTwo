#pragma once
#include "GameObject.h"

BEGIN(Client)

class CRockParticle final : public CGameObject
{
private:
	explicit CRockParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRockParticle(const CRockParticle& rhs);
	virtual ~CRockParticle() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;

private:
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	_float			m_fScale = 1.f;
	_double			m_dCoolTime = 0.0;
private:
	HRESULT Ready_Component(void* pArg);

public:
	static CRockParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END

