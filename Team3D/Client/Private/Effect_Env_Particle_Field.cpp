#include "stdafx.h"
#include "..\Public\Effect_Env_Particle_Field.h"

CEffect_Env_Particle_Field::CEffect_Env_Particle_Field(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Env_Particle_Field::CEffect_Env_Particle_Field(const CEffect_Env_Particle_Field & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Env_Particle_Field::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Env_Particle_Field::NativeConstruct(void * pArg)
{
	memcpy(&m_Particle_Desc, pArg, sizeof(ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_PointBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE,	m_EffectDesc_Prototype.TextureName, TEXT("Com_Textures"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName_Second, TEXT("Com_Textures_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_Particle_Desc.vPosition));

	Set_InstanceCount(m_Particle_Desc.iInstanceCount);
	Set_Particle_Radius(m_Particle_Desc.vRadiusXYZ);

	return S_OK;
}

_int CEffect_Env_Particle_Field::Tick(_double TimeDelta)
{
	Check_State(TimeDelta);

	return _int();
}

_int CEffect_Env_Particle_Field::Late_Tick(_double TimeDelta)
{
	if (0.0 >= m_dControl_Time)
		return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Env_Particle_Field::Render(RENDER_GROUP::Enum eGroup)
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

void CEffect_Env_Particle_Field::Set_InstanceCount(_uint iInstanceCount)
{
	m_EffectDesc_Prototype.iInstanceCount = iInstanceCount;
	Initialize_Instance();
}

void CEffect_Env_Particle_Field::Set_Particle_Radius(_float3 vRadiusXYZ)
{
	m_vParticleRadius.x = vRadiusXYZ.x;
	m_vParticleRadius.y = vRadiusXYZ.y;
	m_vParticleRadius.z = vRadiusXYZ.z;
}

void CEffect_Env_Particle_Field::Set_ParentMatrix(_fmatrix ParentMatrix)
{
	m_pTransformCom->Set_WorldMatrix(ParentMatrix);
}

void CEffect_Env_Particle_Field::Set_ControlTime(_double dControlTime)
{
	m_eStateValue_Next = CEffect_Env_Particle_Field::STATE_START;

	if (0.0 >= dControlTime)
	{
		dControlTime = 0.0;
		m_eStateValue_Next = CEffect_Env_Particle_Field::STATE_DISAPPEAR;
	}

	m_dControl_Time = dControlTime;
}

void CEffect_Env_Particle_Field::Check_State(_double TimeDelta)
{
	if (nullptr == m_pInstanceBuffer_STT)
		return;

	switch (m_eStateValue_Next)
	{
	case CEffect_Env_Particle_Field::STATE_START:
		State_Start(TimeDelta);
		break;
	case  CEffect_Env_Particle_Field::STATE_DISAPPEAR:
		State_Disappear(TimeDelta);
		break;
	default:
		m_eStateValue_Next = STATE_DISAPPEAR;
		break;
	}
}

void CEffect_Env_Particle_Field::State_Start(_double TimeDelta)
{
	_float fTimeDelta = (_float)TimeDelta;
	_fvector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (1.0 <= m_dControl_Time)
		m_dControl_Time = 1.0;

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (0.f >= m_pInstance_Pos_UpdateTime[iIndex])
			Reset_Instance(iIndex, vWorldPos);

		m_pInstanceBuffer_STT[iIndex].fTime += fTimeDelta  * (_float)m_dControl_Time;
		if (1.f <= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 1.f;

		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
		_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);
		vPos += vDir * fTimeDelta * m_Particle_Desc.fSpeedPerSec;
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);

		m_pInstanceBuffer_STT[iIndex].vSize.x -= fTimeDelta * m_Particle_Desc.fReSizing_Power;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= fTimeDelta * m_Particle_Desc.fReSizing_Power;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x)
			m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };
	}
}

void CEffect_Env_Particle_Field::State_Disappear(_double TimeDelta)
{
	_float fTimeDelta = (_float)TimeDelta;
	_fvector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (0.0 >= m_dControl_Time)
	{
		m_dControl_Time = 0.0;
		return;
	}

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (0.f >= m_pInstance_Pos_UpdateTime[iIndex])
			Reset_Instance(iIndex, vWorldPos);

		m_pInstanceBuffer_STT[iIndex].fTime += fTimeDelta  * (_float)m_dControl_Time;
		if (1.f <= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 1.f;

		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
		_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);
		vPos += vDir * fTimeDelta * m_Particle_Desc.fSpeedPerSec;
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);

		m_pInstanceBuffer_STT[iIndex].vSize.x -= fTimeDelta * m_Particle_Desc.fReSizing_Power;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= fTimeDelta * m_Particle_Desc.fReSizing_Power;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x)
			m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };
	}
}

_float4 CEffect_Env_Particle_Field::Get_Rand_Pos()
{
	_vector vRandPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	vRandPos = Set_RandPos_Default();

	_float4 vPos;
	XMStoreFloat4(&vPos, vRandPos);
	return vPos;
}

_float2 CEffect_Env_Particle_Field::Get_Rand_Size(_float2 vDefaultSize, _float vReSize)
{
	_float2 vRandSize = vDefaultSize;

	_int iRand = rand() % 3;
	if (0 == iRand)
	{
		vRandSize.x -= vReSize;
		vRandSize.y -= vReSize;
	}
	else if (1 == iRand)
	{
		vRandSize.x += vReSize;
		vRandSize.y += vReSize;
	}

	return vRandSize;
}

