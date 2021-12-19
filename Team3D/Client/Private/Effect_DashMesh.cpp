#include "stdafx.h"
#include "..\Public\Effect_DashMesh.h"

CEffect_DashMesh::CEffect_DashMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_DashMesh::CEffect_DashMesh(const CEffect_DashMesh & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_DashMesh::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_DashMesh::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Zoom"), TEXT("Com_Texture_Color"), (CComponent**)&m_pTexturesCom), E_FAIL);

	//m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-90.f));


	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.75f;
	vPos -= m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 1.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (EFFECT_DESC_CLONE::PV_CODY_S != m_EffectDesc_Clone.iPlayerValue)
		m_pTransformCom->Set_Scale(XMVectorSet(1.5f, 1.f, 1.5f, 0.f));
	else
		m_pTransformCom->Set_Scale(XMVectorSet(0.15f, 0.1f, 0.15f, 0.f));

	return S_OK;
}

_int CEffect_DashMesh::Tick(_double TimeDelta)
{
	if (0.f > m_fAlphaTime)
		return EVENT_DEAD;

	m_fAlphaTime -= (_float)TimeDelta * 0.75f;

	if (0.f > m_fAlphaTime)
		m_fAlphaTime = 0.f;

	return _int();
}

_int CEffect_DashMesh::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CEffect_DashMesh::Render(RENDER_GROUP::Enum eGroup)
{
	m_pModelCom->Set_Variable("g_fTime", &m_fAlphaTime, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture_Second", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(15);

	return S_OK;
}

void CEffect_DashMesh::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

void CEffect_DashMesh::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

HRESULT CEffect_DashMesh::Ready_Instance()
{
	return S_OK;
}

CEffect_DashMesh * CEffect_DashMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_DashMesh*	pInstance = new CEffect_DashMesh(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_DashMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_DashMesh::Clone_GameObject(void * pArg)
{
	CEffect_DashMesh* pInstance = new CEffect_DashMesh(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_DashMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_DashMesh::Free()
{
	__super::Free();
}
