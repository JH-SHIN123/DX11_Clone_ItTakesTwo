#include "stdafx.h"
#include "..\Public\Effect_PipeLocker_Particle.h"

CEffect_PipeLocker_Particle::CEffect_PipeLocker_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_PipeLocker_Particle::CEffect_PipeLocker_Particle(const CEffect_PipeLocker_Particle & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_PipeLocker_Particle::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 40;

	return S_OK;
}

HRESULT CEffect_PipeLocker_Particle::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_Buffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);

	Check_Target_Matrix();
	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_PipeLocker_Particle::Tick(_double TimeDelta)
{
	if (0.0 > m_dControlTime)
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

	Check_Target_Matrix();
	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_PipeLocker_Particle::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_PipeLocker_Particle::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_int i = 1;
	m_pPointInstanceCom->Set_DefaultVariables();
	m_pPointInstanceCom->Set_Variable("g_IsBillBoard", &i, sizeof(_int));
	m_pPointInstanceCom->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));

	m_pPointInstanceCom->Render(24, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_PipeLocker_Particle::Check_Instance(_double TimeDelta)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex] && true == m_IsActivate)
		{
			Reset_Instance(TimeDelta, vMyPos, iIndex);
			continue;
		}

		Instance_Pos((_float)TimeDelta, iIndex);
		Instance_Size((_float)TimeDelta, iIndex);
	}
}

void CEffect_PipeLocker_Particle::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize.x -= TimeDelta * 0.9f;
	if (0.f > m_pInstanceBuffer[iIndex].vSize.x) m_pInstanceBuffer[iIndex].vSize.x = 0.f;

	m_pInstanceBuffer[iIndex].vSize.y -= TimeDelta * 0.9f;
	if (0.f > m_pInstanceBuffer[iIndex].vSize.y) m_pInstanceBuffer[iIndex].vSize.y = 0.f;
}

void CEffect_PipeLocker_Particle::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition) + vDir * TimeDelta * 3.f;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

void CEffect_PipeLocker_Particle::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_PipeLocker_Particle::Reset_Instance(_double TimeDelta, _vector vPos, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize = Get_RandSize();

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;

	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	
	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos + vRandDir * 0.8f);

	vRandDir += XMLoadFloat3(&m_vDir);
	vRandDir = XMVector3Normalize(vRandDir);
	XMStoreFloat3(&m_pInstance_Dir[iIndex], vRandDir);
}

HRESULT CEffect_PipeLocker_Particle::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[iInstanceCount];
	m_pInstance_Dir = new _float3[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer[iIndex].vPosition = vMyPos;

		m_pInstanceBuffer[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer[iIndex].vSize = { 0.f, 0.f };

		m_pInstance_Pos_UpdateTime[iIndex] = 0.05 * _double(iIndex);

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));

		XMStoreFloat3(&m_pInstance_Dir[iIndex], vRandDir);
	}

	return S_OK;
}

_float3 CEffect_PipeLocker_Particle::Get_Particle_Rand_Dir(_fvector vDefaultPos)
{
	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	_vector vPos = vDefaultPos - vRandDir;



	return _float3();
}

void CEffect_PipeLocker_Particle::Check_Target_Matrix()
{
}

_float2 CEffect_PipeLocker_Particle::Get_RandSize()
{
	_int iRand = rand() % 3;
	_float2 vSize = m_vDefaultSize;

	switch (iRand)
	{
	case 0:
		vSize.x -= 0.025f;
		vSize.y -= 0.025f;
		break;
	case 1:
		vSize.x += 0.025f;
		vSize.y += 0.025f;
		break;
	default:
		break;
	}

	return vSize;
}

CEffect_PipeLocker_Particle * CEffect_PipeLocker_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_PipeLocker_Particle*	pInstance = new CEffect_PipeLocker_Particle(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_PipeLocker_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_PipeLocker_Particle::Clone_GameObject(void * pArg)
{
	CEffect_PipeLocker_Particle* pInstance = new CEffect_PipeLocker_Particle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_PipeLocker_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_PipeLocker_Particle::Free()
{
	__super::Free();
}
