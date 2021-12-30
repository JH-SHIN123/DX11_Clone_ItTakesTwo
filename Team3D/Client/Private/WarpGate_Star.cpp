#include "stdafx.h"
#include "..\Public\WarpGate_Star.h"
#include "Effect_Generator.h"

CWarpGate_Star::CWarpGate_Star(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWarpGate_Star::CWarpGate_Star(const CWarpGate_Star & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWarpGate_Star::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CWarpGate_Star::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_WarpGate_Star_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	return S_OK;
}

_int CWarpGate_Star::Tick(_double TimeDelta)
{
	return _int();
}

_int CWarpGate_Star::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		if(false == m_IsActivate)
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_PRE_CUSTOM_BLUR, this);
		else
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
	}

	return NO_EVENT;
}

HRESULT CWarpGate_Star::Render(RENDER_GROUP::Enum eGroup)
{
	_int iShaderPass = 27;
	if(false == m_IsActivate)
		iShaderPass = 18;

	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(iShaderPass);

	return S_OK;
}

void CWarpGate_Star::Set_WorldMatrix(_fmatrix WorldMatrix, _fvector vOffSetPos)
{
	_matrix ParentMatrix = WorldMatrix;
	for (_int i = 0; i < 3; ++i)
		ParentMatrix.r[i] = XMVector3Normalize(ParentMatrix.r[i]);

	ParentMatrix.r[3] = XMVector3Transform(vOffSetPos, ParentMatrix);

	m_pTransformCom->Set_WorldMatrix(ParentMatrix);
}

void CWarpGate_Star::Set_Activate(_bool IsActivate)
{
	if (false == m_IsActivate && true == IsActivate)
	{		
		EFFECT->Add_Effect(Effect_Value::WarpGate_Clear, m_pTransformCom->Get_WorldMatrix());
		m_IsActivate = true;
	}
}

void CWarpGate_Star::Activate_Check(_double TimeDelta)
{
}

CWarpGate_Star * CWarpGate_Star::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWarpGate_Star* pInstance = new CWarpGate_Star(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CWarpGate_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWarpGate_Star::Clone_GameObject(void * pArg)
{
	CWarpGate_Star* pInstance = new CWarpGate_Star(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CWarpGate_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWarpGate_Star::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);

	__super::Free();
}
