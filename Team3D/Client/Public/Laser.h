#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CLaser abstract : public CGameObject
{
protected:
	explicit CLaser(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaser(const CLaser& rhs);
	virtual	~CLaser() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eRender) override;
	virtual HRESULT Render_ShadowDepth() override;

public:
	void Set_Dead();
	void Player_Hit_Effect();

protected:
	CTransform*							m_pTransformCom = nullptr;
	CTextures*							m_pTextureCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CVIBuffer_PointInstance_Custom_ST*	m_pVIBufferCom = nullptr;
	CTriggerActor*						m_pTriggerCom = nullptr;
	VTXMATRIX_CUSTOM_ST*				m_pLaserMatrices = nullptr; /* Base(0), StartCircle(1), EndCircle(2) */
	_float4								m_vStartPoint = _float4(0.f, 0.f, 0.f, 0.f);
	_float4								m_vEndPoint = _float4(0.f, 0.f, 0.f, 0.f);
	_float4								m_vLaserDir = _float4(0.f, 0.f, 0.f, 0.f);
	_float								m_fLaserSizeX = 0.f;
	_float								m_fLaserSizeY = 0.f;
	_float								m_fLaserMaxY = 0.f;
	_float								m_fShootSpeed = 0.f;
	_float								m_fSineGrpah = 0.f;
	_float								m_fOutsideAlpha = 0.f;
	PxRaycastBuffer						m_RaycastBuffer;
	_bool								m_isCollided = false;
	/* Hit Player */
	_bool								m_isHitCody = false;
	_bool								m_isHitMay = false;
	_double								m_dDamagingDelay_Cody = 0.0;
	_double								m_dDamagingDelay_May = 0.0;
	/* Create Effect */
	_double								m_dCreateEffectCycle = 0.0;
	_double								m_dCreateEffectDelay = 0.0;
protected:
	void	Set_LaserMatices();
	void	Adjust_OutsideAlpha(_double dTimeDelta);

public:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free() override;
};

END