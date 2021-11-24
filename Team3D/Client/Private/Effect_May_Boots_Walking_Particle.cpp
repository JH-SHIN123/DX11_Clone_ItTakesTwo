#include "stdafx.h"
#include "..\Public\Effect_May_Boots_Walking_Particle.h"
#include "GameInstance.h"

CEffect_May_Boots_Walking_Particle::CEffect_May_Boots_Walking_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_May_Boots_Walking_Particle::CEffect_May_Boots_Walking_Particle(const CEffect_May_Boots_Walking_Particle & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_May_Boots_Walking_Particle::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_May_Boots_Walking_Particle::NativeConstruct(void * pArg)
{
	__super::Ready_Component(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 40;
	m_EffectDesc_Prototype.fLifeTime = 3.f;
	m_EffectDesc_Prototype.iTextureCount_U = 8;
	m_EffectDesc_Prototype.iTextureCount_V = 8;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Puff_02"), TEXT("Com_Texture_Smoke"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	m_EffectDesc_Clone.UVTime = 0.01f;
	
	Ready_Smoke_Effect();
	Ready_Instance();

	return S_OK;
}

_int CEffect_May_Boots_Walking_Particle::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	m_pInstanceBuffer_Smoke[0].vTextureUV = Check_UV_Smoke(TimeDelta);

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		Check_Point_Move(TimeDelta, iIndex);
		Check_Point_Size(TimeDelta, iIndex);
	}

	m_fSpeedPerSec -= (_float)TimeDelta * 0.9f;
	//if (0.f >= m_fSpeedPerSec)
	//	m_fSpeedPerSec = 0.f;

	return NO_EVENT;
}

_int CEffect_May_Boots_Walking_Particle::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_May_Boots_Walking_Particle::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	// 연기
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Second->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(1, m_pInstanceBuffer_Smoke, 1);

	// 나머지 파티클
	_float4 vColor = { 0.499411793f, 0.787843208f, 0.980392218f, 1.000000000f }; //LightSkyBlue
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(5, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);



	return S_OK;
}

void CEffect_May_Boots_Walking_Particle::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_May_Boots_Walking_Particle::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_May_Boots_Walking_Particle::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_May_Boots_Walking_Particle::Ready_Smoke_Effect()
{
	m_pInstanceBuffer_Smoke = new VTXMATRIX_CUSTOM_ST[1];

	m_pInstanceBuffer_Smoke[0].vRight		= { 1.f, 0.f, 0.f, 0.f };
	m_pInstanceBuffer_Smoke[0].vUp			= { 0.f, 1.f, 0.f, 0.f };
	m_pInstanceBuffer_Smoke[0].vLook		= { 0.f, 0.f, 1.f, 0.f };
	XMStoreFloat4(&m_pInstanceBuffer_Smoke[0].vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pInstanceBuffer_Smoke[0].vTextureUV	= { 0.f, 0.f, 1.f, 1.f };;
	m_pInstanceBuffer_Smoke[0].vSize		= { 1.5f, 1.5f };
	return S_OK;
}

HRESULT CEffect_May_Boots_Walking_Particle::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer			= new VTXMATRIX_CUSTOM_ST[iInstanceCount];
	m_pInstance_Dir				= new _float3[iInstanceCount];

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = XMLoadFloat3(&m_EffectDesc_Clone.vDir);

	memcpy(m_pInstanceBuffer, m_pPointInstanceCom->Get_InstanceBuffer(), sizeof(VTXMATRIX_CUSTOM_ST) * iInstanceCount);

	for (_int i = 0; i < iInstanceCount; ++i)
	{
		_int iRandSize = rand() % 3;
		if (0 == iRandSize)
			m_pInstanceBuffer[i].vSize = {0.0625f, 0.0625f };
		else if (1 == iRandSize)
			m_pInstanceBuffer[i].vSize = { 0.09375f, 0.09375f };
		else
			m_pInstanceBuffer[i].vSize = { 0.125f, 0.125f };

		_vector vRandDir = XMVectorZero();
		_int iDir[3] = { 0, 0, 0 };

		for (_int j = 0; j < 3; ++j)
		{
			iDir[j] = rand() % 20 + 1;
			if (true == m_EffectDesc_Clone.IsRandDirDown[j] && rand() % 2 == 0)
				iDir[j] *= -1;

			vRandDir.m128_f32[j] = (_float)iDir[j];
		}
		if (0.f >= vRandDir.m128_f32[1])
			vRandDir.m128_f32[1] *= -1.f;
		vRandDir.m128_f32[1] += 2.f;

		vRandDir = XMVector3Normalize(vRandDir);

		vPos.m128_f32[0] += vRandDir.m128_f32[0] * 0.01f;
		vPos.m128_f32[2] += vRandDir.m128_f32[2] * 0.01f;

		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vPos);
		XMStoreFloat3(&m_pInstance_Dir[i], vRandDir);	
	}

	return S_OK;
}

