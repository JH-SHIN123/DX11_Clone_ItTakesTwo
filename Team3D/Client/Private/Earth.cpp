#include "stdafx.h"
#include "..\Public\Earth.h"

CEarth::CEarth(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEarth::CEarth(const CEarth & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEarth::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CEarth::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Earth"), TEXT("Com_Textures"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Earth"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -40000.f, 0.f, 1.f));

	return S_OK;
}

_int CEarth::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	m_dTimeUV += TimeDelta * 0.001;

	if (1.0 < m_dTimeUV)
		m_dTimeUV -= 1.0;

	return NO_EVENT;
}

_int CEarth::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 50000.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CEarth::Render(RENDER_GROUP::Enum eRender)
{
	CGameObject::Render(eRender);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	CPipeline* pPipeline = CPipeline::GetInstance();
	NULL_CHECK_RETURN(pPipeline, E_FAIL);

	_float fFar = 50000.f;
	_float fTimeUV = (_float)m_dTimeUV;
	_int iGara = 0;

	m_pModelCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ_FOR_EARTH)), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ_FOR_EARTH)), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_fMainCamFar", &fFar, sizeof(_float));
	m_pModelCom->Set_Variable("g_fSubCamFar", &fFar, sizeof(_float));
	m_pModelCom->Set_Variable("g_fTimeUV", &fTimeUV, sizeof(_float));

	m_pModelCom->Sepd_Bind_Buffer();

	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pModelCom->Set_ShaderResourceView("g_DarkTexture", m_pTexturesCom->Get_ShaderResourceView(2));
	m_pModelCom->Set_ShaderResourceView("g_RoughnessTexture", m_pTexturesCom->Get_ShaderResourceView(3));
	m_pModelCom->Sepd_Render_Model(0, 0);

	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(1));
	m_pModelCom->Sepd_Render_Model(1, 1);

	return S_OK;
}

HRESULT CEarth::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	return S_OK;
}

CEarth * CEarth::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEarth* pInstance = new CEarth(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CEarth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEarth::Clone_GameObject(void * pArg)
{
	CEarth* pInstance = new CEarth(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEarth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEarth::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTexturesCom);

	CGameObject::Free();
}
