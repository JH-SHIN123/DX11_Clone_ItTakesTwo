#include "stdafx.h"
#include "..\public\Performer.h"
#include "GameInstance.h"
#include "DataStorage.h"
#include"CutScenePlayer.h"
#ifdef _AFX
#include"PerformerOption.h"
#include"MainForm.h"
#endif
CPerformer::CPerformer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CPerformer::CPerformer(const CPerformer & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPerformer::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPerformer::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);


	m_tDesc = *(PERFORMERDESC*)(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_tDesc.strModelTag.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (m_tDesc.strModelTag == TEXT("Component_Model_ControlRoom_Keyboard_01_Button"))
	{
		_uint iNum = CCutScenePlayer::GetInstance()->Get_ButtonNum();
		_tchar szBuf[32] = L"";
		_itow_s(iNum, szBuf, 10);
		m_tDesc.strModelTag += szBuf;
		CCutScenePlayer::GetInstance()->Add_ButtonNum();
		m_bIsButton = true;
	}
	if(m_tDesc.strModelTag == TEXT("Component_Model_ControlRoom_Button_Large_01_Button"))
		m_bIsButton = true;
	m_pModelTag = new wstring(m_tDesc.strModelTag.c_str());

	CCutScenePlayer::GetInstance()->Add_Performer(m_pModelTag->c_str(), this);

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_tDesc.vScale));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tDesc.vPosition), 1.f));


	m_pModelCom->Set_Animation(0);
	return S_OK;
}

_int CPerformer::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);


	_matrix matWorld = XMMatrixIdentity();

	matWorld = XMMatrixScaling(m_tDesc.vScale.x, m_tDesc.vScale.y, m_tDesc.vScale.z) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_tDesc.vRot.z), XMConvertToRadians(m_tDesc.vRot.x), XMConvertToRadians(m_tDesc.vRot.y))*
		XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);
	m_pTransformCom->Set_WorldMatrix(matWorld);


#ifdef __TEST_JUN
	if (m_pGameInstance->Key_Down(DIK_NUMPADENTER))
		m_bStartAnim = !m_bStartAnim;
#endif
	if (m_bButtonDown)
		Start_ButtonPress(dTimeDelta);
	
	if(false == m_bIsButton)
	m_pModelCom->Update_Animation(m_bStartAnim ? dTimeDelta : 0.0/*CCutScenePlayer::GetInstance()->Get_TimeDelta()*/);

	return NO_EVENT;
}

_int CPerformer::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	return NO_EVENT;
}






HRESULT CPerformer::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();

	m_bIsButton ? m_pModelCom->Render_Model(1) : m_pModelCom->Render_Model(0);

	return S_OK;
}



void CPerformer::Start_Perform(_uint iAnimIdx, _double dAnimTime)
{
	m_pModelCom->Set_CutSceneAnimation(iAnimIdx,dAnimTime);
	m_bStartAnim = true;

}

void CPerformer::Finish_Perform()
{
	m_bStartAnim = false;
}

void CPerformer::ButtonDown()
{
	m_bButtonDown = true;
}

void CPerformer::Start_ButtonPress(_double dTimeDelta)
{
	if (false == m_bIsButton)
		return;
	if (m_bStartPressing == false && m_tDesc.vPosition.y > 218.5f)
	{
		m_tDesc.vPosition.y -= (_float)dTimeDelta *2.f;
	}
	else
		m_bStartPressing = true;
	if (m_tDesc.strModelTag == L"Component_Model_ControlRoom_Button_Large_01_Button")
	{
		if (m_bStartPressing == true && m_tDesc.vPosition.y < 218.652f)
		{
			m_tDesc.vPosition.y += (_float)dTimeDelta * 2.f;
		}
	}
	else
	{
		if (m_bStartPressing == true && m_tDesc.vPosition.y < 219.131f)
		{
			m_tDesc.vPosition.y += (_float)dTimeDelta * 2.f;
		}
	}
}


CPerformer * CPerformer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPerformer* pInstance = new CPerformer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPerformer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPerformer::Clone_GameObject(void * pArg)
{
	CPerformer* pInstance = new CPerformer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPerformer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPerformer::Free()
{
	Safe_Delete(m_pModelTag);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}