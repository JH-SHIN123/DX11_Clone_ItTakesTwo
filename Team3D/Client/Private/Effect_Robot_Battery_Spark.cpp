#include "stdafx.h"
#include "..\Public\Effect_Robot_Battery_Spark.h"

CEffect_Robot_Battery_Spark::CEffect_Robot_Battery_Spark(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Robot_Battery_Spark::CEffect_Robot_Battery_Spark(const CEffect_Robot_Battery_Spark & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Robot_Battery_Spark::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Robot_Battery_Spark::NativeConstruct(void * pArg)
{
	__super::Ready_Component(pArg);

	m_EffectDesc_Prototype.fLifeTime = 0.25f;
	m_fResetTime = m_EffectDesc_Prototype.fLifeTime;

	Ready_Instance();

	return S_OK;
}

_int CEffect_Robot_Battery_Spark::Tick(_double TimeDelta)
{
	// KA-BOOOOM

	if (-1 == m_iEffectCount)
		return EVENT_DEAD;

	m_IsNoRender = false;
	Check_Particle_UV(TimeDelta);
	if (3 == m_iEffectCount)
		m_IsNoRender = true;


	return _int();
}

_int CEffect_Robot_Battery_Spark::Late_Tick(_double TimeDelta)
{
	if (true == m_IsNoRender)
		return NO_EVENT;

	if (7 == m_iEffectCount)
	{
		m_iEffectCount = -1;
		return NO_EVENT;
	}

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Robot_Battery_Spark::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	_float4 vColor = m_vColor;
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(8, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

HRESULT CEffect_Robot_Battery_Spark::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom) 
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	m_pInstanceBuffer	= new VTXMATRIX_CUSTOM_ST[iInstanceCount];
	_vector vDir		= m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vLook		= m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float2 vSize		= { m_EffectDesc_Prototype.vSize.x ,m_EffectDesc_Prototype.vSize.y };
	_vector vOffSetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vOffSetPos += vDir * 0.85f;
	_float4 vPos;
	XMStoreFloat4(&vPos, vOffSetPos);

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer[iIndex].vPosition		= vPos;
		m_pInstanceBuffer[iIndex].vSize			= { 0.15f, 0.15f };
		m_pInstanceBuffer[iIndex].vTextureUV	= Get_TexUV_Rand(8, 8);

		_vector vUp = Get_RandDir(_int3(100, 100, 100), vDir);
		_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
		_vector vReal_Look = XMVector3Normalize(XMVector3Cross(vRight, vUp));
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vUp, vUp);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vRight, vRight);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vLook,	vReal_Look);
	}	

	return S_OK;
}

void CEffect_Robot_Battery_Spark::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Robot_Battery_Spark::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Robot_Battery_Spark::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Robot_Battery_Spark::Check_Particle_UV(_double TimeDelta)
{
	// 0.25초씩 3번 UV바꿈 0.75
	// 0.25초 안그림
	// 0.25초씩 3번 UV바꿈 0.75
	// 끝
	m_fResetTime -= (_float)TimeDelta;

	if (0.f >= m_fResetTime)
	{
		m_fResetTime = 0.15f;

		_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
		m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[iInstanceCount];
		_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_float2 vSize = { m_EffectDesc_Prototype.vSize.x ,m_EffectDesc_Prototype.vSize.y };
		_vector vOffSetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vOffSetPos += vDir * 0.875f;
		_float4 vPos;
		XMStoreFloat4(&vPos, vOffSetPos);
		for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
		{
			m_pInstanceBuffer[iIndex].vPosition = vPos;
			m_pInstanceBuffer[iIndex].vSize = { 0.15f, 0.15f };
			m_pInstanceBuffer[iIndex].vTextureUV = Get_TexUV_Rand(8, 8);

			_vector vUp = Get_RandDir(_int3(100, 100, 100), vDir);
			_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
			_vector vReal_Look = XMVector3Normalize(XMVector3Cross(vRight, vUp));
			XMStoreFloat4(&m_pInstanceBuffer[iIndex].vUp, vUp);
			XMStoreFloat4(&m_pInstanceBuffer[iIndex].vRight, vRight);
			XMStoreFloat4(&m_pInstanceBuffer[iIndex].vLook, vReal_Look);
		}

		++m_iEffectCount;
	}
}

_vector CEffect_Robot_Battery_Spark::Get_RandDir(_int3 iRandPower, _vector vDir)
{
	_vector vRandDir = XMLoadFloat3(&Get_Dir_Rand(iRandPower));
	vRandDir += vDir;

	vRandDir = XMVector3Normalize(vRandDir);
	vRandDir.m128_f32[3] = 0.f;

	return vRandDir;
}

CEffect_Robot_Battery_Spark * CEffect_Robot_Battery_Spark::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CEffect_Robot_Battery_Spark*	pInstance = new CEffect_Robot_Battery_Spark(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Robot_Battery_Spark");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Robot_Battery_Spark::Clone_GameObject(void * pArg)
{
	CEffect_Robot_Battery_Spark* pInstance = new CEffect_Robot_Battery_Spark(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Robot_Battery_Spark");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Robot_Battery_Spark::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer);

	__super::Free();
}
