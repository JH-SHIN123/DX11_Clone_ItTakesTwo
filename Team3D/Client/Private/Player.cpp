#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"
#include "DataStorage.h"
CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	//Test
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pModelCom->Add_LerpInfo(0, 1, false);
	m_pModelCom->Add_LerpInfo(1, 2, true, 8.f);

	m_pModelCom->Set_Animation(0);
	m_pModelCom->Set_NextAnimIndex(0);
	
	//CDataStorage::GetInstance()->Set_PlayerPtr(this);

	PxCapsuleControllerDesc CapsuleControllerDesc;
	CapsuleControllerDesc.setToDefault();
	CapsuleControllerDesc.height = 0.5f;
	CapsuleControllerDesc.radius = 0.5f;
	CapsuleControllerDesc.material = m_pGameInstance->Create_PxMaterial(0.5f, 0.5f, 0.5f);
	CapsuleControllerDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	CapsuleControllerDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	CapsuleControllerDesc.contactOffset = 0.02f;
	CapsuleControllerDesc.stepOffset = 0.5f;
	CapsuleControllerDesc.upDirection = PxVec3(0.0, 1.0, 0.0);
	CapsuleControllerDesc.slopeLimit = 0.707f;
	CapsuleControllerDesc.position = MH_PxExtendedVec3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	////CapsuleControllerDesc.reportCallback = NULL;
	////CapsuleControllerDesc.behaviorCallback = NULL;
	//CapsuleControllerDesc.density = 10.f;
	//CapsuleControllerDesc.scaleCoeff = 0.8f;
	//CapsuleControllerDesc.invisibleWallHeight = 0.f;
	//CapsuleControllerDesc.maxJumpHeight = 10.f;
	//CapsuleControllerDesc.volumeGrowth = 1.5f;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &CControllableActor::ARG_DESC(m_pTransformCom, CapsuleControllerDesc, -50.f)), E_FAIL);


	//m_pTransformCom->Set_Scale(XMVectorSet(5.f, 5.f, 5.f, 0.f));

	return S_OK;
}

_int CPlayer::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	//if (m_pGameInstance->Key_Pressing(DIK_1))
	//	m_pModelCom->Set_Animation(1, m_pTransformCom);
	//if (m_pGameInstance->Key_Pressing(DIK_2))
	//	m_pModelCom->Set_Animation(2, m_pTransformCom);
	//if (m_pGameInstance->Key_Pressing(DIK_3))
	//	m_pModelCom->Set_Animation(4, m_pTransformCom);
	//if (m_pGameInstance->Key_Pressing(DIK_4))
	//	m_pModelCom->Set_Animation(5, m_pTransformCom);
	//if (m_pGameInstance->Key_Pressing(DIK_5))
	//	m_pModelCom->Set_Animation(6, m_pTransformCom);
	//if (m_pGameInstance->Key_Pressing(DIK_6))
	//	m_pModelCom->Set_Animation(7, m_pTransformCom);

	if (m_pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(dTimeDelta);


	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	PxMaterial* pMaterial = CPhysX::GetInstance()->Create_Material(0.5f, 0.5f, 0.f);

	if (m_pGameInstance->Key_Pressing(DIK_I))
		m_pActorCom->Move(XMVectorSet(0.f, 0.f, (_float)dTimeDelta * 10.f, 1.f), dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_K))
		m_pActorCom->Move(XMVectorSet(0.f, 0.f, (_float)-dTimeDelta * 10.f, 1.f), dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_J))
		m_pActorCom->Move(XMVectorSet((_float)-dTimeDelta * 10.f, 0.f, 0.f, 1.f), dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_L))
		m_pActorCom->Move(XMVectorSet((_float)dTimeDelta * 10.f, 0.f, 0.f, 1.f), dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_SPACE))
		m_pActorCom->Jump_Start(10.f);
	if (m_pGameInstance->Key_Pressing(DIK_SPACE))
		m_pActorCom->Jump_Higher(0.5f);

	m_pActorCom->Update(dTimeDelta);

	m_pModelCom->Update_Animation(dTimeDelta);

	return NO_EVENT;
}

_int CPlayer::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CPlayer::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CPlayer::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone_GameObject(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	Safe_Release(m_pActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}