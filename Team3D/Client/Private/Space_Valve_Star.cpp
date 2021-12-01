#include "stdafx.h"
#include "..\Public\Space_Valve_Star.h"

CSpace_Valve_Star::CSpace_Valve_Star(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSpace_Valve_Star::CSpace_Valve_Star(const CSpace_Valve_Star & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpace_Valve_Star::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CSpace_Valve_Star::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_InstanceModel_Matrix(), E_FAIL);

	return S_OK;
}

_int CSpace_Valve_Star::Tick(_double TimeDelta)
{
// 	_vector vPos = XMVectorSet(64.f, 0.f, 35.f, 1.f);
// 	vPos += XMVectorSet(0.2f, -1.7f, 0.f, 0.f);
// 	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return _int();
}

_int CSpace_Valve_Star::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom_Door_R->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
	{
		if (0 < m_pModelCom_Door_L->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		{
			if (0 < m_pModelCom_Base->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
		}
	}

	return S_OK;
}

HRESULT CSpace_Valve_Star::Render(RENDER_GROUP::Enum eGroup)
{
	NULL_CHECK_RETURN(m_pModelCom_Base, E_FAIL);
	m_pModelCom_Base->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom_Base->Set_DefaultVariables_Shadow();
	m_pModelCom_Base->Render_Model(1, 0);

// 	_matrix DoorMatrix = m_pTransformCom->Get_WorldMatrix();
// 	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
// 	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
// 	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
// 	DoorMatrix.r[3] += (vLook * 3.1f) + (vRight * 0.5f);

	NULL_CHECK_RETURN(m_pModelCom_Door_R, E_FAIL);
	m_pModelCom_Door_R->Set_DefaultVariables_Perspective(XMLoadFloat4x4(&m_MatrixValve_Door[Right]));
	m_pModelCom_Door_R->Set_DefaultVariables_Shadow();
	m_pModelCom_Door_R->Render_Model(4, 0);

	//_matrix RotateMatrix = XMMatrixRota

// 	DoorMatrix = m_pTransformCom->Get_WorldMatrix();
// 	DoorMatrix.r[3] += (vLook * -3.1f) + (vRight * 0.5f);

	NULL_CHECK_RETURN(m_pModelCom_Door_L, E_FAIL);
	m_pModelCom_Door_L->Set_DefaultVariables_Perspective(XMLoadFloat4x4(&m_MatrixValve_Door[Left]));
	m_pModelCom_Door_L->Set_DefaultVariables_Shadow();
	m_pModelCom_Door_L->Render_Model(4, 0);

	return S_OK;
}

HRESULT CSpace_Valve_Star::Render_ShadowDepth()
{
	return S_OK;
}

void CSpace_Valve_Star::Set_WorldMatrix(_fmatrix WorldMatrix)
{
	_matrix ParentMatrix = WorldMatrix;

	for (_int i = 0; i < 3; ++i)
		ParentMatrix.r[i] = XMVector3Normalize(ParentMatrix.r[i]);

	XMStoreFloat4x4(&m_MatrixValve_Door[Right], XMLoadFloat4x4(&m_MatrixValve_Door[Right]) * ParentMatrix);
	XMStoreFloat4x4(&m_MatrixValve_Door[Left], XMLoadFloat4x4(&m_MatrixValve_Door[Left]) * ParentMatrix);

	_matrix Matrix_ValveBase = m_pTransformCom->Get_WorldMatrix() * ParentMatrix;
	m_pTransformCom->Set_WorldMatrix(Matrix_ValveBase);
}

void CSpace_Valve_Star::Set_Clear_Level(_bool IsClear_Level)
{
	m_IsClear_Level = IsClear_Level;
}

void CSpace_Valve_Star::Add_Rotate_Count(_int iCount)
{
	m_iRotate_Count = iCount;
}

HRESULT CSpace_Valve_Star::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve_Base"), TEXT("Com_Model_Base"), (CComponent**)&m_pModelCom_Base), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve_Door_Right"), TEXT("Com_Model_Door_R"), (CComponent**)&m_pModelCom_Door_R), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_SpaceValve_Door_Left"), TEXT("Com_Model_Door_L"), (CComponent**)&m_pModelCom_Door_L), E_FAIL);

//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Generator_Star"), TEXT("Com_Model_Star"), (CComponent**)&m_pModelInstanceCom), E_FAIL);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.2f, -1.7f, 0.f, 1.f));

	m_MatrixValve_Door[Right] = MH_XMFloat4x4Identity();
	m_MatrixValve_Door[Left]  = MH_XMFloat4x4Identity();

	memcpy(&m_MatrixValve_Door[Right].m[3][0], &m_vOffsetPos_Door_R, sizeof(_float3));
	memcpy(&m_MatrixValve_Door[Left].m[3][0],  &m_vOffsetPos_Door_L, sizeof(_float3));

	return S_OK;
}

HRESULT CSpace_Valve_Star::Ready_InstanceModel_Matrix()
{


	return S_OK;
}

void CSpace_Valve_Star::Open_ValveDoor()
{

}

CSpace_Valve_Star * CSpace_Valve_Star::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSpace_Valve_Star*	pInstance = new CSpace_Valve_Star(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSpace_Valve_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSpace_Valve_Star::Clone_GameObject(void * pArg)
{
	CSpace_Valve_Star* pInstance = new CSpace_Valve_Star(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSpace_Valve_Star");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpace_Valve_Star::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom_Base);
	Safe_Release(m_pModelCom_Door_R);
	Safe_Release(m_pModelCom_Door_L);
	Safe_Release(m_pModelInstanceCom);

	Safe_Delete_Array(m_pWorldMatrix_Star);

	__super::Free();
}
