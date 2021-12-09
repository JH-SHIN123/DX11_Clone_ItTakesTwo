#include "stdafx.h"
#include "..\Public\CutScene.h"
#include"CutScenePlayer.h"
#include"Performer.h"
#include"MoonBaboon.h"
#include"MainCamera.h"
#include"SubCamera.h"
#include"DataStorage.h"
#include"Film.h"
CCutScene::CCutScene()
{
}

_bool CCutScene::Tick_CutScene(_double dTimeDelta)
{
	//컷씬중에 바꿔줄것들 
	if (m_dTime > m_dDuration)
	{
		switch (m_eCutSceneOption)
		{
		case Client::CCutScene::CutSceneOption::CutScene_Intro:
			End_CutScene_Intro();
			break;
		case Client::CCutScene::CutSceneOption::CutScene_Active_GravityPath_01:
			End_CutScene_Active_GravityPath_01();
			break;
		}
		return false;
	}
	m_dTime += dTimeDelta;

	
	switch (m_eCutSceneOption)
	{
	case Client::CCutScene::CutSceneOption::CutScene_Intro:
		return Tick_CutScene_Intro(dTimeDelta);
	case CutSceneOption::CutScene_Active_GravityPath_01:
		return Tick_CutScene_Active_GravityPath_01(dTimeDelta);
	}


	return true;
}

_bool CCutScene::Tick_CutScene_Intro(_double dTimeDelta)
{
	CPerformer* pMay = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_May_CutScene1"));
	CPerformer* pCody = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_Cody_CutScene1"));
	_float3 vMayPos = pMay->Get_Pos();
	_float3 vCodyPos = pCody->Get_Pos();
	_float3 vCodyScale = pCody->Get_Scale();
	
	if (m_dTime >= 88.55) //아이템 얻으러감
	{
		vMayPos = (_float3(62.8f, 0.15f, 0.3f));
		vCodyPos = _float3(63.9f, 0.2f, 0.9f);
	}	
	//97벨트달기
	_float fCurCodySize = 1.f;
	_float fCodyChangeSpeed = 4.f;
	_float3 vTargetCodyPos = vCodyPos;
	if (m_dTime >= 100.0)//코디 사이즈키우기 L 5
	{
		m_eCurCodySize = CCody::PLAYER_SIZE::SIZE_LARGE;

	}
	if (m_dTime >= 103.8)//코디 사이즈키우기 S 0.1
	{
		m_eCurCodySize = CCody::PLAYER_SIZE::SIZE_SMALL;

	}
	if (m_dTime >= 108.8f)//코디 사이즈키우기 L
	{
		m_eCurCodySize = CCody::PLAYER_SIZE::SIZE_LARGE;

	}
	if (m_dTime >= 111.f)//코디 사이즈키우기 S
	{
		m_eCurCodySize = CCody::PLAYER_SIZE::SIZE_SMALL;

	}
	//118.8 
	//벨트 리모컨달기

	if (m_dTime >= 116.8f)//코디 사이즈키우기 M 1
	{
		m_eCurCodySize = CCody::PLAYER_SIZE::SIZE_MEDIUM;

	}
	//124.7 메이 부츠달기

	if (m_eCurCodySize != m_ePreCodySize)
	{
		m_fCodySizingTime += dTimeDelta;
		_vector vPreCodySize,vPreCodyPos;
		switch (m_ePreCodySize)
		{
			case Client::CCody::SIZE_SMALL:
				vPreCodySize=	XMVectorSet(0.1f, 0.1f, 0.1f, 0.f);
				vPreCodyPos =	XMVectorSet(65.f, 0.2f, 0.3f, 1.f);
				break;
			case Client::CCody::SIZE_MEDIUM:
				vPreCodySize = XMVectorSet(1.f, 1.f, 1.f, 0.f);
				vPreCodyPos = XMVectorSet(62.8f, 0.15f, 0.3f, 1.f);
				break;
			case Client::CCody::SIZE_LARGE:
				vPreCodySize = XMVectorSet(5.f, 5.f, 5.f, 0.f);
				vPreCodyPos = XMVectorSet(57.8f, -8.1f, 3.5f, 1.f);
				break;
		}
		switch (m_eCurCodySize)
		{
			case Client::CCody::SIZE_SMALL:
			{
				XMStoreFloat3(&vCodyScale,XMVectorLerp(vPreCodySize,XMVectorSet(0.1f, 0.1f, 0.1f, 0.f),m_fCodySizingTime));
				XMStoreFloat3(&vCodyPos, XMVectorLerp(vPreCodyPos, XMVectorSet(65.f, 0.2f, 0.3f, 1.f), m_fCodySizingTime));
			}
				break;
			case Client::CCody::SIZE_MEDIUM:
			{
				XMStoreFloat3(&vCodyScale, XMVectorLerp(vPreCodySize, XMVectorSet(1.f, 1.f, 1.f, 0.f), m_fCodySizingTime));
				XMStoreFloat3(&vCodyPos, XMVectorLerp(vPreCodyPos, XMVectorSet(62.8f, 0.15f, 0.3f, 1.f), m_fCodySizingTime));
			}
				break;
			case Client::CCody::SIZE_LARGE:
			{
				XMStoreFloat3(&vCodyScale, XMVectorLerp(vPreCodySize, XMVectorSet(5.f, 5.f, 5.f, 0.f), m_fCodySizingTime));
				XMStoreFloat3(&vCodyPos, XMVectorLerp(vPreCodyPos, XMVectorSet(57.8f, -8.1f, 3.5f, 1.f), m_fCodySizingTime));
			}
				break;
		}
		if(m_fCodySizingTime >= 1.f)
			m_ePreCodySize = m_eCurCodySize;
	}
	else 
		m_fCodySizingTime = 0.f;
	if (m_dTime >= 128.51)
	{
		vCodyPos = _float3(65.0f, 0.2f, 1.1f);
		vMayPos = _float3(63.5f, 0.15f, 0.3f);
	}
	
	pCody->Set_Position(vCodyPos);
	pCody->Set_Scale(vCodyScale);
	pMay->Set_Position(vMayPos);
	return true;
}

