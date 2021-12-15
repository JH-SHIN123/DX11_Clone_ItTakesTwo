#include "stdafx.h"
#include "..\Public\Effect_Pinball_Move.h"
#include "DataStorage.h"
#include "Effect_Generator.h"
#include "Cody.h"
#include "PinBall.h"

CEffect_Pinball_Move::CEffect_Pinball_Move(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Pinball_Move::CEffect_Pinball_Move(const CEffect_Pinball_Move & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Pinball_Move::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 50;

	return S_OK;
}

HRESULT CEffect_Pinball_Move::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	m_pTargetObject = DATABASE->Get_Pinball();
	NULL_CHECK_RETURN(m_pTargetObject, E_FAIL);
	Safe_AddRef(m_pTargetObject);

	Check_Target_Matrix();
	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Pinball_Move::Tick(_double TimeDelta)
{
	if (true == m_IsDuplication)
		return EVENT_DEAD;

	if (false == m_IsActivate && 0.0 > m_dControlTime)
	{
		EFFECT->Set_PlayerRail_Effect((CEffect_Generator::EPlayer_Type)m_EffectDesc_Clone.iPlayerValue, false);
		return EVENT_DEAD;
	}

	if (true == m_IsActivate)
	{
		m_dControlTime += TimeDelta;
		if (1.0 < m_dControlTime) m_dControlTime = 1.0;
	}
	else
	{
		m_dControlTime -= TimeDelta;
		if (0.0 > m_dControlTime) m_dControlTime = 0.0;
	}

	Check_Activate();
	Check_Target_Matrix();
	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Pinball_Move::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Pinball_Move::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Second->Get_ShaderResourceView(6));

	m_pPointInstanceCom_STT->Render(12, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Pinball_Move::Check_Instance(_double TimeDelta)
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
		{
			Reset_Instance(TimeDelta, vMyPos, iIndex);
			continue;
		}

		Instance_Pos((_float)TimeDelta, iIndex);
	}
}

void CEffect_Pinball_Move::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Pinball_Move::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstanceBiffer_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition) + vDir * TimeDelta * 5.f * (m_pInstanceBuffer_STT[iIndex].fTime * m_pInstanceBuffer_STT[iIndex].fTime);

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_Pinball_Move::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Pinball_Move::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;
	m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;
	m_pInstanceBuffer_STT[iIndex].fTime = 1.02f;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand_Matrix(_int3(100, 20, 0), WorldMatrix, CTransform::STATE_UP));
	vRandDir += XMLoadFloat3(&__super::Get_Dir_Rand_Matrix(_int3(1000, 200, 500), WorldMatrix));
	vRandDir = XMVector3Normalize(vRandDir);
	_float3 v3RandDir;
	XMStoreFloat3(&v3RandDir, vRandDir);
	_float4 v4Dir = { v3RandDir.x, v3RandDir.y, v3RandDir.z, 0.f };
	m_pInstanceBuffer_STT[iIndex].vUp = v4Dir;

	vRandDir += WorldMatrix.r[2] * 2.f;
	vRandDir += WorldMatrix.r[1] * 1.f;
	XMStoreFloat3(&m_pInstanceBiffer_Dir[iIndex], vRandDir);
}

HRESULT CEffect_Pinball_Move::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstanceBiffer_Dir = new _float3[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;

		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
		m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };

		m_pInstance_Pos_UpdateTime[iIndex] = (m_dInstance_Pos_Update_Time * 2.f)  * (_double(iIndex) / iInstanceCount);

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand_Matrix(_int3(100, 20, 0), WorldMatrix, CTransform::STATE_UP));
		vRandDir += XMLoadFloat3(&__super::Get_Dir_Rand_Matrix(_int3(1000, 200, 500), WorldMatrix));
		vRandDir = XMVector3Normalize(vRandDir);

		vRandDir += WorldMatrix.r[2] * 1.5f;
		vRandDir += WorldMatrix.r[1] * 0.5f;
		XMStoreFloat3(&m_pInstanceBiffer_Dir[iIndex], vRandDir);
	}

	return S_OK;
}

_float3 CEffect_Pinball_Move::Get_Particle_Rand_Dir(_fvector vDefaultPos)
{
	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	_vector vPos = vDefaultPos - vRandDir;



	return _float3();
}

void CEffect_Pinball_Move::Check_Target_Matrix()
{
	m_pTransformCom->Set_WorldMatrix(Normalize_Matrix(static_cast<CPinBall*>(m_pTargetObject)->Get_WorldMatrix()));
}

void CEffect_Pinball_Move::Check_Activate()
{
	m_IsActivate = !static_cast<CPinBall*>(DATABASE->Get_Pinball())->Get_Failed();
}

CEffect_Pinball_Move * CEffect_Pinball_Move::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Pinball_Move*	pInstance = new CEffect_Pinball_Move(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Pinball_Move");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Pinball_Move::Clone_GameObject(void * pArg)
{
	CEffect_Pinball_Move* pInstance = new CEffect_Pinball_Move(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Pinball_Move");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Pinball_Move::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstanceBiffer_Dir);

	Safe_Release(m_pPointInstanceCom_STT);
	Safe_Release(m_pTargetObject);

	__super::Free();
}
