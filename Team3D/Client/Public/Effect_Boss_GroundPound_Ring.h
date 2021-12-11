#pragma once
#include "InGameEffect.h"
BEGIN(Client)
class CEffect_Boss_GroundPound_Ring final : public CInGameEffect
{
private:
	explicit CEffect_Boss_GroundPound_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_GroundPound_Ring(const CEffect_Boss_GroundPound_Ring& rhs);
	virtual ~CEffect_Boss_GroundPound_Ring() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	HRESULT Ready_Instance();
	void Check_RotateUV(_double TimeDelta);

private:
	CVIBuffer_Rect*		m_pBufferRectCom = nullptr;


	_float4 m_vColor = { 0.980392218f, 0.401960814f, 0.347058856f, 1.000000000f };
	_double m_dRotateTime = 0.0;
	_float  m_fTime = 1.5f;
	_float2 m_vSize = { 5.f, 5.f };
	_float4 m_vColorRamp_UV = { 0.f, 0.f, 1.f, 1.f };

private:
	CTextures* m_pTexturesCom_Distorion = nullptr;

public:
	static CEffect_Boss_GroundPound_Ring* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END	
