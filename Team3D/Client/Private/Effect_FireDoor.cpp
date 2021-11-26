#include "stdafx.h"
#include "..\Public\Effect_FireDoor.h"
#include "GameInstance.h"

CEffect_FireDoor::CEffect_FireDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_FireDoor::CEffect_FireDoor(const CEffect_FireDoor & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_FireDoor::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_FireDoor::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_ColorRamp"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);
	
	m_pInstanceBuffer[0].vTextureUV = _float4(0.f, 0.f, 1.f, 1.f);
	m_vWeight = m_pInstanceBuffer[0].vTextureUV;

	m_vWeight = _float4(0.25f, 0.25f, 0.75f, 0.75f);
// 	PxPlaneGeometry
	m_IsBillBoard = false;
	return S_OK;
}

_int CEffect_FireDoor::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_float Time = (_float)TimeDelta * 0.3f;

	//m_vWeight.x += Time;
	m_vWeight.y += Time;
	//m_vWeight.z += TimeDelta * 0.15f;
	m_vWeight.w += Time;

	//m_pInstanceBuffer[0].vTextureUV.x = 0.f;
	//m_pInstanceBuffer[0].vTextureUV.y += Time;
	//m_pInstanceBuffer[0].vTextureUV.z = 0.f;
	//m_pInstanceBuffer[0].vTextureUV.w += Time;

	dResetTime += Time;
	if (1.f <= dResetTime)
	{
		dResetTime = 0.f;
		m_vWeight = _float4(0.25f, 0.25f, 0.75f, 0.75f);
		m_pInstanceBuffer[0].vTextureUV = _float4(0.f, 0.f, 1.f, 1.f);
	}

	m_pInstanceBuffer[0].vTextureUV.x = 0.f;
	m_pInstanceBuffer[0].vTextureUV.y = 0.f;
	m_pInstanceBuffer[0].vTextureUV.z = 1.f;
	m_pInstanceBuffer[0].vTextureUV.w = 1.f;

	m_pInstanceBuffer[0].vSize = { 6.f, 10.25f };
	m_pInstanceBuffer[0].vPosition = { 10.f, 5.f, 10.f,1.f };
	return _int();
}

_int CEffect_FireDoor::Late_Tick(_double TimeDelta)
{
	return  m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_FireDoor::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom->Get_ShaderResourceView(m_EffectDesc_Prototype.iTextureNum));

	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Second->Get_ShaderResourceView(m_EffectDesc_Prototype.iTextureNum_Second));

	_float4 vColorRamp = { 0.0f, 0.0f, 2.0f, 2.0f };
	m_pPointInstanceCom->Set_Variable("g_vUV", &m_vWeight, sizeof(_float4));
	m_pPointInstanceCom->Set_Variable("g_vColorRamp_UV", &vColorRamp, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(3));

	m_pPointInstanceCom->Render(3, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_FireDoor::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_FireDoor::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_FireDoor::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_FireDoor * CEffect_FireDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_FireDoor*	pInstance = new CEffect_FireDoor(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_FireDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_FireDoor::Clone_GameObject(void * pArg)
{
	CEffect_FireDoor* pInstance = new CEffect_FireDoor(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_FireDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_FireDoor::Free()
{
	Safe_Release(m_pTexturesCom_ColorRamp);

	__super::Free();
}
