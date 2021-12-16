#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Env_Particle_Field : public CInGameEffect
{
public:
	typedef struct tagArg_Desc
	{
		_int iInstanceCount;
		_float3 vRadiusXYZ;
		_float4 vPosition;

		_float		fSpeedPerSec = 0.125f;
		_float		fReSizing_Power = 0.033f;
		_float		fReSize = 0.02f;
		_float		fResetPosTime = 3.5f;
		XMINT2		vTextureUV = { 4, 2 };
		_float2		vDefaultSize = { 0.1f, 0.1f };
		_int3		vRandPower = { 100, 100, 100 };

		_float		fInitialize_UpdatePos_Term = 1.f;
		_int		iGrouping_Count = 1;
		_bool		IsGrouping = false;
// 		_bool		IsGrouping_Dir = false;
// 		_bool		IsGrouping_Pos = false;

		tagArg_Desc() {}
		tagArg_Desc(_int iInstanceCount, _float3 vRadiusXYZ, _float4 vPosition)
			: iInstanceCount(iInstanceCount), vRadiusXYZ(vRadiusXYZ), vPosition(vPosition) {}
	}ARG_DESC;

protected:
	explicit CEffect_Env_Particle_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Env_Particle_Field(const CEffect_Env_Particle_Field& rhs);
	virtual ~CEffect_Env_Particle_Field() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	void	Set_InstanceCount(_uint iInstanceCount);
	void	Set_Particle_Radius(_float3 vRadiusXYZ);
	void	Set_ParentMatrix(_fmatrix ParentMatrix);

public:
	void	Set_ControlTime(_double dControlTime);

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

protected:
	void Check_State(_double TimeDelta);
	void State_Start(_double TimeDelta);
	void State_Disappear(_double TimeDelta);

protected:
	_float4 Get_Rand_Pos();
	_float2 Get_Rand_Size(_float2 vDefaultSize, _float vReSize);
	HRESULT Reset_Instance(_int iIndex);
	HRESULT Reset_Instance_All();
	HRESULT Initialize_Instance();
	void	Initialize_Instance_Goruping(_int* iIndex, _int iInstance_Count);

protected:
	_vector Set_RandPos_Default();

protected: // 전체적인 인스턴싱을 제어함
	enum STATE_VALUE { STATE_START, STATE_DISAPPEAR, STATE_END };
	STATE_VALUE		m_eStateValue_Cur = STATE_END;
	STATE_VALUE		m_eStateValue_Next = STATE_START;
	_double			m_dControl_Time = 1.0;		// 인스턴싱의 알파값을 통괄적으로 제어
	_float3			m_vParticleRadius = { 0.f, 0.f, 0.f };
	
	ARG_DESC m_Particle_Desc;
	//_float			m_fSpeedPerSec		= 0.125f;
	//_float			m_fReSizing_Power	= 0.033f;
	//_float			m_fReSize			= 0.02f;
	//_float			m_fResetPosTime		= 3.5f;
	//XMINT2			m_vTextureUV		= { 4, 2 };
	//_float2			m_vDefaultSize		= { 0.1f, 0.1f };
	//_int3			m_vRandPower		= { 100, 100, 100 };


protected:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	VTXMATRIX_CUSTOM_STT* m_pInstanceBuffer_STT = nullptr;
	_float4* m_pInstanceBuffer_LocalPos = nullptr;

public:
	static CEffect_Env_Particle_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END