_float4 CEffect_May_Boots_Walking_Particle::Check_UV_Smoke(_double TimeDelta)
{
	_float4 vUV = m_pInstanceBuffer_Smoke[0].vTextureUV;

	if (-1.f == m_EffectDesc_Clone.UVTime)
		return vUV;

	m_UVTime -= TimeDelta;

	if (m_UVTime <= 0.f)
	{

		if (m_vTexUV_Smoke.y == m_EffectDesc_Prototype.iTextureCount_V - 1)
		{
			if (m_vTexUV_Smoke.x == m_EffectDesc_Prototype.iTextureCount_U - 1)
				return vUV = { 1.f, 1.f, 1.f, 1.f };
		}
		

		m_UVTime = m_EffectDesc_Clone.UVTime;

		if (m_vTexUV_Smoke.x >= m_EffectDesc_Prototype.iTextureCount_U - 1)
		{
			m_vTexUV_Smoke.x = 0;
			m_vTexUV_Smoke.y += 1.f;
		}
		else
			m_vTexUV_Smoke.x += 1.f;

		if (m_vTexUV_Smoke.y >= m_EffectDesc_Prototype.iTextureCount_V)
			m_vTexUV_Smoke.y = 0;

		_float fLeft	= (1.f / m_EffectDesc_Prototype.iTextureCount_U) *  m_vTexUV_Smoke.x;
		_float fTop		= (1.f / m_EffectDesc_Prototype.iTextureCount_V) *  m_vTexUV_Smoke.y;
		_float fRight	= (1.f / m_EffectDesc_Prototype.iTextureCount_U) * (m_vTexUV_Smoke.x + 1.f);
		_float fBottom	= (1.f / m_EffectDesc_Prototype.iTextureCount_V) * (m_vTexUV_Smoke.y + 1.f);

		vUV = { fLeft, fTop, fRight, fBottom };
	}

	return vUV;
}

void CEffect_May_Boots_Walking_Particle::Check_Point_Move(_double TimeDelta, _int iIndex)
{
	//일단 dir
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);

	vPos += vDir * (_float)TimeDelta * m_fSpeedPerSec;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

void CEffect_May_Boots_Walking_Particle::Check_Point_Size(_double TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.125f;
	m_pInstanceBuffer[iIndex].vSize.y -= (_float)TimeDelta * 0.125f;

	if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
	{
		m_pInstanceBuffer[iIndex].vSize.x = 0.f;
		m_pInstanceBuffer[iIndex].vSize.y = 0.f;
	}
}

CEffect_May_Boots_Walking_Particle * CEffect_May_Boots_Walking_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_May_Boots_Walking_Particle*	pInstance = new CEffect_May_Boots_Walking_Particle(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_May_Boots_Walking_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_May_Boots_Walking_Particle::Clone_GameObject(void * pArg)
{
	CEffect_May_Boots_Walking_Particle* pInstance = new CEffect_May_Boots_Walking_Particle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_May_Boots_Walking_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_May_Boots_Walking_Particle::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_Smoke);

	__super::Free();
}
