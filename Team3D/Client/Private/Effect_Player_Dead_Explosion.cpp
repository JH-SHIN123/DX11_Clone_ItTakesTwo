#include "stdafx.h"
#include "..\Public\Effect_Player_Dead_Explosion.h"

CEffect_Player_Dead_Explosion::CEffect_Player_Dead_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Player_Dead_Explosion::CEffect_Player_Dead_Explosion(const CEffect_Player_Dead_Explosion & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Player_Dead_Explosion::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);


	return S_OK;
}

HRESULT CEffect_Player_Dead_Explosion::NativeConstruct(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Loop"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Wormhole_Noise"), TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Dist), E_FAIL);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);
	
	_matrix WorldMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	for (_int i = 0; i < 3; ++i)
		WorldMatrix.r[i] = XMVector3Normalize(WorldMatrix.r[i]);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_EffectDesc_Prototype.fLifeTime = 2.f;
	m_EffectDesc_Prototype.vSize = { 1.f,1.f,0.f };
	m_EffectDesc_Prototype.iInstanceCount = 2;
	 
	Ready_Instance();

	return S_OK;
}

_int CEffect_Player_Dead_Explosion::Tick(_double TimeDelta)
{
	if (5.0 <= m_dControlTime)
		return EVENT_DEAD;

	m_dControlTime += TimeDelta;
	if (3.5 < m_dControlTime)
		m_IsDisapear = true;

	//if (false == m_IsDisapear)
	//{
	//	m_dAlphaTime += TimeDelta * 0.5f;
	//	if (1.0 < m_dAlphaTime) m_dAlphaTime = 1.0;
	//}
	//else
	//{
		m_dAlphaTime -= TimeDelta * 0.5f;
		if (0.0 > m_dAlphaTime) m_dAlphaTime = 0.0;
	//}

	m_fTextureTime -= (_float)TimeDelta;

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		Instance_Size((_float)TimeDelta, iIndex);
		Instance_Pos((_float)TimeDelta, iIndex);

	}



	return _int();
}

_int CEffect_Player_Dead_Explosion::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_Player_Dead_Explosion::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	_float fAlpha = (_float)m_dAlphaTime;
	m_pPointInstanceCom->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(3));
	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Dist->Get_ShaderResourceView(0));

	m_pPointInstanceCom->Render(17, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Player_Dead_Explosion::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize.x += TimeDelta * 1.5f;
	m_pInstanceBuffer[iIndex].vSize.y += TimeDelta * 1.5f;
}

void CEffect_Player_Dead_Explosion::Instance_Pos(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vPosition.y += TimeDelta * 1.2f;
}

void CEffect_Player_Dead_Explosion::Instance_UV(_float TimeDelta, _int iIndex)
{
	if (0.f >= m_fTextureTime)
	{
		m_pInstanceBuffer[iIndex].vTextureUV = __super::Check_UV(m_vTexUV_Max.x, m_vTexUV_Max.y, &m_vTexUV.x, &m_vTexUV.y);
		m_fTextureTime = 0.05f;
	}
}

HRESULT CEffect_Player_Dead_Explosion::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	m_pInstanceBuffer	= new VTXMATRIX_CUSTOM_ST[iInstanceCount];

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_float4 TexUV = __super::Get_TexUV(m_vTexUV_Max.x, m_vTexUV_Max.y, true);
	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer[iIndex].vRight	= { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer[iIndex].vUp		= { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer[iIndex].vLook		= { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer[iIndex].vPosition = vPos;

		m_pInstanceBuffer[iIndex].vSize		 = m_vSize_Default;
		m_pInstanceBuffer[iIndex].vTextureUV = TexUV;
	}

	return S_OK;
}

_float4 CEffect_Player_Dead_Explosion::Set_particleUV(_int iIndex, _int U, _int V)
{
	_float fLeft	= (1.f / U) *  m_pInstance_UVCount[iIndex].x;
	_float fTop		= (1.f / V) *  m_pInstance_UVCount[iIndex].y;
	_float fRight	= (1.f / U) * (m_pInstance_UVCount[iIndex].x + 1.f);
	_float fBottom	= (1.f / V) * (m_pInstance_UVCount[iIndex].y + 1.f);

	_float4 vUV = { fLeft, fTop, fRight, fBottom };

	return vUV;
}

CEffect_Player_Dead_Explosion * CEffect_Player_Dead_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Player_Dead_Explosion*	pInstance = new CEffect_Player_Dead_Explosion(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Player_Dead_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Player_Dead_Explosion::Clone_GameObject(void * pArg)
{
	CEffect_Player_Dead_Explosion* pInstance = new CEffect_Player_Dead_Explosion(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Player_Dead_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Player_Dead_Explosion::Free()
{
	Safe_Release(m_pTexturesCom_Dist);

	__super::Free();
}
