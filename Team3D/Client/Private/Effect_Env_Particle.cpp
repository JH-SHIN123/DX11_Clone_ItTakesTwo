#include "stdafx.h"
#include "..\Public\Effect_Env_Particle.h"

CEffect_Env_Particle::CEffect_Env_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Env_Particle::CEffect_Env_Particle(const CEffect_Env_Particle & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Env_Particle::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Env_Particle::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_eParticle_Type, pArg, sizeof(EParticle_Type));

	//m_pRendererCom
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_PointBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName, TEXT("Com_Textures"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName_Second, TEXT("Com_Textures_Smoke2"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	return S_OK;
}

_int CEffect_Env_Particle::Tick(_double TimeDelta)
{
	Check_State(TimeDelta);

	return _int();
}

_int CEffect_Env_Particle::Late_Tick(_double TimeDelta)
{
	if (0.0 >= m_dControl_Time)
		return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Env_Particle::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = (_float)m_dControl_Time;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fAlpha, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Render(2, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);
	return S_OK;
}

void CEffect_Env_Particle::Set_InstanceCount(_uint iInstanceCount)
{
	m_EffectDesc_Prototype.iInstanceCount = iInstanceCount;
	Initialize_Instance();
}

void CEffect_Env_Particle::Set_Particle_Radius(_float3 vRadiusXYZ)
{
	m_vParticleRadius.x = vRadiusXYZ.x;
	m_vParticleRadius.y = vRadiusXYZ.y;
	m_vParticleRadius.z = vRadiusXYZ.z;
}

// void CEffect_Env_Particle::Set_IsActivateParticles(_bool IsActivate)
// {
// 	if (true == IsActivate)
// 		m_eStateValue_Next = CEffect_Env_Particle::STATE_START;
// 
// 	else
// 		m_eStateValue_Next = CEffect_Env_Particle::STATE_DISAPPEAR;
// }

void CEffect_Env_Particle::Set_ParentMatrix(_fmatrix ParentMatrix)
{
	m_pTransformCom->Set_WorldMatrix(ParentMatrix);
}

void CEffect_Env_Particle::Set_ControlTime(_double dControlTime)
{
	m_eStateValue_Next = CEffect_Env_Particle::STATE_START;

	if (0.0 >= dControlTime)
	{
		dControlTime = 0.0;
		m_eStateValue_Next = CEffect_Env_Particle::STATE_DISAPPEAR;
	}

	m_dControl_Time = dControlTime;
}

void CEffect_Env_Particle::Check_State(_double TimeDelta)
{
	if(nullptr == m_pInstanceBuffer_STT)
		return;

	switch (m_eStateValue_Next)
	{
	case CEffect_Env_Particle::STATE_START:
		State_Start(TimeDelta);
		break;
	case  CEffect_Env_Particle::STATE_DISAPPEAR:
		State_Disappear(TimeDelta);
		break;
	default:
		m_eStateValue_Next = STATE_DISAPPEAR;
		break;
	}
}

void CEffect_Env_Particle::State_Start(_double TimeDelta)
{
	_float fTimeDelta = (_float)TimeDelta;
	_fmatrix ParentMatrix = m_pTransformCom->Get_WorldMatrix(); // NULL

//  	m_dControl_Time += TimeDelta * 0.1;
 	if (1.0 <= m_dControl_Time)
 		m_dControl_Time = 1.0;

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (0.f >= m_pInstance_Pos_UpdateTime[iIndex])
			Reset_Instance(iIndex);

		m_pInstanceBuffer_STT[iIndex].fTime += fTimeDelta  * (_float)m_dControl_Time;
		if (1.f <= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 1.f;

		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
		_vector vLocalPos = XMLoadFloat4(&m_pInstanceBuffer_LocalPos[iIndex]);
		vLocalPos += vDir * fTimeDelta * 0.125f;
		XMStoreFloat4(&m_pInstanceBuffer_LocalPos[iIndex], vLocalPos);
		vLocalPos = XMVector3Transform(vLocalPos, ParentMatrix);
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vLocalPos);

		m_pInstanceBuffer_STT[iIndex].vSize.x -= fTimeDelta * 0.033f;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= fTimeDelta * 0.033f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x)
			m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };
	}
}

void CEffect_Env_Particle::State_Disappear(_double TimeDelta)
{
	_float fTimeDelta = (_float)TimeDelta;
	_fmatrix ParentMatrix = m_pTransformCom->Get_WorldMatrix();

//  	m_dControl_Time -= TimeDelta * 0.1;
	if (0.0 >= m_dControl_Time)
	{
		m_dControl_Time = 0.0;
		return;
	}

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (0.f >= m_pInstance_Pos_UpdateTime[iIndex])
			Reset_Instance(iIndex);

		m_pInstanceBuffer_STT[iIndex].fTime += fTimeDelta  * (_float)m_dControl_Time;
		if (1.f <= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 1.f;

		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
		_vector vLocalPos = XMLoadFloat4(&m_pInstanceBuffer_LocalPos[iIndex]);
		vLocalPos += vDir * fTimeDelta * 0.125f;
		XMStoreFloat4(&m_pInstanceBuffer_LocalPos[iIndex], vLocalPos);
		vLocalPos = XMVector3Transform(vLocalPos, ParentMatrix);
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vLocalPos);

		m_pInstanceBuffer_STT[iIndex].vSize.x -= fTimeDelta * 0.033f;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= fTimeDelta * 0.033f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x)
			m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };
	}
}

