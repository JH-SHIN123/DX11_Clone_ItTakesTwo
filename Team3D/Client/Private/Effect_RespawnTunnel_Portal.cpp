#include "stdafx.h"
#include "..\Public\Effect_RespawnTunnel_Portal.h"
#include "Effect_Env_Particle.h"
#include "Effect_Env_Particle_Follow.h"

CEffect_RespawnTunnel_Portal::CEffect_RespawnTunnel_Portal(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_RespawnTunnel_Portal::CEffect_RespawnTunnel_Portal(const CEffect_RespawnTunnel_Portal & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_RespawnTunnel_Portal::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_RespawnTunnel_Portal::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Color_Ramp", TEXT("Com_Texture_Color"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_VIBuffer_PointInstance_Custom_STT", TEXT("Com_PointBuffer_Smoke"), (CComponent**)&m_pPointInstanceCom_Smoke), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Smoke_Flow_02", TEXT("Com_Textures_Smoke"), (CComponent**)&m_pTexturesCom_Smoke), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Smoke_Flow_01", TEXT("Com_Textures_Smoke2"), (CComponent**)&m_pTexturesCom_Smoke2), E_FAIL);

	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Effect", Level::LEVEL_STAGE, L"GameObject_2D_Env_Particle", &CEffect_Env_Particle::tagEnvParticle(CEffect_Env_Particle::Portal), (CGameObject**)&m_pParticle), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Effect", Level::LEVEL_STAGE, L"GameObject_2D_Env_Particle_Follow", &CEffect_Env_Particle::tagEnvParticle(CEffect_Env_Particle::Portal), (CGameObject**)&m_pParticleFollow), E_FAIL);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[0] *= -1.f;
	WorldMatrix.r[2] *= -1.f;
	WorldMatrix.r[3].m128_f32[1] += 7.f;
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_pTransformCom->Set_Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));

	//m_pParticle->Set_InstanceCount(100);
	//m_pParticle->Set_ParentMatrix(WorldMatrix);
	//m_pParticle->Set_Particle_Radius(_float3(5.5f, 5.5f, 1.f));

	m_pParticleFollow->Set_InstanceCount(80);
	m_pParticleFollow->Set_Follow_Distance(8.f);
	m_pParticleFollow->Set_ParentMatrix(WorldMatrix);
	m_pParticleFollow->Set_Particle_Radius(_float3(5.5f, 5.5f, 1.f));


	Ready_Instance();

	return S_OK;
}

_int CEffect_RespawnTunnel_Portal::Tick(_double TimeDelta)
{
	m_dAngle += 0.8;
	if (720.0 <= m_dAngle)
		m_dAngle = 0.0;

	m_fColorRamp_U += (_float)TimeDelta * 0.15f;

	if (2.f <= m_fColorRamp_U)
		m_fColorRamp_U = 0.f;

	//Check_Smoke(TimeDelta);

	m_dAlphaTime += TimeDelta * 0.5;
	if (1.0 < m_dAlphaTime)
		m_dAlphaTime = 1.0;

	//m_pParticle->Set_ControlTime(m_dAlphaTime);

	return _int();
}

_int CEffect_RespawnTunnel_Portal::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);

	return NO_EVENT;
}

HRESULT CEffect_RespawnTunnel_Portal::Render(RENDER_GROUP::Enum eGroup)
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	_float fAlphaTime	 = (_float)m_dAlphaTime;
	_float fRadian		 = XMConvertToRadians((_float)m_dAngle);
	_float2 vColorRampUV = { m_fColorRamp_U, 0.f };

	m_pModelCom->Set_Variable("g_fAlpha", &fAlphaTime, sizeof(_float));
	m_pModelCom->Set_Variable("g_vColorRamp_UV",&vColorRampUV, sizeof(_float2));
	m_pModelCom->Set_Variable("g_fRadianAngle",&fRadian, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(11));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(4);

	//// Smoke
	//Set_Shader_SmokeData();
	//m_pPointInstanceCom_Smoke->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Smoke2->Get_ShaderResourceView(0));
	//m_pPointInstanceCom_Smoke->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Smoke->Get_ShaderResourceView(0));
	//m_pPointInstanceCom_Smoke->Render(0, m_pInstanceBuffer_Smoke, m_iInstanceCount_Smoke, 0);

	return S_OK;
}

void CEffect_RespawnTunnel_Portal::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

