#include "stdafx.h"
#include "..\Public\Space_Valve_Door.h"

CSpace_Valve_Door::CSpace_Valve_Door(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSpace_Valve_Door::CSpace_Valve_Door(const CSpace_Valve_Door & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpace_Valve_Door::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSpace_Valve_Door::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	Ready_Component(pArg);

	return S_OK;
}

_int CSpace_Valve_Door::Tick(_double TimeDelta)
{
	return _int();
}

_int CSpace_Valve_Door::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 100.f))
	{
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
	}

	return S_OK;
}

HRESULT CSpace_Valve_Door::Render(RENDER_GROUP::Enum eGroup)
{
	if (eGroup == RENDER_GROUP::RENDER_NONALPHA) return S_OK;

	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(18);

	return S_OK;
}

HRESULT CSpace_Valve_Door::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CSpace_Valve_Door::Set_Clear_Level(_bool IsClear_Level)
{
	m_IsClear_Level = IsClear_Level;

	Open_ValveDoor();
}

HRESULT CSpace_Valve_Door::Ready_Component(void * pArg)
{
	ARG_DESC Arg_Desc;
	memcpy(&Arg_Desc, pArg, sizeof(ARG_DESC));
	m_eDoorType = Arg_Desc.eDoorType;
	m_IsCodyValve = Arg_Desc.IsCodyDoor;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);

	_tchar pDoorType[MAX_PATH]	= TEXT("Component_Model_SpaceValve_Door_Right");
	_vector vPos		= XMLoadFloat4(&m_vOffsetPos_Door_R);
	if (Left_Door == m_eDoorType)
	{
		lstrcpy(pDoorType, TEXT("Component_Model_SpaceValve_Door_Left"));
		vPos = XMLoadFloat4(&m_vOffsetPos_Door_L);
	}
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, pDoorType, TEXT("Com_Model_Door"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	CStaticActor::ARG_DESC StaticDesc;
	m_UserData = USERDATA(GameID::eENVIRONMENT, this);
	StaticDesc.pUserData	= &m_UserData;
	StaticDesc.pModel		= m_pModelCom;
	StaticDesc.pTransform	= m_pTransformCom;
	CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static_Base"), (CComponent**)&m_pStaticActCom, &StaticDesc);


	return S_OK;
}

void CSpace_Valve_Door::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * WorldMatrix);
	m_pStaticActCom->Update_StaticActor();
}

void CSpace_Valve_Door::Open_ValveDoor()
{
	if (false == m_IsOpen)
	{
		_float fRotateAngle = 90.f;
		if (false == m_IsCodyValve)
			fRotateAngle += -180.f;

		if (Left_Door == m_eDoorType)
			fRotateAngle *= -1.f;

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		_matrix Matrix_Door = m_pTransformCom->Get_WorldMatrix();
		Matrix_Door = XMMatrixRotationY(XMConvertToRadians(fRotateAngle));
		Matrix_Door.r[3] = XMLoadFloat4(&vPos);
		m_pTransformCom->Set_WorldMatrix(Matrix_Door);
		m_pStaticActCom->Update_StaticActor();

// 			XMStoreFloat4(&vPos, m_pTransformCom_Door_L->Get_State(CTransform::STATE_POSITION));
// 			Matrix_Door = m_pTransformCom_Door_L->Get_WorldMatrix();
// 			Matrix_Door = XMMatrixRotationY(XMConvertToRadians(fRotateAngle));
// 			Matrix_Door.r[3] = XMLoadFloat4(&vPos);
// 			m_pTransformCom_Door_L->Set_WorldMatrix(Matrix_Door);
	}

	m_IsOpen = true;
}

CSpace_Valve_Door * CSpace_Valve_Door::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSpace_Valve_Door*	pInstance = new CSpace_Valve_Door(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSpace_Valve_Door");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSpace_Valve_Door::Clone_GameObject(void * pArg)
{
	CSpace_Valve_Door* pInstance = new CSpace_Valve_Door(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSpace_Valve_Door");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpace_Valve_Door::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActCom);

	__super::Free();
}
