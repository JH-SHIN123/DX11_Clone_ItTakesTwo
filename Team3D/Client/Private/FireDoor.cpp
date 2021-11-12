#include "stdafx.h"
#include "..\Public\FireDoor.h"
#include "GameInstance.h"

CFireDoor::CFireDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameEffect(pDevice, pDeviceContext)
{
}

CFireDoor::CFireDoor(const CFireDoor & rhs)
	: CGameEffect(rhs)
{
}

HRESULT CFireDoor::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CFireDoor::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Textrue_ColorRamp"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);

	
	m_pInstanceBuffer[0].vTextureUV = _float4(0.f, 0.f, 1.f, 1.f);
	m_vWeight = m_pInstanceBuffer[0].vTextureUV;

	m_vWeight = _float4(0.25f, 0.25f, 0.75f, 0.75f);

	return S_OK;
}

_int CFireDoor::Tick(_double TimeDelta)
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

	fT += Time;
	if (1.f <= fT)
	{
		fT = 0.f;
		m_vWeight = _float4(0.25f, 0.25f, 0.75f, 0.75f);
		m_pInstanceBuffer[0].vTextureUV = _float4(0.f, 0.f, 1.f, 1.f);
	}

	m_pInstanceBuffer[0].vTextureUV.x = 0.f;
	m_pInstanceBuffer[0].vTextureUV.y = 0.f;
	m_pInstanceBuffer[0].vTextureUV.z = 1.f;
	m_pInstanceBuffer[0].vTextureUV.w = 1.f;



	return _int();
}

_int CFireDoor::Late_Tick(_double TimeDelta)
{
	return  m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CFireDoor::Render()
{

	SetUp_Shader_Data();
	//_float2 fUV = {0.f,}

	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom->Get_ShaderResourceView(m_EffectDesc_Prototype.iTextureNum));

	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Second->Get_ShaderResourceView(m_EffectDesc_Prototype.iTextureNum_Second));

	_float4 vColorRamp = { 0.0f, 0.0f, 2.0f, 2.0f };
	m_pPointInstanceCom->Set_Variable("g_vUV", &m_vWeight, sizeof(_float4));
	m_pPointInstanceCom->Set_Variable("g_vColorRamp_UV", &vColorRamp, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(3));

	m_pPointInstanceCom->Render(3, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

CFireDoor * CFireDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CFireDoor*	pInstance = new CFireDoor(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CFireDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFireDoor::Clone_GameObject(void * pArg)
{
	CFireDoor* pInstance = new CFireDoor(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CFireDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireDoor::Free()
{
	Safe_Release(m_pTexturesCom_ColorRamp);

	__super::Free();
}
