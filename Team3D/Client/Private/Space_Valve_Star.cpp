#include "stdafx.h"
#include "..\Public\Space_Valve_Star.h"
#include "DataStorage.h"
#include "Space_Valve_Door.h"

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
	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Ready_InstanceModel_Matrix(), E_FAIL);

	m_iRotate_Count = 0;

	for (_int i = 0; i < 6; ++i)
	{
		m_fRotateAngle[i] = 0.f;
		m_fRotateAngle_Powwer[i] = 0.f;

		if (i % 2 == 1)
			m_fRotateAngle[i] = -0.75f;
	}

	return S_OK;
}

_int CSpace_Valve_Star::Tick(_double TimeDelta)
{
	Rotate_Check(TimeDelta);

	if (false == m_IsValve_Max)
		m_iRotate_Count = DATABASE->Get_ValveCount_Cody(m_IsCodyValve);
	else
		m_iRotate_Count = 0;

	return _int();
}

_int CSpace_Valve_Star::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelInstanceCom_Star->Culling())
	{
		if (0 < m_pModelCom_Base->Culling(m_pTransformCom_Base->Get_State(CTransform::STATE_POSITION), 10.f))
			m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	}

	return S_OK;
}

HRESULT CSpace_Valve_Star::Render(RENDER_GROUP::Enum eGroup)
{
	NULL_CHECK_RETURN(m_pModelCom_Base, E_FAIL);
	m_pModelCom_Base->Set_DefaultVariables_Perspective(m_pTransformCom_Base->Get_WorldMatrix());
	m_pModelCom_Base->Set_DefaultVariables_Shadow();
	m_pModelCom_Base->Render_Model(1, 0);

	m_pModelInstanceCom_Star->Set_DefaultVariables_Perspective();
	m_pModelInstanceCom_Star->Set_DefaultVariables_Shadow();
	m_pModelInstanceCom_Star->Render_Model(0);

	return S_OK;
}

HRESULT CSpace_Valve_Star::Render_ShadowDepth()
{
	return S_OK;
}

void CSpace_Valve_Star::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
	_matrix ParentMatrix = WorldMatrix;

	for (_int i = 0; i < 3; ++i)
		ParentMatrix.r[i] = XMVector3Normalize(ParentMatrix.r[i]);

	m_pSpace_Valve_Door_Right->SetUp_WorldMatrix(ParentMatrix);
	m_pSpace_Valve_Door_Left->SetUp_WorldMatrix(ParentMatrix);


	_matrix Matrix_ValveBase = m_pTransformCom_Base->Get_WorldMatrix() * ParentMatrix;
	m_pTransformCom_Base->Set_WorldMatrix(Matrix_ValveBase);
}

