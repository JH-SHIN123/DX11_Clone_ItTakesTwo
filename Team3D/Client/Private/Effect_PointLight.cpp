#include "stdafx.h"
#include "..\Public\Effect_PointLight.h"

CEffect_PointLight::CEffect_PointLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_PointLight::CEffect_PointLight(const CEffect_PointLight & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_PointLight::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_PointLight::NativeConstruct(void * pArg)
{
	memcpy(&m_PointLight_Desc, pArg, sizeof(CEffect_Generator::Effect_PointLight_Desc));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName, TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_2"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	//
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);

	_float4 vPos = { m_PointLight_Desc.vPosition.x, m_PointLight_Desc.vPosition.y, m_PointLight_Desc.vPosition.z, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	_float vSize = m_PointLight_Desc.fPointRadius;
	m_pTransformCom->Set_Scale(XMVectorSet(vSize, vSize, vSize, 0.f));

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_PointLight::Tick(_double TimeDelta)
{
	return _int();
}

_int CEffect_PointLight::Late_Tick(_double TimeDelta)
{

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_PointLight::Render(RENDER_GROUP::Enum eGroup)
{
	m_pPointInstanceCom->Set_DefaultVariables();
	m_pPointInstanceCom->Set_Variable("g_vColor", &m_PointLight_Desc.vDiffuseColor, sizeof(_float4));
	m_pPointInstanceCom->Set_Variable("g_fPointContrast_Power", &m_PointLight_Desc.fPointContrast_Power, sizeof(_float));
	m_pPointInstanceCom->Set_Variable("g_fPointSaturation_Power", &m_PointLight_Desc.fPointSaturation_Power, sizeof(_float));
	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Second->Get_ShaderResourceView(2));

	m_pPointInstanceCom->Render(10, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_PointLight::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_PointLight::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_PointLight::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_PointLight::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[iInstanceCount];

	_float2 vSize;
	vSize.x = m_PointLight_Desc.fPointRadius * 2.f;
	vSize.y = m_PointLight_Desc.fPointRadius * 2.f;	

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		m_pInstanceBuffer[iIndex].vSize = vSize;
		m_pInstanceBuffer[iIndex].vTextureUV = { 0.f, 0.f, 1.f, 1.f };
	}

	return S_OK;
}

CEffect_PointLight * CEffect_PointLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_PointLight*	pInstance = new CEffect_PointLight(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_PointLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_PointLight::Clone_GameObject(void * pArg)
{
	CEffect_PointLight* pInstance = new CEffect_PointLight(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_PointLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_PointLight::Free()
{
	__super::Free();
}