HRESULT CEffect_RespawnTunnel_Portal::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom_Smoke)
		return S_OK;

	_int iInstanceCount = m_iInstanceCount_Smoke;

	m_pInstanceBuffer_Smoke				= new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstanceBuffer_Smoke_Dir			= new _float3[iInstanceCount];
	m_pInstanceBuffer_Smoke_PosReset	= new _double[iInstanceCount];
	m_pInstanceBuffer_Smoke_Spawn		= new _double[iInstanceCount];

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_Smoke[iIndex].vRight		= { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_Smoke[iIndex].vUp			= { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_Smoke[iIndex].vLook		= { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_Smoke[iIndex].vPosition	= vPos;
		m_pInstanceBuffer_Smoke[iIndex].vSize		= {0.f, 0.f};
		m_pInstanceBuffer_Smoke[iIndex].vTextureUV	= Get_TexUV(m_vTextureUV_Smoke.x, m_vTextureUV_Smoke.y, true);
		m_pInstanceBuffer_Smoke[iIndex].fTime		= 0.5f;

		m_pInstanceBuffer_Smoke_Spawn[iIndex]		= m_fSmoke_PosResetTime / iInstanceCount * iIndex;
		m_pInstanceBuffer_Smoke_PosReset[iIndex]	= m_fSmoke_PosResetTime;
		XMStoreFloat3(&m_pInstanceBuffer_Smoke_Dir[iIndex], Get_RandDir_Look());
	}

	return S_OK;
}

_float4 CEffect_RespawnTunnel_Portal::Get_TexUV(_int iTexture_U, _int iTexture_V, _bool IsRand)
{
	_float4 vTexUV		= { 0.f, 0.f, 1.f, 1.f };
	_float2 fStartUV	= { 0.f, 0.f };

	if (true == IsRand)
	{
		fStartUV.x = _float(rand() % iTexture_U);
		fStartUV.y = _float(rand() % iTexture_V);
	}

	_float fLeft	= (1.f / iTexture_U) *  fStartUV.x;
	_float fTop		= (1.f / iTexture_V) *  fStartUV.y;
	_float fRight	= (1.f / iTexture_U) * (fStartUV.x + 1.f);
	_float fBottom	= (1.f / iTexture_V) * (fStartUV.y + 1.f);

	vTexUV = { fLeft, fTop, fRight, fBottom };

	return vTexUV;
}

_vector CEffect_RespawnTunnel_Portal::Get_RandDir_Look()
{
	_float fRand = _float(rand() % 7 + 1);
	_vector vBaseDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.f * fRand;

	_vector vRandDir = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	if (rand() % 2 == 0)
		vRandDir *= -1.f;

	vBaseDir += vRandDir;

	return XMVector3Normalize(vBaseDir);
}

void CEffect_RespawnTunnel_Portal::Check_Smoke(_double TimeDelta)
{
	m_dTexUV_Time -= TimeDelta;

	for (_int iIndex = 0; iIndex < m_iInstanceCount_Smoke; ++iIndex)
	{
		if (0.0 <= m_pInstanceBuffer_Smoke_Spawn[iIndex])
		{
			m_pInstanceBuffer_Smoke_Spawn[iIndex] -= TimeDelta;
			continue;
		}
		else
		{
			if (m_fSmoke_PosResetTime == m_pInstanceBuffer_Smoke_PosReset[iIndex])
			{
				m_pInstanceBuffer_Smoke[iIndex].vSize = m_vDefaultSize_Smoke;
				m_pInstanceBuffer_Smoke_PosReset[iIndex] -= TimeDelta;
				XMStoreFloat4(&m_pInstanceBuffer_Smoke[iIndex].vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

				continue;
			}
		}
		m_pInstanceBuffer_Smoke_PosReset[iIndex] -= TimeDelta;
		if((m_fSmoke_PosResetTime * 0.5f) <= m_pInstanceBuffer_Smoke_PosReset[iIndex])
			m_pInstanceBuffer_Smoke[iIndex].fTime += (_float)TimeDelta * m_fSmoke_AlphaPower;
		else
			m_pInstanceBuffer_Smoke[iIndex].fTime -= (_float)TimeDelta * m_fSmoke_AlphaPower;


		if (0.0 >= m_pInstanceBuffer_Smoke_PosReset[iIndex])
		{
			m_pInstanceBuffer_Smoke[iIndex].fTime = 0.f;
			m_pInstanceBuffer_Smoke_PosReset[iIndex] = m_fSmoke_PosResetTime;
			XMStoreFloat4(&m_pInstanceBuffer_Smoke[iIndex].vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat3(&m_pInstanceBuffer_Smoke_Dir[iIndex], Get_RandDir_Look());
		}

		m_pInstanceBuffer_Smoke[iIndex].vSize.x += (_float)TimeDelta * 0.125f;
		m_pInstanceBuffer_Smoke[iIndex].vSize.y += (_float)TimeDelta * 0.125f;

		_vector vDir = XMLoadFloat3(&m_pInstanceBuffer_Smoke_Dir[iIndex]);
		_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_Smoke[iIndex].vPosition) + (vDir * (_float)TimeDelta * 1.f);
		XMStoreFloat4(&m_pInstanceBuffer_Smoke[iIndex].vPosition , vPos);

		if(0.0 >= m_dTexUV_Time)
			m_pInstanceBuffer_Smoke[iIndex].vTextureUV = Check_TexUV(m_pInstanceBuffer_Smoke[iIndex].vTextureUV);
	}
	//D3D11_RENDER_TARGET_BLEND_DESC
	if (0.0 >= m_dTexUV_Time)
		m_dTexUV_Time = m_dTexUV_TimeMax;

}

_float4 CEffect_RespawnTunnel_Portal::Check_TexUV(_float4 vNowUV)
{
	XMINT4 vUV_Count = { _int(vNowUV.x * m_vTextureUV_Smoke.x), _int(vNowUV.y * m_vTextureUV_Smoke.y), _int(vNowUV.z * m_vTextureUV_Smoke.x), _int(vNowUV.w * m_vTextureUV_Smoke.y) };

	if (vUV_Count.x >= m_vTextureUV_Smoke.x - 1)
	{
		vUV_Count.x = 0;
		++vUV_Count.y;
	}
	else
		++vUV_Count.x;

	if (vUV_Count.y >= m_vTextureUV_Smoke.y)
		vUV_Count.y = 0;

	_float fLeft	= (_float)(1.f / m_vTextureUV_Smoke.x) *  vUV_Count.x;
	_float fTop		= (_float)(1.f / m_vTextureUV_Smoke.y) *  vUV_Count.y;
	_float fRight	= (_float)(1.f / m_vTextureUV_Smoke.x) * (vUV_Count.x + 1);
	_float fBottom	= (_float)(1.f / m_vTextureUV_Smoke.y) * (vUV_Count.y + 1);

	vNowUV = { fLeft, fTop, fRight, fBottom };

	return vNowUV;
}

void CEffect_RespawnTunnel_Portal::Set_Shader_SmokeData()
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	if (nullptr == pPipeline)
		return;

	m_pPointInstanceCom_Smoke->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pPointInstanceCom_Smoke->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINVIEW)), sizeof(_matrix));
	m_pPointInstanceCom_Smoke->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ)), sizeof(_matrix));
	m_pPointInstanceCom_Smoke->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBVIEW)), sizeof(_matrix));
	m_pPointInstanceCom_Smoke->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ)), sizeof(_matrix));

	_float	fMainCamFar = pPipeline->Get_MainCamFar();
	_float	fSubCamFar = pPipeline->Get_SubCamFar();
	m_pPointInstanceCom_Smoke->Set_Variable("g_fMainCamFar", &fMainCamFar, sizeof(_float));
	m_pPointInstanceCom_Smoke->Set_Variable("g_fSubCamFar", &fSubCamFar, sizeof(_float));

	_vector vMainCamPosition = pPipeline->Get_MainCamPosition();
	_vector vSubCamPosition = pPipeline->Get_SubCamPosition();
	m_pPointInstanceCom_Smoke->Set_Variable("g_vMainCamPosition", &vMainCamPosition, sizeof(_vector));
	m_pPointInstanceCom_Smoke->Set_Variable("g_vSubCamPosition", &vSubCamPosition, sizeof(_vector));

	return;
}

CEffect_RespawnTunnel_Portal * CEffect_RespawnTunnel_Portal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_RespawnTunnel_Portal*	pInstance = new CEffect_RespawnTunnel_Portal(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_RespawnTunnel_Portal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_RespawnTunnel_Portal::Clone_GameObject(void * pArg)
{
	CEffect_RespawnTunnel_Portal* pInstance = new CEffect_RespawnTunnel_Portal(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_RespawnTunnel_Portal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_RespawnTunnel_Portal::Free()
{
	Safe_Release(m_pTexturesCom_ColorRamp);
	Safe_Release(m_pPointInstanceCom_Smoke);
	Safe_Release(m_pTexturesCom_Smoke);
	Safe_Release(m_pTexturesCom_Smoke2);
	Safe_Release(m_pParticleFollow);
	Safe_Release(m_pParticle);

	Safe_Delete_Array(m_pInstanceBuffer_Smoke);
	Safe_Delete_Array(m_pInstanceBuffer_Smoke_Dir);
	Safe_Delete_Array(m_pInstanceBuffer_Smoke_Spawn);
	Safe_Delete_Array(m_pInstanceBuffer_Smoke_PosReset);

	__super::Free();
}