#pragma region Star_Rotate
void CSpace_Valve_Star::Rotate_Check(_double TimeDelta)
{
	if (false == m_IsClear_Level)
		return;

	if (0 == m_iRotate_Count)
		return;

	if (true == m_IsValve_Max)
		return;

	for (_int i = 0; i < 6; ++i)
	{
		if (360.f <= m_fRotateAngle[i])
			m_fRotateAngle[i] = 0.f;

		if (1.5f <= m_fRotateAngle_Powwer[i])
			m_fRotateAngle_Powwer[i] = 1.5f;
	}

	_float fRotatePower_Basic = 1.f;
	_float fRotateAnglePower_Power = 0.5f;

	if (1 <= m_iRotate_Count)
	{
		m_fRotateAngle[0] += (_float)TimeDelta * fRotatePower_Basic *m_fRotateAngle_Powwer[0];
		m_fRotateAngle_Powwer[0] += (_float)TimeDelta * fRotateAnglePower_Power;
		m_fRotateAngle[1] += (_float)TimeDelta * m_fRotateAngle_Powwer[1];
		m_fRotateAngle_Powwer[1] += (_float)TimeDelta * fRotateAnglePower_Power;

		Rotate_Matrix(false, m_fRotateAngle[0], 0);
		Rotate_Matrix(true, m_fRotateAngle[0], 1);

		if (0.f <= m_fRotateAngle[1])
		{
			Rotate_Matrix(false, m_fRotateAngle[1], 2);
			Rotate_Matrix(true, m_fRotateAngle[1], 3);
		}
	}

	if (2 <= m_iRotate_Count)
	{
		m_fRotateAngle[2] += (_float)TimeDelta * fRotatePower_Basic *m_fRotateAngle_Powwer[2];
		m_fRotateAngle_Powwer[2] += (_float)TimeDelta * fRotateAnglePower_Power;
		m_fRotateAngle[3] += (_float)TimeDelta * m_fRotateAngle_Powwer[3];
		m_fRotateAngle_Powwer[3] += (_float)TimeDelta * fRotateAnglePower_Power;

		Rotate_Matrix(false, m_fRotateAngle[2], 4);
		Rotate_Matrix(true, m_fRotateAngle[2], 5);

		if (0.f <= m_fRotateAngle[3])
		{
			Rotate_Matrix(false, m_fRotateAngle[3], 6);
			Rotate_Matrix(true, m_fRotateAngle[3], 7);
		}
	}

	if (3 <= m_iRotate_Count)
	{
		m_fRotateAngle[4] += (_float)TimeDelta * fRotatePower_Basic *m_fRotateAngle_Powwer[4];
		m_fRotateAngle_Powwer[4] += (_float)TimeDelta * fRotateAnglePower_Power;
		m_fRotateAngle[5] += (_float)TimeDelta * m_fRotateAngle_Powwer[5];
		m_fRotateAngle_Powwer[5] += (_float)TimeDelta * fRotateAnglePower_Power;

		Rotate_Matrix(false, m_fRotateAngle[4], 8);
		Rotate_Matrix(true, m_fRotateAngle[4], 9);

		if (0.f <= m_fRotateAngle[3])
		{
			Rotate_Matrix(false, m_fRotateAngle[5], 10);
		}
	}
 
 	if (4 <= m_iRotate_Count)
 	{
		for (_int i = 0; i < 6; ++i)
		{
			m_fRotateAngle_Powwer[i] -= (_float)TimeDelta * fRotateAnglePower_Power * 1.25f;

			if (0.f >= m_fRotateAngle_Powwer[i])
				m_fRotateAngle_Powwer[i] = 0.f;
		}

		if(false == m_IsValve_Max)
			m_fReset_Time += (_float)TimeDelta;

		if (5.f <= m_fReset_Time && false == m_IsValve_Max)
		{
			DATABASE->Reset_ValveCount_Cody(m_IsCodyValve);
			m_IsValve_Max = true;
		}
 	}

}

void CSpace_Valve_Star::SetUp_Instance_World_Pos()
{
	_matrix WorldMatrix = m_pTransformCom_Base->Get_WorldMatrix();
	for (_int iIndex = 0; iIndex < m_iModelInstance_Count; ++iIndex)
		XMStoreFloat4x4(&m_pWorldMatrix_Star[iIndex], WorldMatrix);

	_vector vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * 2.78f) + (WorldMatrix.r[1] * 1.f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[0].m[3][0], &vPos, sizeof(_float4));

	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * 2.58f) + (WorldMatrix.r[1] * 1.8f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[2].m[3][0], &vPos, sizeof(_float4));

	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * 2.17f) + (WorldMatrix.r[1] * 2.55f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[4].m[3][0], &vPos, sizeof(_float4));

	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * 1.6f) + (WorldMatrix.r[1] * 3.15f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[6].m[3][0], &vPos, sizeof(_float4));

	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * 0.85f) + (WorldMatrix.r[1] * 3.51f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[8].m[3][0], &vPos, sizeof(_float4));

	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * 0.f) + (WorldMatrix.r[1] * 3.65f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[10].m[3][0], &vPos, sizeof(_float4));

	// Right
	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * -2.78f) + (WorldMatrix.r[1] * 1.f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[1].m[3][0], &vPos, sizeof(_float4));

	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * -2.58f) + (WorldMatrix.r[1] * 1.8f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[3].m[3][0], &vPos, sizeof(_float4));

	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * -2.17f) + (WorldMatrix.r[1] * 2.55f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[5].m[3][0], &vPos, sizeof(_float4));

	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * -1.6f) + (WorldMatrix.r[1] * 3.15f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[7].m[3][0], &vPos, sizeof(_float4));

	vPos = WorldMatrix.r[3] + (WorldMatrix.r[2] * -0.85f) + (WorldMatrix.r[1] * 3.51f) + (WorldMatrix.r[0] * 0.3f);
	memcpy(&m_pWorldMatrix_Star[9].m[3][0], &vPos, sizeof(_float4));

	// 0.695
}
#pragma endregion
void CSpace_Valve_Star::Set_Clear_Level(_bool IsClear_Level)
{
	m_IsClear_Level = IsClear_Level;

	m_pSpace_Valve_Door_Right->Set_Clear_Level(m_IsClear_Level);
	m_pSpace_Valve_Door_Left->Set_Clear_Level(m_IsClear_Level);
}

