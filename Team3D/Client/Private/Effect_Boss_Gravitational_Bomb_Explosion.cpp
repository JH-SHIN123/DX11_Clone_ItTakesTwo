#include "stdafx.h"
#include "..\Public\Effect_Boss_Gravitational_Bomb_Explosion.h"

CEffect_Boss_Gravitational_Bomb_Explosion::CEffect_Boss_Gravitational_Bomb_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Gravitational_Bomb_Explosion::CEffect_Boss_Gravitational_Bomb_Explosion(const CEffect_Boss_Gravitational_Bomb_Explosion & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Gravitational_Bomb_Explosion::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 300;

	return S_OK;
}

HRESULT CEffect_Boss_Gravitational_Bomb_Explosion::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Spark_Mask"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Boss_Gravitational_Bomb_Explosion::Tick(_double TimeDelta)
{
	if (m_dInstance_Pos_Update_Time + 1.5 <= m_dControlTime)
		m_IsActivate = false;

	if (false == m_IsActivate && 0.0 >= m_dControlTime)
		return EVENT_DEAD;

	if (true == m_IsActivate)
		m_dControlTime += TimeDelta;
	else
		m_dControlTime -= TimeDelta;


	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Boss_Gravitational_Bomb_Explosion::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_Gravitational_Bomb_Explosion::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(3));

	m_pPointInstanceCom_STT->Render(6, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_Gravitational_Bomb_Explosion::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.65f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex] && true == m_IsActivate)
			continue;

		Instance_Pos((_float)TimeDelta, iIndex);
	}
}

void CEffect_Boss_Gravitational_Bomb_Explosion::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Gravitational_Bomb_Explosion::Instance_Pos(_float TimeDelta, _int iIndex)
{
	m_pInstance_Parabola_Time[iIndex] = (_double)TimeDelta;

	_vector vDir = XMLoadFloat3(&m_pInstanceBiffer_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition) + vDir * TimeDelta * 20.f * (m_pInstanceBuffer_STT[iIndex].fTime * m_pInstanceBuffer_STT[iIndex].fTime);

	//vPos.m128_f32[1] -= TimeDelta * (1.f - m_pInstanceBuffer_STT[iIndex].fTime) * 1.5f;

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_Boss_Gravitational_Bomb_Explosion::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Gravitational_Bomb_Explosion::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;

	m_pInstanceBuffer_STT[iIndex].fTime = 1.02f;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;
	m_pInstance_Parabola_Time[iIndex] = 0.0;

	//_vector vLookDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * -1.5f;
	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	//vRandDir = XMVector3Normalize(vRandDir + vLookDir);

	_float3 v3RandDir;
	XMStoreFloat3(&v3RandDir, vRandDir);
	_float4 v4Dir = { v3RandDir.x, v3RandDir.y, v3RandDir.z, 0.f };
	m_pInstanceBuffer_STT[iIndex].vUp = v4Dir;
	m_pInstanceBiffer_Dir[iIndex] = v3RandDir;
	if (0.f >= m_pInstanceBiffer_Dir[iIndex].y)
	{
		m_pInstanceBiffer_Dir[iIndex].y *= -1.2f;
		m_pInstanceBuffer_STT[iIndex].vUp.y *= -1.f;
	}
}

HRESULT CEffect_Boss_Gravitational_Bomb_Explosion::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstanceBiffer_Dir = new _float3[iInstanceCount];
	m_pInstance_Parabola_Time = new _double[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;

		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

		m_pInstance_Pos_UpdateTime[iIndex] = 0.75  * (_double(iIndex) / iInstanceCount);
		m_pInstance_Parabola_Time[iIndex] = 0.0;

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
		_float3 v3RandDir;
		XMStoreFloat3(&v3RandDir, vRandDir);
		_float4 v4Dir = { v3RandDir.x, v3RandDir.y, v3RandDir.z, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp = v4Dir;
		m_pInstanceBiffer_Dir[iIndex] = v3RandDir;
		if (0.f >= m_pInstanceBiffer_Dir[iIndex].y)
		{
			m_pInstanceBiffer_Dir[iIndex].y *= -1.2f;
			m_pInstanceBuffer_STT[iIndex].vUp.y *= -1.f;
		}
	}

	return S_OK;
}

_float3 CEffect_Boss_Gravitational_Bomb_Explosion::Get_Particle_Rand_Dir(_fvector vDefaultPos)
{
	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	_vector vPos = vDefaultPos - vRandDir;



	return _float3();
}

CEffect_Boss_Gravitational_Bomb_Explosion * CEffect_Boss_Gravitational_Bomb_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Gravitational_Bomb_Explosion*	pInstance = new CEffect_Boss_Gravitational_Bomb_Explosion(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Gravitational_Bomb_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Gravitational_Bomb_Explosion::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Gravitational_Bomb_Explosion* pInstance = new CEffect_Boss_Gravitational_Bomb_Explosion(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Gravitational_Bomb_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Gravitational_Bomb_Explosion::Free()
{
	Safe_Delete_Array(m_pInstance_Parabola_Time);
	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstanceBiffer_Dir);

	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
