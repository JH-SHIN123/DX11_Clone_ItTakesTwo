#include "stdafx.h"
#include "Effect_Dash.h"

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
	__super::NativeConstruct_Prototype(pArg); //

	m_EffectDesc_Prototype.vSize = { 1.5f,1.5f,1.5f };

	return S_OK;
}

HRESULT CEffect_Dash::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_ShockWave"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Rect_TripleUV"), TEXT("Com_VIBuffer"), (CComponent**)&m_pRectCom), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);

	WolrdMatrix.r[3] += WolrdMatrix.r[1] * 0.8f;

	for (_int i = 0; i < 3; ++i)
		WolrdMatrix.r[i] = XMVector3Normalize(WolrdMatrix.r[i]);

	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);
	if (EFFECT_DESC_CLONE::PV_CODY_S != m_EffectDesc_Clone.iPlayerValue)
		m_pTransformCom->Set_Scale(XMVectorSet(1.5f, 1.5f, 1.5f, 0.f));
	else
		m_pTransformCom->Set_Scale(XMVectorSet(0.15f, 0.15f, 0.15f, 0.f));


	m_dAngle = (_double)(rand() % 180);
	if (rand() % 2 == 0) m_dAngle *= 2.f;

	m_pTransformCom->RotatePitch_Angle(90.0);
	
	m_pTransformCom->RotateYaw_Angle(m_dAngle);


	return S_OK;
}

_int CEffect_Dash::Tick(_double TimeDelta)
{
	if (0.f >= m_fAlphaTime)
		return EVENT_DEAD;

	m_fAlphaTime -= (_float)TimeDelta * 0.65f;
	if (0.f > m_fAlphaTime)	m_fAlphaTime = 0.f;

	m_dAngle += TimeDelta * 100.0;
	if (360.0 < m_dAngle) m_dAngle = 0.0;

	Check_Move(TimeDelta);
	Check_Scale(TimeDelta);

	return _int();
}

_int CEffect_Dash::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_Dash::Render(RENDER_GROUP::Enum eGroup)
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	if (nullptr == pPipeline)
		return E_FAIL;

	m_pRectCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pRectCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINVIEW)), sizeof(_matrix));
	m_pRectCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ)), sizeof(_matrix));
	m_pRectCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBVIEW)), sizeof(_matrix));
	m_pRectCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ)), sizeof(_matrix));

	_float	fMainCamFar = pPipeline->Get_MainCamFar();
	_float	fSubCamFar = pPipeline->Get_SubCamFar();
	_vector vMainCamPosition = pPipeline->Get_MainCamPosition();
	_vector vSubCamPosition = pPipeline->Get_SubCamPosition();

	m_pRectCom->Set_Variable("g_fMainCamFar", &fMainCamFar, sizeof(_float));
	m_pRectCom->Set_Variable("g_fSubCamFar", &fSubCamFar, sizeof(_float));
	m_pRectCom->Set_Variable("g_vMainCamPosition", &vMainCamPosition, sizeof(_vector));
	m_pRectCom->Set_Variable("g_vSubCamPosition", &vSubCamPosition, sizeof(_vector));

	_float fRadian = XMConvertToRadians((_float)m_dAngle);
	m_pRectCom->Set_Variable("g_fTime", &m_fAlphaTime, sizeof(_float));
	m_pRectCom->Set_Variable("g_fRadianAngle", &fRadian, sizeof(_float));

	m_pRectCom->Set_ShaderResourceView("g_DiffuseTexture",	m_pTexturesCom->Get_ShaderResourceView(1));

	m_pRectCom->Render(0);

	return S_OK;
}

void CEffect_Dash::Check_Scale(_double TimeDelta)
{
	_float fScale_X = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fScale_Y = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
	_float fScale_Z = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

	_float fScalePower = 0.f;
	if (EFFECT_DESC_CLONE::PV_CODY_S != m_EffectDesc_Clone.iPlayerValue)
		fScalePower = 2.5f;
	else
		fScalePower = 0.25f;

	fScale_X += (_float)TimeDelta * fScalePower;
	fScale_Y += (_float)TimeDelta * fScalePower;
	fScale_Z += (_float)TimeDelta * fScalePower;

	m_pTransformCom->Set_Scale(XMVectorSet(fScale_X, fScale_Y, fScale_Z, 0.f));
}

void CEffect_Dash::Check_Move(_double TimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_UP);

	if (EFFECT_DESC_CLONE::PV_CODY_S != m_EffectDesc_Clone.iPlayerValue)
		vPos -= vDir * (_float)TimeDelta * 4.f * m_fAlphaTime;
	else
		vPos -= vDir * (_float)TimeDelta * 0.4f * m_fAlphaTime;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
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
	__super::Free();
}
