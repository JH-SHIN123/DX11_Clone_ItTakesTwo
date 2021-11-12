#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"

#include "UI_Generator.h"

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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pModelCom->Set_Animation(0, m_pTransformCom);
	m_pModelCom->Set_NextAnimIndex(0);

	return S_OK;
}

_int CPlayer::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pModelCom->Update_Animation(dTimeDelta, m_pTransformCom);

	return NO_EVENT;
}

_int CPlayer::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_1))
	{
		CUI_Generator::GetInstance()->Set_TriggerOn();
		CUI_Generator::GetInstance()->Generator_UI(Player::Player_May, UI::InputButton_PS_L2);
	}

	if (m_pGameInstance->Key_Down(DIK_2))
	{

		CUI_Generator::GetInstance()->Set_TriggerOn();
		CUI_Generator::GetInstance()->Generator_UI(Player::Player_May, UI::InputButton_PS_R2);
	}
	if (m_pGameInstance->Key_Down(DIK_3))
	{
		CUI_Generator::GetInstance()->Set_TriggerOn();
		CUI_Generator::GetInstance()->Generator_UI(Player::Player_Cody, UI::InputButton_PS_InterActive);
	}

	if (m_pGameInstance->Key_Down(DIK_4))
	{
		CUI_Generator::GetInstance()->Set_TriggerOn();
		CUI_Generator::GetInstance()->Generator_UI(Player::Player_May, UI::InputButton_Dot);
	}


	if(m_pGameInstance->Key_Down(DIK_Z))
		CUI_Generator::GetInstance()->Delete_UI(Player::Player_Cody, UI::InputButton_Dot);

	if (m_pGameInstance->Key_Down(DIK_V))
		CUI_Generator::GetInstance()->Delete_UI(Player::Player_May, UI::InputButton_Dot);





	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CPlayer::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(0);

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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}