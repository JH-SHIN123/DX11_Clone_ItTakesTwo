#include "stdafx.h"
#include "..\Public\Effect_StarBuddy_Move_Particle.h"

CEffect_StarBuddy_Move_Particle::CEffect_StarBuddy_Move_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_StarBuddy_Move_Particle::CEffect_StarBuddy_Move_Particle(const CEffect_StarBuddy_Move_Particle & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_StarBuddy_Move_Particle::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 60;

	return S_OK;
}

HRESULT CEffect_StarBuddy_Move_Particle::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_StarBuddy_Move_Particle::Tick(_double TimeDelta)
{
	if (true == m_IsDuplication || true == m_isDead)
		return EVENT_DEAD;

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
	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_StarBuddy_Move_Particle::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_StarBuddy_Move_Particle::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Second->Get_ShaderResourceView(9));

	m_pPointInstanceCom_STT->Render(12, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_StarBuddy_Move_Particle::Check_Instance(_double TimeDelta)
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

		//Instance_Pos((_float)TimeDelta, iIndex);
		Instance_Size((_float)TimeDelta, iIndex);
	}
}

void CEffect_StarBuddy_Move_Particle::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vSize.x -= TimeDelta * 0.2f;
	if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = 0.f;
	m_pInstanceBuffer_STT[iIndex].vSize.y = m_pInstanceBuffer_STT[iIndex].vSize.x;
}

void CEffect_StarBuddy_Move_Particle::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstanceBiffer_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition) + vDir * TimeDelta * (m_pInstanceBuffer_STT[iIndex].fTime * m_pInstanceBuffer_STT[iIndex].fTime);

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_StarBuddy_Move_Particle::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_StarBuddy_Move_Particle::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;
	m_pInstanceBuffer_STT[iIndex].vSize = Get_RandSize();
	m_pInstanceBuffer_STT[iIndex].fTime = 1.02f;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;

	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));

	XMStoreFloat3(&m_pInstanceBiffer_Dir[iIndex], vRandDir);
}

HRESULT CEffect_StarBuddy_Move_Particle::Ready_InstanceBuffer()
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

		m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time * (_double(iIndex) / (_double)iInstanceCount);

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));

		XMStoreFloat3(&m_pInstanceBiffer_Dir[iIndex], vRandDir);
	}

	return S_OK;
}

_float3 CEffect_StarBuddy_Move_Particle::Get_Particle_Rand_Dir(_fvector vDefaultPos)
{
	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	_vector vPos = vDefaultPos - vRandDir;



	return _float3();
}

void CEffect_StarBuddy_Move_Particle::Check_Target_Matrix()
{
}

void CEffect_StarBuddy_Move_Particle::Check_Activate()
{
}

_float2 CEffect_StarBuddy_Move_Particle::Get_RandSize()
{
	_int iRand = rand() % 3;
	_float2 vSize = m_vDefaultSize;

	switch (iRand)
	{
	case 0:
		vSize.x -= 0.05f;
		vSize.y -= 0.05f;
		break;
	case 1:
		vSize.x += 0.05f;
		vSize.y += 0.05f;
		break;
	default:
		break;
	}

	return vSize;
}

CEffect_StarBuddy_Move_Particle * CEffect_StarBuddy_Move_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_StarBuddy_Move_Particle*	pInstance = new CEffect_StarBuddy_Move_Particle(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_StarBuddy_Move_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_StarBuddy_Move_Particle::Clone_GameObject(void * pArg)
{
	CEffect_StarBuddy_Move_Particle* pInstance = new CEffect_StarBuddy_Move_Particle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_StarBuddy_Move_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_StarBuddy_Move_Particle::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstanceBiffer_Dir);

	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
