#include "stdafx.h"
#include "Effect_Dash.h"
#include "GameInstance.h"

CEffect_Dash::CEffect_Dash(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Rect(pDevice, pDeviceContext)
{
}

CEffect_Dash::CEffect_Dash(const CEffect_Dash & rhs)
	: CInGameEffect_Rect(rhs)
{
}

HRESULT CEffect_Dash::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.fLifeTime = 3;

	return S_OK;
}

HRESULT CEffect_Dash::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Textrue_Color"), (CComponent**)&m_pTexturesCom_Color), E_FAIL);

	_vector vRight = XMLoadFloat4(&m_pInstanceBuffer[0].vRight);
	_vector vUp = XMLoadFloat4(&m_pInstanceBuffer[0].vUp);
	_vector vLook = XMLoadFloat4(&m_pInstanceBuffer[0].vLook);

	_float3 vScale = { XMVector3Length(vRight).m128_f32[0], XMVector3Length(vUp).m128_f32[0], XMVector3Length(vLook).m128_f32[0] };

	XMStoreFloat4(&m_pInstanceBuffer[0].vRight, (XMVector3Normalize(vRight) * vScale.x));
	XMStoreFloat4(&m_pInstanceBuffer[0].vUp, (XMVector3Normalize(vUp) * vScale.y));
	XMStoreFloat4(&m_pInstanceBuffer[0].vLook, (XMVector3Normalize(vLook) * vScale.z));

	return S_OK;
}

_int CEffect_Dash::Tick(_double TimeDelta)
{
	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;
	m_dAlphaTime -= TimeDelta;

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
		Check_Scale(TimeDelta, iIndex);


	return _int();
}

_int CEffect_Dash::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Dash::Render()
{
	SetUp_Shader_Data();

	CPipeline* pPipeline = CPipeline::GetInstance();
	if (nullptr == pPipeline)
		return S_OK;

	_float fTime = (_float)m_dAlphaTime;
	m_pRectInstanceCom->Set_Variable("g_fTime", &fTime, sizeof(_float));

	m_pRectInstanceCom->Set_ShaderResourceView("g_SecondTexture",	m_pTexturesCom->Get_ShaderResourceView(m_EffectDesc_Prototype.iTextureNum));
	m_pRectInstanceCom->Set_ShaderResourceView("g_DiffuseTexture",	m_pTexturesCom_Second->Get_ShaderResourceView(m_EffectDesc_Prototype.iTextureNum_Second));
	m_pRectInstanceCom->Set_ShaderResourceView("g_ColorTexture",	m_pTexturesCom_Color->Get_ShaderResourceView(9));

	m_pRectInstanceCom->Render(0, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Dash::Check_Scale(_double TimeDelta, _uint iIndex)
{
	_vector vRight	= XMLoadFloat4(&m_pInstanceBuffer[iIndex].vRight);
	_vector vUp		= XMLoadFloat4(&m_pInstanceBuffer[iIndex].vUp);
	_vector vLook	= XMLoadFloat4(&m_pInstanceBuffer[iIndex].vLook);

	_float3 vScale = { XMVector3Length(vRight).m128_f32[0], XMVector3Length(vUp).m128_f32[0], XMVector3Length(vLook).m128_f32[0] };

	//vScale.x -= (_float)TimeDelta * 0.5f;
	vScale.z += (_float)TimeDelta * 0.8f;

	if (0.f >= vScale.x)
		vScale.x = 0.1f;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vRight, (XMVector3Normalize(vRight) * vScale.x));
	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vUp, (XMVector3Normalize(vUp) * vScale.y));
	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vLook, (XMVector3Normalize(vLook) * vScale.z));

}

CEffect_Dash * CEffect_Dash::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CEffect_Dash*	pInstance = new CEffect_Dash(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Dash");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Dash::Clone_GameObject(void * pArg)
{
	CEffect_Dash* pInstance = new CEffect_Dash(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Dash");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Dash::Free()
{
	Safe_Release(m_pTexturesCom_Color);
	__super::Free();
}