HRESULT CEffect_Env_Particle_Field::Reset_Instance(_int iIndex, _fvector vWorldPos)
{
	_vector vPos = vWorldPos + XMLoadFloat4(&Get_Rand_Pos());
	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);

	m_pInstanceBuffer_STT[iIndex].vSize = Get_Rand_Size(m_Particle_Desc.vDefaultSize, m_Particle_Desc.fReSize);
	m_pInstanceBuffer_STT[iIndex].vTextureUV = Get_TexUV_Rand(m_Particle_Desc.vTextureUV.x, m_Particle_Desc.vTextureUV.y);
	m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
	m_pInstance_Dir[iIndex] = Get_Dir_Rand(_int3(100, 100, 100));
	m_pInstance_Pos_UpdateTime[iIndex] = m_Particle_Desc.fResetPosTime;

	_float fRotateAngle = 1.f;
	if (rand() % 2 == 0)
		fRotateAngle *= -1.f;
	m_pInstanceBuffer_STT[iIndex].vRight.y = (_float)(rand() % 5 + 5) * 5.f * fRotateAngle;

	return S_OK;
}

HRESULT CEffect_Env_Particle_Field::Reset_Instance_All()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	_vector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		_vector vPos = vWorldPos + XMLoadFloat4(&Get_Rand_Pos());
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
		m_pInstance_Dir[iIndex] = Get_Dir_Rand(_int3(100, 100, 100));

		_float fRotateAngle = 1.f;
		if (rand() % 2 == 0)
			fRotateAngle *= 1;
		m_pInstanceBuffer_STT[iIndex].vRight.y = (_float)(rand() % 5 + 5) * 10.f * fRotateAngle;
	}

	return S_OK;
}

HRESULT CEffect_Env_Particle_Field::Initialize_Instance()
{
	if (nullptr == m_pPointInstanceCom_STT)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Dir = new _float3[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];

	_vector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		if (true == m_Particle_Desc.IsGrouping)
		{
			Initialize_Instance_Goruping(&iIndex, iInstanceCount, vWorldPos);
			continue;
		}

		m_pInstanceBuffer_STT[iIndex].vRight	= { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook		= { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vSize		= { 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vTextureUV = Get_TexUV_Rand(m_Particle_Desc.vTextureUV.x, m_Particle_Desc.vTextureUV.y);
		m_pInstanceBuffer_STT[iIndex].fTime		= 0.0f;

		_vector vPos = vWorldPos + XMLoadFloat4(&Get_Rand_Pos());
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);

		m_pInstance_Pos_UpdateTime[iIndex] = m_Particle_Desc.fResetPosTime * m_Particle_Desc.fInitialize_UpdatePos_Term * _float(iIndex) / iInstanceCount;
		m_pInstance_Dir[iIndex] = Get_Dir_Rand(m_Particle_Desc.vRandPower);

		_float fRotateAngle = 1.f;
		if (rand() % 2 == 0)
			fRotateAngle *= 1;
		m_pInstanceBuffer_STT[iIndex].vRight.y = (_float)(rand() % 5 + 5) * 10.f * fRotateAngle;
	}

	return S_OK;
}

void CEffect_Env_Particle_Field::Initialize_Instance_Goruping(_int* iIndex, _int iInstance_Count, _fvector vWorldPos)
{
	_int iIndexGroup = *iIndex;
	for (; iIndexGroup < *iIndex + m_Particle_Desc.iGrouping_Count;)
	{
		if (iIndexGroup >= iInstance_Count)
			break;

		m_pInstanceBuffer_STT[iIndexGroup].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndexGroup].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndexGroup].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndexGroup].vSize = { 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndexGroup].vTextureUV = Get_TexUV_Rand(m_Particle_Desc.vTextureUV.x, m_Particle_Desc.vTextureUV.y);
		m_pInstanceBuffer_STT[iIndexGroup].fTime = 0.0f;

		_vector vPos = vWorldPos + XMLoadFloat4(&Get_Rand_Pos());
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndexGroup].vPosition, vPos);

		m_pInstance_Pos_UpdateTime[iIndexGroup] = m_Particle_Desc.fResetPosTime * m_Particle_Desc.fInitialize_UpdatePos_Term * _float(*iIndex) / iInstance_Count;
		m_pInstance_Dir[iIndexGroup] = Get_Dir_Rand(m_Particle_Desc.vRandPower);

		_float fRotateAngle = 1.f;
		if (rand() % 2 == 0)
			fRotateAngle *= 1;
		m_pInstanceBuffer_STT[iIndexGroup].vRight.y = (_float)(rand() % 5 + 5) * 10.f * fRotateAngle;

		++iIndexGroup;
	}

	*iIndex = iIndexGroup - 1;
}

_vector CEffect_Env_Particle_Field::Set_RandPos_Default()
{
	_vector vDir = XMLoadFloat3(&Get_Dir_Rand(m_Particle_Desc.vRandPower));

	vDir.m128_f32[0] *= m_vParticleRadius.x / _float(rand() % 5 + 1);
	vDir.m128_f32[1] *= m_vParticleRadius.y;
	vDir.m128_f32[2] *= m_vParticleRadius.z / _float(rand() % 5 + 1);

	return vDir;
}

void CEffect_Env_Particle_Field::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Env_Particle_Field::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Env_Particle_Field::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_Env_Particle_Field * CEffect_Env_Particle_Field::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Env_Particle_Field*	pInstance = new CEffect_Env_Particle_Field(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Env_Particle_Field");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Env_Particle_Field::Clone_GameObject(void * pArg)
{
	CEffect_Env_Particle_Field* pInstance = new CEffect_Env_Particle_Field(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Env_Particle_Field");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Env_Particle_Field::Free()
{
	Safe_Release(m_pPointInstanceCom_STT);
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	__super::Free();
}