_bool CCutScene::Tick_CutScene_Active_GravityPath_01(_double dTimeDelta)
{
	CCam_Helper* pCamHelper = static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Get_CamHelper();
	
	if (!m_bIsStartFilm &&pCamHelper->Get_CamHelperState(CFilm::RScreen) != CCam_Helper::CamHelperState::Helper_SeeCamNode )
	{
		pCamHelper->Start_Film(TEXT("Film_Active_GravityPath_01"), CFilm::RScreen);
		m_bIsStartFilm = true;
	}
	
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
	case CutSceneOption::CutScene_Active_GravityPath_01:
		if (FAILED(Start_CutScene_Active_GravityPath_01()))
			return E_FAIL;
		break;
	}
	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Intro()
{
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), false);
	CPerformer::PERFORMERDESC tDesc;
	CGameObject* pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_Cody_CutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 62.8f,0.15f,0.3f };
	tDesc.vRot = { -32.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();


	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_May_CutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_ToyBox1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.4f,0.15f,-4.45f };
	tDesc.vRot = { 268.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_MoonBaboon");
	if (nullptr == pPerformer)
		return E_FAIL;
	static_cast<CMoonBaboon*>(pPerformer)->Get_Transform()->Set_WorldMatrix(
		MakeRollPitchYawMatrix(_float3(62.8f, 0.5f, -6.2f), _float3(1.f, 1.f, 1.f), _float3(90.f, -90.f, 0.f)));
	static_cast<CMoonBaboon*>(pPerformer)->Get_Model()->Set_Animation(7);

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_RemoteContollerCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 62.8f, 0.5f, -6.2f };
	tDesc.vRot = { 90.f, -90.f, 0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_GravityBootsCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.43f	, 0.15f, - 4.55f };
	tDesc.vRot = { 180.f, 0.f , 0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.39f,0.15f,-4.65f };
	tDesc.vRot = { 180.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltRemoteControllerCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform();



	static_cast<CMainCamera*>(CDataStorage::GetInstance()->Get_MainCam())->Start_Film(L"Film_Begin_Game");
	

	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Active_GravityPath_01()
{
	
	m_bIsStartFilm = false;
	CCam_Helper* pCamHelper = static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Get_CamHelper();
	if (nullptr == pCamHelper)
		return E_FAIL;
	CFilm::CamNode* pFirstNode = pCamHelper->Get_Film(TEXT("Film_Active_GravityPath_01"))->Get_CamNodes()->front();
	pCamHelper->SeeCamNode(pFirstNode, CFilm::RScreen);
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), true, 1.f);
	
	return S_OK;
}

HRESULT CCutScene::End_CutScene_Intro()
{
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), true, 1.f);
	return S_OK;
}

HRESULT CCutScene::End_CutScene_Active_GravityPath_01()
{
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), true, 1.f);
	return E_NOTIMPL;
}

HRESULT CCutScene::Ready_CutScene_Intro()
{
	m_dDuration = 138.0;
	return S_OK;
}

HRESULT CCutScene::Ready_CutScene_Active_GravityPath_01()
{
	m_dDuration = 10.5;
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
	case Client::CCutScene::CutSceneOption::CutScene_Active_GravityPath_01:
		if (FAILED(Ready_CutScene_Active_GravityPath_01()))
			return E_FAIL;
		break;
	}
	m_pCutScenePlayer = CCutScenePlayer::GetInstance();
	return S_OK;
}

_fmatrix CCutScene::MakeRollPitchYawMatrix(_float3 vPos, _float3 vScale, _float3 vRot)
{
	return	XMMatrixScaling(vScale.x,vScale.y, vScale.z) *
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
}
