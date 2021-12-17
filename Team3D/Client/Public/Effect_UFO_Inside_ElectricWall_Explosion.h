#pragma once

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_UFO_Inside_ElectricWall_Explosion final : public CInGameEffect
{
private:
	explicit CEffect_UFO_Inside_ElectricWall_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_UFO_Inside_ElectricWall_Explosion(const CEffect_UFO_Inside_ElectricWall_Explosion& rhs);
	virtual ~CEffect_UFO_Inside_ElectricWall_Explosion() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_IsActivate(_bool IsActivate) { m_IsActivate = IsActivate; }
	void Set_WorldMatrix(_fmatrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }


private:
	void Check_Instance(_double TimeDelta);

private:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0)		override;

private:
	void Reset_Instance(_double TimeDelta, _fmatrix vParentMatrix, _int iIndex = 0);

private:
	HRESULT Ready_InstanceBuffer();

private:
	_double m_dAlphaTime = 0.5; //
	_double m_dActivateTime = 0.0;
	_bool m_IsActivate = true;

private:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_STT = nullptr;
	_float*								m_pInstanceBuffer_Parabola_PosY = nullptr;
	_float*								m_pInstanceBuffer_Parabola_Time = nullptr;
	_float*								m_pInstanceBuffer_Parabola_Power = nullptr;
	_float3*							m_pInstanceBuffer_Dir = nullptr;

	const _float  m_fAlphaTime_Power = 1.f;
	const _float  m_fInstance_SpeedPerSec = 2.5f;
	const _double m_dInstance_Pos_Update_Time = 2.0;
	const _float2 m_vDefaultSize = { 0.0625f, 0.0775f };

private:
	_float3 m_vOffSet_Pos = { 0.f, -0.75f, -0.2f };

public:
	static CEffect_UFO_Inside_ElectricWall_Explosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END