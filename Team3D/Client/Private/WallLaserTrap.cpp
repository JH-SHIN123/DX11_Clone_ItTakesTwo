#include "stdafx.h"
#include "..\Public\WallLaserTrap.h"
#include "May.h"
#include "Cody.h"


CWallLaserTrap::CWallLaserTrap(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWallLaserTrap::CWallLaserTrap(const CWallLaserTrap & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWallLaserTrap::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CWallLaserTrap::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	Ready_Component(pArg);
	Ready_PointInstance();

	return S_OK;
}

_int CWallLaserTrap::Tick(_double TimeDelta)
{
// 
// 	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
// 	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
// 	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
// 	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
// 
// 	XMStoreFloat4(&m_pPointInstanceBuffer[0].vPosition, vPos - (vRight * m_vHalfSize.x) + (vUp * 0.01f));
// 	XMStoreFloat4(&m_pPointInstanceBuffer[1].vPosition, vPos + (vLook  * m_vHalfSize.y) + (vUp * 0.01f));
// 	XMStoreFloat4(&m_pPointInstanceBuffer[2].vPosition, vPos + (vRight * m_vHalfSize.x) + (vUp * 0.01f));
// 	XMStoreFloat4(&m_pPointInstanceBuffer[3].vPosition, vPos - (vLook  * m_vHalfSize.y) + (vUp * 0.01f));
// 
// 	XMStoreFloat4(&m_pPointInstanceBuffer[0].vTextureUV, vLook);
// 	XMStoreFloat4(&m_pPointInstanceBuffer[1].vTextureUV, vRight);
// 	XMStoreFloat4(&m_pPointInstanceBuffer[2].vTextureUV, vLook);
// 	XMStoreFloat4(&m_pPointInstanceBuffer[3].vTextureUV, vRight);

	Check_Activate(TimeDelta);

	return _int();
}

_int CWallLaserTrap::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelInstanceCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CWallLaserTrap::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelInstanceCom, E_FAIL);
// 	m_pModelInstanceCom->Set_DefaultVariables_Shadow();

	for (_int i = 0; i < 4; ++i)
	{
		m_pModelInstanceCom->Set_DefaultVariables_Perspective(Get_Matrix(m_vPosition_Lamp[i]));
		m_pModelInstanceCom->Render_Model(1, 0);
	}

	if(true == m_IsActivate)
		Render_PointInstance();

	return S_OK;
}

void CWallLaserTrap::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (false == m_IsActivate)
		return;

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eWALLLASERTRAP, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eWALLLASERTRAP, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eWALLLASERTRAP, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	//	((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eWALLLASERTRAP, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CWallLaserTrap::Set_TriggerActivate(_bool IsActivate)
{
	m_IsActivate = IsActivate;
}

HRESULT CWallLaserTrap::Render_ShadowDepth()
{
// 	NULL_CHECK_RETURN(m_pModelInstanceCom, E_FAIL);
// 	m_pModelInstanceCom->Set_DefaultVariables_ShadowDepth();
// 	// Skinned: 2 / Normal: 3
// 	m_pModelInstanceCom->Render_Model(3, 0, true);
 	return S_OK;
}

void CWallLaserTrap::Set_Transform(_vector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CWallLaserTrap::Set_Rotate(_float3 vAngle)
{
	_matrix vPivot = XMMatrixIdentity();

	vPivot *= XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(vAngle.x),
		XMConvertToRadians(vAngle.y),
		XMConvertToRadians(vAngle.z));

	vPivot.r[0] *= m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	vPivot.r[1] *= m_pTransformCom->Get_Scale(CTransform::STATE_UP);
	vPivot.r[2] *= m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);
	vPivot.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_WorldMatrix(vPivot);
}

HRESULT CWallLaserTrap::Ready_Component(void * pArg)
{
	tagWallLaserTrap_Desc Data;
	memcpy(&Data, pArg, sizeof(tagWallLaserTrap_Desc));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Saucer_Lamp_02"), TEXT("Com_Model"), (CComponent**)&m_pModelInstanceCom), E_FAIL);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform_Trigger"), (CComponent**)&m_pTransformCom_Trigger), E_FAIL);

	Calcul_Transform(Data);

	m_pTransformCom_Trigger->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.2f);
	m_pTransformCom_Trigger->Set_State(CTransform::STATE_POSITION, vPos);

	m_UserData = USERDATA(GameID::eWALLLASERTRAP, this);
	CTriggerActor::ARG_DESC ArgDesc;
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom_Trigger;
	ArgDesc.pGeometry = new PxBoxGeometry(1.5f, 0.05f, 5.1f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_PointInstance"), (CComponent**)&m_pPointInstanceCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Ember_Texture"), TEXT("Com_Textures_Mask"), (CComponent**)&m_pTexturesCom_Mask), E_FAIL);


	m_vPosition_Lamp[0] = { -m_vHalfSize.x,  0.f,  m_vHalfSize.y,  1.f };
	m_vPosition_Lamp[1] = { m_vHalfSize.x,  0.f,  m_vHalfSize.y,  1.f };
	m_vPosition_Lamp[2] = { m_vHalfSize.x, -0.f, -m_vHalfSize.y,  1.f };
	m_vPosition_Lamp[3] = { -m_vHalfSize.x, -0.f, -m_vHalfSize.y,  1.f };

	return S_OK;
}

