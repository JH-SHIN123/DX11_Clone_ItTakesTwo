#include "stdafx.h"
#include "..\Public\Effect_Boss_Missile_Ring.h"

CEffect_Boss_Missile_Ring::CEffect_Boss_Missile_Ring(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Missile_Ring::CEffect_Boss_Missile_Ring(const CEffect_Boss_Missile_Ring & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Missile_Ring::NativeConstruct_Prototype(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 1;
	return S_OK;
}

HRESULT CEffect_Boss_Missile_Ring::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_ShockWave"), TEXT("Com_Texture_1"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_2"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_Buffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix));

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Boss_Missile_Ring::Tick(_double TimeDelta)
{
	if (false == m_IsActivate && 0.0 >= m_dActivateTime)
		return EVENT_DEAD;

	m_dActivateTime += TimeDelta;
	if (1.0 < m_dActivateTime)
		m_IsActivate = false;

	if (true == m_IsActivate)
	{
		m_dControlTime += TimeDelta;
		if (0.75 <= m_dControlTime) m_dControlTime = 0.75;
	}
	else
	{
		m_dControlTime -= TimeDelta * 0.5;
		if (0.0 > m_dControlTime) m_dControlTime = 0.0;
	}

	m_fSize_Power -= (_float)TimeDelta;
	if (0.f > m_fSize_Power)
		m_fSize_Power = 0.f;

	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Boss_Missile_Ring::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_Missile_Ring::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = (_float)m_dControlTime;
	_int i = 1;
	m_pPointInstanceCom->Set_DefaultVariables();

	m_pPointInstanceCom->Set_Variable("g_IsBillBoard", &i, sizeof(_int));
	m_pPointInstanceCom->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(1));
	m_pPointInstanceCom->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(3));

	m_pPointInstanceCom->Render(27, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_Missile_Ring::Check_Instance(_double TimeDelta)
{
	_int iInstanceCount_Max = m_EffectDesc_Prototype.iInstanceCount;

	for (_int iIndex = 0; iIndex < iInstanceCount_Max; ++iIndex)
		Instance_Size((_float)TimeDelta, iIndex);
}

void CEffect_Boss_Missile_Ring::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize.x += TimeDelta * m_fSize_Power * m_fSize_Power;
	m_pInstanceBuffer[iIndex].vSize.y += TimeDelta * m_fSize_Power * m_fSize_Power;
}

void CEffect_Boss_Missile_Ring::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Missile_Ring::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_Boss_Missile_Ring::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[iInstanceCount];


	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer[iIndex].vSize = m_vDefaultSize;

		m_pInstanceBuffer[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, WorldMatrix.r[3]);
	}

	return S_OK;
}

CEffect_Boss_Missile_Ring * CEffect_Boss_Missile_Ring::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Missile_Ring*	pInstance = new CEffect_Boss_Missile_Ring(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Missile_Ring");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Missile_Ring::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Missile_Ring* pInstance = new CEffect_Boss_Missile_Ring(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Missile_Ring");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Missile_Ring::Free()
{
	__super::Free();
}
