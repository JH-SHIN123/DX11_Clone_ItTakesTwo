#include "stdafx.h"
#include "..\Public\CutScene.h"
#include"CutScenePlayer.h"
#include"Performer.h"
#include"MoonBaboon.h"
#include"MainCamera.h"
CCutScene::CCutScene()
{
}

_bool CCutScene::Tick_CutScene(_double dTimeDelta)
{
	//ÄÆ¾ÀÁß¿¡ ¹Ù²ãÁÙ°Íµé 
	return true;
}

HRESULT CCutScene::Start_CutScene()
{
	m_dTime = 0.0;
	switch (m_eCutSceneOption)
	{
	case Client::CCutScene::CutSceneOption::CutScene_Intro:
		if (FAILED(Start_CutScene_Intro()))
			return E_FAIL;
		break;
	}
	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Intro()
{
	CPerformer::PERFORMERDESC tDesc;
	CGameObject* pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_Cody_CutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 61.5f,0.15f,2.f };
	tDesc.vRot = { -29.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();


	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_May_CutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 62.8f,0.15f,0.3f };
	tDesc.vRot = { -32.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_ToyBox1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.4f,0.f,-4.45f };
	tDesc.vRot = { 268.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_ToyBox1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.4f,0.f,-4.45f };
	tDesc.vRot = { 268.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_RemoteContollerCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 62.8f,25.9f,4.05f };
	tDesc.vRot = { 90.f,-30.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_GravityBootsCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.28f,0.15f,-4.65f };
	tDesc.vRot = { 275.f,90.f,180.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.37f,0.f,-3.89f };
	tDesc.vRot = { 77.f,90.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltRemoteControllerCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.37f,0.f,-3.89f };
	tDesc.vRot = { 77.f,90.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_MoonBaboon");
	if (nullptr == pPerformer)
		return E_FAIL;
	static_cast<CMoonBaboon*>(pPerformer)->Set_Animation(7);
	static_cast<CMoonBaboon*>(pPerformer)->Get_Transform()->Set_WorldMatrix(MakeRollPitchYawMatrix(_float3(62.8f,0.5f,-6.2f),_float3(1.f,1.f,1.f),_float3(90.f,-90.f,0.f)));

	static_cast<CMainCamera*>(CDataStorage::GetInstance()->Get_MainCam())->Start_Film(L"Film_Begin_Game");
	

	return S_OK;
}

HRESULT CCutScene::Ready_CutScene_Intro()
{
	m_dDuration = 138.0;
	return S_OK;
}

HRESULT CCutScene::NativeConstruct(CutSceneOption eOption)
{
	switch (m_eCutSceneOption = eOption)
	{
	case Client::CCutScene::CutSceneOption::CutScene_Intro:
		if (FAILED(Ready_CutScene_Intro()))
			return E_FAIL;
		break;
	case Client::CCutScene::CutSceneOption::CutScene_End:
		break;
	}
	m_pCutScenePlayer = CCutScenePlayer::GetInstance();
	Safe_AddRef(m_pCutScenePlayer);
	return S_OK;
}

_fmatrix CCutScene::MakeRollPitchYawMatrix(_float3 vPos, _float3 vScale, _float3 vRot)
{
	return XMMatrixScaling(vScale.x,vScale.y, vScale.z) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(vRot.z), XMConvertToRadians(vRot.x), XMConvertToRadians(vRot.y))*
		XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	
}


CCutScene * CCutScene::Create(CutSceneOption eOption)
{
	CCutScene* pInstance = new CCutScene;
	if (FAILED(pInstance->NativeConstruct(eOption)))
	{
		MSG_BOX("Failed to Create CutScene");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCutScene::Free()
{
	Safe_Release(m_pCutScenePlayer);
}