_float4 CEffect_Env_Particle::Get_Rand_Pos()
{
	_vector vRandPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	switch (m_eParticle_Type)
	{
	case CEffect_Env_Particle::Default:
		vRandPos = Set_RandPos_Default();
		break;
	case CEffect_Env_Particle::Umbrella:
		vRandPos = Set_RandPos_Umbrella();
		break;
	case CEffect_Env_Particle::Portal:
		break;
	default:
		break;
	}

	_float4 vPos;
	XMStoreFloat4(&vPos, vRandPos);
	return vPos;
}

_float2 CEffect_Env_Particle::Get_Rand_Size()
{
	_float2 vRandSize = m_vDefaultSize;

	_int iRand = rand() % 3;
	if (0 == iRand)
	{
		vRandSize.x -= 0.02f;
		vRandSize.y -= 0.02f;
	}
	else if (1 == iRand)
	{
		vRandSize.x += 0.02f;
		vRandSize.y += 0.02f;
	}

	if (Umbrella == m_eParticle_Type)
	{
		vRandSize.x += 0.02f;
		vRandSize.y += 0.02f;
	}

	return vRandSize;
}

HRESULT CEffect_Env_Particle::Reset_Instance(_int iIndex)
{
	m_pInstanceBuffer_LocalPos[iIndex]			= Get_Rand_Pos();
	m_pInstanceBuffer_STT[iIndex].vPosition		= { 0.f, 0.f, 0.f, 1.f };
	m_pInstanceBuffer_STT[iIndex].vSize			= Get_Rand_Size();
	m_pInstanceBuffer_STT[iIndex].vTextureUV	= Get_TexUV_Rand(4, 2);
	m_pInstanceBuffer_STT[iIndex].fTime			= 0.f;
	m_pInstance_Dir[iIndex]						= Get_Dir_Rand(_int3(100, 100, 100));
	m_pInstance_Pos_UpdateTime[iIndex]			= m_fResetPosTime;

	return S_OK;
}

HRESULT CEffect_Env_Particle::Reset_Instance_All()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_LocalPos[iIndex]	= Get_Rand_Pos();
		m_pInstance_Dir[iIndex]				= Get_Dir_Rand(_int3(100, 100, 100));
	}

	return S_OK;
}

HRESULT CEffect_Env_Particle::Initialize_Instance()
{
	if (nullptr == m_pPointInstanceCom_STT)
		return S_OK;

	_int iInstanceCount			= m_EffectDesc_Prototype.iInstanceCount;
	m_pInstanceBuffer_STT		= new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Dir				= new _float3[iInstanceCount];
	m_pInstance_Pos_UpdateTime	= new _double[iInstanceCount];
	m_pInstanceBuffer_LocalPos	= new _float4[iInstanceCount];

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight		= { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp			= { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook			= { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition		= { 0.f, 0.f, 0.f, 1.f };
		m_pInstanceBuffer_STT[iIndex].vSize			= { 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vTextureUV	= Get_TexUV_Rand(4, 2);
		m_pInstanceBuffer_STT[iIndex].fTime			= 0.0f;
		m_pInstance_Pos_UpdateTime[iIndex]			= m_fResetPosTime * 0.05f * _float(iIndex);
		m_pInstanceBuffer_LocalPos[iIndex]			= { 0.f, 0.f, 0.f, 1.f };
		m_pInstance_Dir[iIndex]						= Get_Dir_Rand(m_ivRandPower);
	}

	return S_OK;
}

_vector CEffect_Env_Particle::Set_RandPos_Default()
{
	_vector vDir	 = XMLoadFloat3(&Get_Dir_Rand(m_ivRandPower));

	vDir.m128_f32[0] *= m_vParticleRadius.x / _float(rand() % 5 + 1);
	vDir.m128_f32[1] *= m_vParticleRadius.y;
	vDir.m128_f32[2] *= m_vParticleRadius.z / _float(rand() % 5 + 1);

	return vDir;
}

_vector CEffect_Env_Particle::Set_RandPos_Umbrella()
{
	_vector vRandPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector vDir	 = XMLoadFloat3(&Get_Dir_Rand(m_ivRandPower));

	vRandPos.m128_f32[1] = m_vParticleRadius.y;
	vRandPos.m128_f32[1] *= _float(rand() % m_ivRandPower.y + 1) / m_ivRandPower.y;
	vDir.m128_f32[0] *= m_vParticleRadius.x / _float(rand() % 5 + 1);
	vDir.m128_f32[1] = 0.f;
	vDir.m128_f32[2] *= m_vParticleRadius.z / _float(rand() % 5 + 1);

	vRandPos += vDir;

	return vRandPos;
}

void CEffect_Env_Particle::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Env_Particle::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Env_Particle::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_Env_Particle * CEffect_Env_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Env_Particle*	pInstance = new CEffect_Env_Particle(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Env_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Env_Particle::Clone_GameObject(void * pArg)
{ 
	CEffect_Env_Particle* pInstance = new CEffect_Env_Particle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Env_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Env_Particle::Free()
{
	Safe_Release(m_pPointInstanceCom_STT);
	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstanceBuffer_LocalPos);

	__super::Free();                         
}
