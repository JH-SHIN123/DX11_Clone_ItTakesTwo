#pragma once
#include "GameObject.h"

BEGIN(Client)
class CMeshParticle final : public CGameObject
{
public:
	typedef struct tagArgument
	{
		_float3		vPosition;
		_uint		iColorType;
	}ARG_DESC;

private:
	explicit CMeshParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMeshParticle(const CMeshParticle& rhs);
	virtual ~CMeshParticle() = default;

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
	CDynamicActor*	m_pDynamicActorCom = nullptr;

	_double			m_dCoolTime = 0.0;
	_uint			m_iColorType = 0;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CMeshParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END