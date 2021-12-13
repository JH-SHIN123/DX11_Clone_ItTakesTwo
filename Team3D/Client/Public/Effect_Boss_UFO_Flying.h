#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Boss_UFO_Flying final : public CInGameEffect 
{
private:
	explicit CEffect_Boss_UFO_Flying(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_UFO_Flying(const CEffect_Boss_UFO_Flying& rhs);
	virtual ~CEffect_Boss_UFO_Flying() = default; public:

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
	void Check_ParentMatrix();
	void Check_RotateUV(_double TimeDelta);

private:
	CVIBuffer_Rect*		m_pBufferRectCom = nullptr;
	_double				m_dActivate_Time = 0.0;
	_bool				m_IsActivate	 = true;

	_float4 m_vColor = { 0.141176534f, 0.172549081f, 1.000000000f, 1.000000000f };
	_double m_dRotateTime = 0.0;
	_float  m_fTime = 0.0f;
	_float2 m_vSize = { 5.f, 5.f };
	_float4 m_vColorRamp_UV = { 0.f, 0.f, 1.f, 1.f };
	_float3 m_vOffSet_Pos = { 0.f, -1.f, -0.2f };


private:
	class CUFO* m_pUFO = nullptr;

public:
	static CEffect_Boss_UFO_Flying* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END