HRESULT CWallLaserTrap::Ready_PointInstance()
{
	// 올해의 개망나니 코드블록 어워드 수상 유력 후보
	m_pPointInstanceBuffer = new VTXMATRIX_CUSTOM_ST[m_iPointInstance_Count];

	_vector vPos	= m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vRight	= m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vUp		= m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vLook	= m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	XMStoreFloat4(&m_pPointInstanceBuffer[0].vPosition, vPos - (vRight * m_vHalfSize.x) + (vUp * 0.01f));
	XMStoreFloat4(&m_pPointInstanceBuffer[1].vPosition, vPos + (vLook  * m_vHalfSize.y) + (vUp * 0.01f));
	XMStoreFloat4(&m_pPointInstanceBuffer[2].vPosition, vPos + (vRight * m_vHalfSize.x) + (vUp * 0.01f));
	XMStoreFloat4(&m_pPointInstanceBuffer[3].vPosition, vPos - (vLook  * m_vHalfSize.y) + (vUp * 0.01f));

	XMStoreFloat4(&m_pPointInstanceBuffer[0].vTextureUV, vLook);
	XMStoreFloat4(&m_pPointInstanceBuffer[1].vTextureUV, vRight);
	XMStoreFloat4(&m_pPointInstanceBuffer[2].vTextureUV, vLook);
	XMStoreFloat4(&m_pPointInstanceBuffer[3].vTextureUV, vRight);

	m_pPointInstanceBuffer[0].vSize = { 0.5f, m_vHalfSize.y * 2.f };
	m_pPointInstanceBuffer[1].vSize = { 0.5f, m_vHalfSize.x * 2.f };
	m_pPointInstanceBuffer[2].vSize = { 0.5f, m_vHalfSize.y * 2.f };
	m_pPointInstanceBuffer[3].vSize = { 0.5f, m_vHalfSize.x * 2.f };

	for (_int iIndex = 0; iIndex < m_iPointInstance_Count; ++iIndex)
	{
		m_pPointInstanceBuffer[iIndex].vRight	= { 1.f, 0.f, 0.f, 0.f };
		m_pPointInstanceBuffer[iIndex].vUp		= { 0.f, 1.f, 0.f, 0.f };
		m_pPointInstanceBuffer[iIndex].vLook	= { 0.f, 0.f, 1.f, 0.f };
	}

	return S_OK;
}

void CWallLaserTrap::Calcul_Transform(tagWallLaserTrap_Desc Data)
{
	_matrix WorldMatrix = XMMatrixIdentity();

	WorldMatrix		 = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Data.vRotateAngle.x), XMConvertToRadians(Data.vRotateAngle.y), XMConvertToRadians(Data.vRotateAngle.z));
	WorldMatrix.r[3] = XMLoadFloat4(&Data.vPosition);
	
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

_fmatrix CWallLaserTrap::Get_Matrix(_float4 vPos)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_vector vPosition	= XMLoadFloat4(&vPos);

	vPosition = XMVector3Transform(vPosition, WorldMatrix);

	WorldMatrix.r[3] = vPosition;

	return WorldMatrix;
}

void CWallLaserTrap::Render_PointInstance()
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	if (nullptr == pPipeline)
		return;

	m_pPointInstanceCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pPointInstanceCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINVIEW)), sizeof(_matrix));
	m_pPointInstanceCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ)), sizeof(_matrix));
	m_pPointInstanceCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBVIEW)), sizeof(_matrix));
	m_pPointInstanceCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ)), sizeof(_matrix));

	_float	fMainCamFar = pPipeline->Get_MainCamFar();
	_float	fSubCamFar = pPipeline->Get_SubCamFar();
	m_pPointInstanceCom->Set_Variable("g_fMainCamFar", &fMainCamFar, sizeof(_float));
	m_pPointInstanceCom->Set_Variable("g_fSubCamFar", &fSubCamFar, sizeof(_float));

	_vector vMainCamPosition = pPipeline->Get_MainCamPosition();
	_vector vSubCamPosition = pPipeline->Get_SubCamPosition();
	m_pPointInstanceCom->Set_Variable("g_vMainCamPosition", &vMainCamPosition, sizeof(_vector));
	m_pPointInstanceCom->Set_Variable("g_vSubCamPosition", &vSubCamPosition, sizeof(_vector));


	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	_float4 vColor = { 1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f };
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom->Set_Variable("g_fTime", &m_fActivateTime, sizeof(_float));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Mask->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(9, m_pPointInstanceBuffer, m_iPointInstance_Count);
}

void CWallLaserTrap::Check_Activate(_double TimeDelta)
{
	if (true == m_IsActivate)
	{
		m_fActivateTime += (_float)TimeDelta * 1.5f;
		if (1.f <= m_fActivateTime)
			m_fActivateTime = 1.f;
	}
	else
	{
		m_fActivateTime -= (_float)TimeDelta * 1.5f;
		if (0.f >= m_fActivateTime)
			m_fActivateTime = 0.f;
	}
}

CWallLaserTrap * CWallLaserTrap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWallLaserTrap*	pInstance = new CWallLaserTrap(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CWallLaserTrap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWallLaserTrap::Clone_GameObject(void * pArg)
{
	CWallLaserTrap* pInstance = new CWallLaserTrap(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CWallLaserTrap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWallLaserTrap::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelInstanceCom);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pPointInstanceCom);
	Safe_Release(m_pTexturesCom_Mask);	
	Safe_Release(m_pTransformCom_Trigger);

	Safe_Delete_Array(m_pPointInstanceBuffer);

	__super::Free();
}