void CSpace_Valve_Star::Add_Rotate_Count(_int iCount)
{
	m_iRotate_Count += iCount;
}

HRESULT CSpace_Valve_Star::Ready_Component(void* pArg)
{
	tagValve_Star_Desc Data;
	memcpy(&Data, pArg, sizeof(tagValve_Star_Desc));

	m_IsCodyValve = Data.IsCody;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom_Base), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"),	TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE,	TEXT("Component_Model_SpaceValve_Base"), TEXT("Com_Model_Base"), (CComponent**)&m_pModelCom_Base), E_FAIL);

	m_pTransformCom_Base->Set_State(CTransform::STATE_POSITION,	XMLoadFloat4(&m_vOffsetPos_Base));

	CSpace_Valve_Door::EDoor_Type eDoorType = CSpace_Valve_Door::Right_Door;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_Valve_Door"), Level::LEVEL_STATIC, TEXT("GameObject_Space_Valve_Door"), &eDoorType, (CGameObject**)&m_pSpace_Valve_Door_Right), E_FAIL);
	eDoorType = CSpace_Valve_Door::Left_Door;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_Valve_Door"), Level::LEVEL_STATIC, TEXT("GameObject_Space_Valve_Door"), &eDoorType, (CGameObject**)&m_pSpace_Valve_Door_Left), E_FAIL);

	SetUp_WorldMatrix(XMLoadFloat4x4(&Data.WorldMatrix));

	CStaticActor::ARG_DESC StaticDesc;
	m_UserData = USERDATA(GameID::eENVIRONMENT, this);
	StaticDesc.pUserData  = &m_UserData;
	StaticDesc.pModel	  = m_pModelCom_Base;
	StaticDesc.pTransform = m_pTransformCom_Base;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static_Base"), (CComponent**)&m_pStaticActCom_Bass, &StaticDesc), E_FAIL);

	return S_OK;
}

HRESULT CSpace_Valve_Star::Ready_InstanceModel_Matrix()
{
	m_pWorldMatrix_Star = new _float4x4[m_iModelInstance_Count];

	SetUp_Instance_World_Pos();

	CModel_Instance::ARG_DESC Arg;
	Arg.fCullingRadius = 3.f;
	Arg.iInstanceCount = m_iModelInstance_Count;
	Arg.pWorldMatrices = m_pWorldMatrix_Star;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Generator_Star_Valve"), TEXT("Com_Model_Generator_Star_Valve"), (CComponent**)&m_pModelInstanceCom_Star, &Arg), E_FAIL);

	return S_OK;
}

void CSpace_Valve_Star::Rotate_Matrix(_bool IsReverseRotate, _float fAngle, _int iIndex)
{
	if (true == IsReverseRotate)
		fAngle *= -1.f;

	_float fRadian = XMConvertToRadians(fAngle);

	_matrix StarWorld = XMLoadFloat4x4(&m_pWorldMatrix_Star[iIndex]);
	_vector vRotateAxis = StarWorld.r[0];
	_vector vPos = StarWorld.r[3];

	StarWorld *= XMMatrixRotationAxis(vRotateAxis, fRadian);
	StarWorld.r[3] = vPos;

	XMStoreFloat4x4(&m_pWorldMatrix_Star[iIndex], StarWorld);
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
	Safe_Release(m_pModelCom_Base);
	Safe_Release(m_pTransformCom_Base);
	Safe_Release(m_pStaticActCom_Bass);
	Safe_Release(m_pSpace_Valve_Door_Right);
	Safe_Release(m_pSpace_Valve_Door_Left);
	Safe_Release(m_pModelInstanceCom_Star);

	__super::Free();
}
