#pragma once

#ifndef __EFFECT_PLAYER_LAIL_H__

#include "InGameEffect.h"
BEGIN(Engine)
class CVIBuffer_PointInstance_Custom_STT;
END

BEGIN(Client)
class CEffect_Player_Lail final : public CInGameEffect
{
private:
	explicit CEffect_Player_Lail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Player_Lail(const CEffect_Player_Lail& rhs);
	virtual ~CEffect_Player_Lail() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	HRESULT Set_Model(CModel* pModel);
	void	Set_WorldMatrix(_fmatrix WorldMatrix);

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

public:
	HRESULT Ready_Component(void* pArg);
	HRESULT Ready_InstanceBuffer_Spark();

private:
	void Check_Update_Loop(_double TimeDelta);
	void Check_Spark(_double TimeDelta);

private:
	void Update_Spark(_double TimeDelta, _int iStartIndex, _int iEndIndex, _fmatrix ParentMatrix);
	void Reset_Spark(_int iIndex, _fmatrix ParentMatrix);
	_float3 Get_Dir_Spark(_int3 vRandPower);
	_float4 Get_UpVector_Rand(_int3 vRandPower);

private:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	CModel*		m_pTargetModel = nullptr;
	_float4x4	m_BoneMatrix_Left;	// LeftToeBase
	_float4x4	m_BoneMatrix_Right; // RightToeBase
	_bool		m_IsActivate = false;

private: // 버퍼의 Time은 알파값이 아닌 생명주기로 쓸것임, 신발 참고
	//m_pTexturesCom  	 = Diff
	//m_pTexturesCom_Sec = Mask
	VTXMATRIX_CUSTOM_STT* m_pInstanceBuffer_Spark = nullptr;
	_float*			m_pSpeedPerSec_Spark		= nullptr;
	_float3*		m_pMoveDir_Spark			= nullptr;
	const _float	m_fSpeedPerSec_Spark = 1.5f;
	const _float	m_fSpark_RespawnTime		= 3.f;
	const _int		m_iInstanceCount_Spark		= 400;
	const _float2	m_vDefaultSize_Spark		= { 1.f, 0.125f };
	const _float4	m_vDefaultTexUV_Spark		= { 1.f, 0.125f, 1.125f, 0.25f };
	_bool			m_IsStopUpdate_Spark		= false;
	_float			m_fStopUpdate_Spark_Timer	= 0.f;

public:
	static CEffect_Player_Lail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};

END
#define __EFFECT_PLAYER_LAIL_H__
#endif // !__EFFECT_PLAYER_LAIL_H__


// 스모크와 스파크 파티클들
/*
둘 다 양발에서 생성
스파크 파티클이 한 세트처럼 여러개가 묶여서 스모크와 같이 나오는 느낌
같이 생성한다면 20개씩 20묶음
스모크		각각 20개, 스프라이트 loop
스파크		각각 20개씩 20묶음 = 400개

*/