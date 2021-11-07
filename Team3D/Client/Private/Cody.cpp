#include "stdafx.h"
#include "..\public\Cody.h"
#include "GameInstance.h"
#include "BehaviorTree_Cody.h"

CCody::CCody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCharacter(pDevice, pDeviceContext)
{
}

CCody::CCody(const CCody& rhs)
	: CCharacter(rhs)
{
}

_bool CCody::IsFinish_CurAnimation()
{

	return _bool();
}

void CCody::Set_Animtaion(_int iState)
{
	m_pModelCom->Set_Animation(iState,m_pTransformCom);
}

HRESULT CCody::NativeConstruct_Prototype()
{
	CCharacter::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CCody::NativeConstruct(void* pArg)
{
	CCharacter::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pModelCom->Set_Animation(MH, m_pTransformCom); // 바로 변경
	m_pModelCom->Set_NextAnimIndex(MH); // 끝난다음 변경

	m_pBehaviorTree = CBehaviorTree_Cody::Create(this);

	return S_OK;
}

_int CCody::Tick(_double dTimeDelta)
{
	CCharacter::Tick(dTimeDelta);

	KeyProcess(dTimeDelta);

	m_pBehaviorTree->Running(dTimeDelta);

	StateCheck();

	m_pModelCom->Update_Animation(dTimeDelta, m_pTransformCom);

	return NO_EVENT;
}

_int CCody::Late_Tick(_double dTimeDelta)
{
	CCharacter::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CCody::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(0);

	return S_OK;
}

void CCody::KeyProcess(_double TimeDelta)
{
	_bool bJog = false;
	if (m_pGameInstance->Key_Pressing(DIK_W)) 
		bJog = true;
	if (m_pGameInstance->Key_Pressing(DIK_S))
		bJog = true;
	if (m_pGameInstance->Key_Pressing(DIK_A)) {
		bJog = true;
		m_bJog_Left = bJog;
	}
	if (m_pGameInstance->Key_Pressing(DIK_D)) {
		bJog = true;
		m_bJog_Right = bJog;
	}

	m_bJog = bJog;

	if (false == m_bJog) {
		m_bJog_Left = false;
		m_bJog_Right = false;
	}
}

void CCody::StateCheck()
{
	if (m_iCurState != m_iNextState)
	{
		switch (m_iNextState)
		{
		}

		m_pModelCom->Set_Animation(m_iNextState,m_pTransformCom);
		m_iCurState = m_iNextState;
	}
}

CCody* CCody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCody* pInstance = new CCody(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CCody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCody::Clone_GameObject(void* pArg)
{
	CCody* pInstance = new CCody(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CCody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCody::Free()
{
	Safe_Release(m_pBehaviorTree);
	CCharacter::Free();
}