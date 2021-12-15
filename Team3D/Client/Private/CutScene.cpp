#include "stdafx.h"
#include "..\Public\CutScene.h"
#include"CutScenePlayer.h"
#include"Performer.h"
#include"MoonBaboon.h"
#include"MainCamera.h"
#include"SubCamera.h"
#include"DataStorage.h"
#include"Film.h"
#include"Cody.h"
#include"May.h"
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

	_bool bIsNoError = false;
	switch (m_eCutSceneOption)
	{
	case Client::CCutScene::CutSceneOption::CutScene_Intro:
		bIsNoError =  Tick_CutScene_Intro(dTimeDelta);
		break;
	case CutSceneOption::CutScene_Active_GravityPath_01:
		bIsNoError = Tick_CutScene_Active_GravityPath_01(dTimeDelta);
		break;
	}
	if (bIsNoError == false)
	{
		MSG_BOX("CutScene Error");
	}

	return true;
}

_bool CCutScene::Tick_CutScene_Intro(_double dTimeDelta)
{
	if (m_iCutSceneTake == 0)
	{
		if (m_dTime >= 88.5f)
		{
			CPerformer* pMay = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_May_CutScene1"));
			CPerformer* pCody = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_Cody_CutScene1"));

			_float3 vMayPos = _float3(62.8f, 0.15f, 0.3f);
			_float3 vCodyPos = _float3(63.9f, 0.2f, 0.9f);
			pCody->Set_Position(vCodyPos);
			pMay->Set_Position(vMayPos);
			m_iCutSceneTake++;
		}
	}

	else if (m_iCutSceneTake == 1) //토이박스쪽으로 가서
	{
		if (m_dTime >= 97.1f)
		{
			if (false == m_bIsChangeToCody)
			{
				CPerformer* pCodyCutScene = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_Cody_CutScene1"));
				pCodyCutScene->Set_Position(_float3(0, 0, 0));
				CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
				pCody->Set_Position(XMVectorSet(63.9f, 0.2f, 0.9f, 1.f));
				pCody->Get_Transform()->Set_Rotaion(XMVectorSet( 0.f, -32.f, 0.f,0.f));
				pCody->Get_Model()->Set_Animation(548, 2913.0);
				CPerformer* pBelt = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltCutScene1"));
				pBelt->Set_Position(_float3(0, 0, 0));
				m_bIsChangeToCody = true;
			}

		}
		if (m_dTime >= 99.92)//~코디 사이즈키우기 M~L 5
		{
			StartCodyLerp(_float3(63.9f, 0.2f, 0.9f), _float3(1.f,1.f,1.f),
				_float3(57.8f, -8.1f, 3.5f), _float3(5.f, 5.f, 5.f), 7);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 2)
	{
		if (m_dTime >= 103.4)//코디 사이즈키우기 L~S 0.1
		{
			StartCodyLerp(_float3(57.8f, -8.1f, 3.5f), _float3(5.f, 5.f, 5.f), 
				_float3(65.f, 0.2f, 0.3f), _float3(0.1f, 0.1f, 0.1f), 3);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 3)
	{
		if (m_dTime >= 108.9f)//코디 사이즈키우기 S~L
		{
			StartCodyLerp(_float3(65.f, 0.2f, 0.3f), _float3(0.1f, 0.1f, 0.1f), 
				_float3(57.8f, -8.1f, 3.5f), _float3(5.f, 5.f, 5.f),  7);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 4)
	{
		if (m_dTime >= 111.32f)//코디 사이즈키우기 L~S
		{
			StartCodyLerp(_float3(57.8f, -8.1f, 3.5f), _float3(5.f, 5.f, 5.f), 
				_float3(65.f, 0.2f, 0.3f), _float3(0.1f, 0.1f, 0.1f), 3);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 5)
	{
		if (m_dTime >= 116.49f)//코디 사이즈키우기 S~M 1
		{
			StartCodyLerp(_float3(65.f, 0.2f, 0.3f), _float3(0.1f, 0.1f, 0.1f),
				_float3(63.9f, 0.2f, 0.9f), _float3(1.f, 1.f, 1.f), 7);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 6)
	{
		//118.8 
		//벨트 리모컨달기 ~코디로 진행
		if (m_dTime >= 118.8f)
		{
			CPerformer* pSizeBeltRemoteCutScene = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltRemoteControllerCutScene1"));
			pSizeBeltRemoteCutScene->Set_Position(_float3(0, 0, 0));
			m_iCutSceneTake++;
		}
	
	}
	else if (m_iCutSceneTake == 7)
	{
		//124.7 메이 부츠달기 ~메이로진행
		
		if (m_dTime >= 124.7)
		{
			CPerformer* pMayCutScene = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_May_CutScene1"));
			CPerformer* pBootsCutScene = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_GravityBootsCutScene1"));
			pMayCutScene->Set_Position(_float3(0.f, 0.f, 0.f));
			pBootsCutScene->Set_Position(_float3(0, 0, 0));
			CMay* pMay = static_cast<CMay*>(DATABASE->GetMay());
			pMay->Set_Position(XMVectorSet(62.8f, 0.15f, 0.3f,1.f));
			pMay->Get_Transform()->Set_Rotaion(XMVectorSet(0.f, -32.f, 0.f, 0.f));
			pMay->Get_Model()->Set_Animation(522,3741.0);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 8)
		{
			if (m_dTime >= 128.51)
			{
				((CCody*)DATABASE->GetCody())->Set_Position(XMVectorSet(65.4f, 0.2f, 1.1f,1.f));
				((CMay*)DATABASE->GetCody())->Set_Position(XMVectorSet(63.5f, 0.15f, 0.3f,1.f));
			}
		}

	CodyLerp();
	//		case Client::CCody::SIZE_SMALL:
	//			vPreCodySize=	XMVectorSet(0.1f, 0.1f, 0.1f, 0.f);
	//			vPreCodyPos =	XMVectorSet(65.f, 0.2f, 0.3f, 1.f);
	//			break;
	//		case Client::CCody::SIZE_MEDIUM:
	//			vPreCodySize = XMVectorSet(1.f, 1.f, 1.f, 0.f);
	//			vPreCodyPos = XMVectorSet(62.8f, 0.15f, 0.3f, 1.f);
	//			break;
	//		case Client::CCody::SIZE_LARGE:
	//			vPreCodySize = XMVectorSet(5.f, 5.f, 5.f, 0.f);
	//			vPreCodyPos = XMVectorSet(57.8f, -8.1f, 3.5f, 1.f);
	//			break;

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
	m_iCutSceneTake = 0;
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
#ifdef __TEST_JUN
	_double dTime = 0.0;
#else
	_double dTime = 0.0;
#endif
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), false);
	static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Start_Film(L"Film_Begin_Game");


	CPerformer::PERFORMERDESC tDesc;
	CGameObject* pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_Cody_CutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 62.8f,0.15f,0.3f };
	tDesc.vRot = { -32.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform(0,dTime);


	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_May_CutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform(0, dTime);

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_ToyBox1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.4f,0.15f,-4.45f };
	tDesc.vRot = { 268.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform(0, dTime);

	pPerformer = DATABASE->Get_MoonBaboon();
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
	static_cast<CPerformer*>(pPerformer)->Start_Perform(0, dTime);

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_GravityBootsCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.43f	, 0.15f, - 4.55f };
	tDesc.vRot = { 180.f, 0.f , 0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform(0, dTime);

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	tDesc.vPosition = { 65.39f,0.15f,-4.65f };
	tDesc.vRot = { 180.f,0.f,0.f };
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform(0, dTime);
//#ifdef __TEST_JUN
//	if (false == static_cast<CPerformer*>(pPerformer)->Get_IsAlreadyOnParentBone())
//	{
//		if (nullptr == pPerformer)
//			return false;
//		CPerformer* pCody = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_Cody_CutScene1"));
//		static_cast<CPerformer*>(pPerformer)->Set_TransformToParentBone(pCody->Get_Transform(), pCody->Get_Model(), "FrontRobe3");
//	}
//#endif
	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltRemoteControllerCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform(0, dTime);



	
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
	m_bIsChangeToCody = false;
	m_bIsChangeToMay = false;
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

void CCutScene::StartCodyLerp(_float3 vStartPos, _float3 vStartScale, _float3 vTargetPos, _float3 vTargetScale, _uint iLerpCount)
{
	if (m_bStartLerpCody)
		return;
	m_bStartLerpCody = true;
	m_iCodyLerpCount = 0;
	m_iMaxCodyLerpCount = iLerpCount;
	m_vCodyPos = vStartPos;
	m_vCodyScale = vStartScale;

	m_vTargetCodyPos = vTargetPos;
	m_vTargetCodyScale = vTargetScale;

}

void CCutScene::CodyLerp()
{
	if (!m_bStartLerpCody)
		return;

	m_iCodyLerpCount++;

	CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
	_vector vCurPos = XMVectorLerp(XMLoadFloat3(&m_vCodyPos), XMLoadFloat3(&m_vTargetCodyPos), m_iCodyLerpCount *1.f / m_iMaxCodyLerpCount);
	_vector vCurScale = XMVectorLerp(XMLoadFloat3(&m_vCodyScale), XMLoadFloat3(&m_vTargetCodyScale), m_iCodyLerpCount *1.f / m_iMaxCodyLerpCount);

	pCody->Get_Transform()->Set_Scale(vCurScale);
	pCody->Set_Position(XMVectorSetW(vCurPos,1.f));

	if (m_iCodyLerpCount == m_iMaxCodyLerpCount)
	{
		pCody->Get_Transform()->Set_Scale(XMLoadFloat3(&m_vTargetCodyScale));
		pCody->Set_Position(XMVectorSetW(XMLoadFloat3(&m_vTargetCodyPos), 1.f));

		m_bStartLerpCody = false;
	}
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
