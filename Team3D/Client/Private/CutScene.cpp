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
#include"PlayerActor.h"
#include"UFO.h"
#include"Sound_Manager.h"
#include"UI_Generator.h"
#include"Script.h"
#include"ControlRoom_Monitor.h"
#include"GameInstance.h"
#include "Effect_Generator.h"
CCutScene::CCutScene()
{
}

_bool CCutScene::Tick_CutScene(_double dTimeDelta)
{
	//컷씬중에 바꿔줄것들 
	m_dTime += dTimeDelta;

	if (m_dTime > m_dDuration)
	{
		UI_Delete(Cody, CutSceneBar);
		UI_Delete(May, CutSceneBar);
		UI_Generator->Set_AllActivation(false);

		switch (m_eCutSceneOption)
		{
		case Client::CCutScene::CutSceneOption::CutScene_Intro:
			End_CutScene_Intro();
			break;
		case Client::CCutScene::CutSceneOption::CutScene_Active_GravityPath_01:
			End_CutScene_Active_GravityPath_01();
			break;
		case Client::CCutScene::CutSceneOption::CutScene_Clear_Umbrella:
			End_CutScene_Clear_Umbrella();
			break;
		case Client::CCutScene::CutSceneOption::CutScene_Clear_Rail:
			End_CutScene_Clear_Rail();
			break;
		case Client::CCutScene::CutSceneOption::CutScene_Boss_Intro:
			End_CutScene_Boss_Intro();
			break;
		case Client::CCutScene::CutSceneOption::CutScene_Eject_InUFO:
			End_CutScene_Eject_InUFO();
			break;
		case Client::CCutScene::CutSceneOption::CutScene_GotoMoon:
			End_CutScene_GotoMoon();
			break;
		case Client::CCutScene::CutSceneOption::CutScene_Outro:
			End_CutScene_Outro();
			break;
		}
		return false;
	}

	_bool bIsNoError = false;
	switch (m_eCutSceneOption)
	{
	case Client::CCutScene::CutSceneOption::CutScene_Intro:
		bIsNoError = Tick_CutScene_Intro(dTimeDelta);
		break;
	case CutSceneOption::CutScene_Active_GravityPath_01:
		bIsNoError = Tick_CutScene_Active_GravityPath_01(dTimeDelta);
		break;
	case CutSceneOption::CutScene_Clear_Umbrella:
		bIsNoError = Tick_CutScene_Clear_Umbrella(dTimeDelta);
		break;
	case CutSceneOption::CutScene_Clear_Rail:
		bIsNoError = Tick_CutScene_Clear_Rail(dTimeDelta);
		break;
	case CutSceneOption::CutScene_Boss_Intro:
		bIsNoError = Tick_CutScene_Boss_Intro(dTimeDelta);
		break;
	case CutSceneOption::CutScene_Eject_InUFO:
		bIsNoError = Tick_CutScene_Eject_InUFO(dTimeDelta);
		break;
	case CutSceneOption::CutScene_GotoMoon:
		bIsNoError = Tick_CutScene_GotoMoon(dTimeDelta);
		break;
	case CutSceneOption::CutScene_Outro:
		bIsNoError = Tick_CutScene_Outro(dTimeDelta);
		break;
	}
	if (bIsNoError == false)
	{
		MSG_BOX("CutScene Error");
	}

	EFFECT->CutScene_Effect_Timer(dTimeDelta);

	return true;
}

_bool CCutScene::Tick_CutScene_Intro(_double dTimeDelta)
{
	Script_Intro(dTimeDelta);
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
				static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_ToyBox1"))->Finish_Perform();

				pCodyCutScene->Set_Position(_float3(0, 0, 0));
				CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
				pCody->Get_Actor()->Set_Position(XMVectorSet(63.9f, 0.2f, 0.9f, 1.f));
				pCody->Get_Transform()->Set_Rotaion(XMVectorSet( 0.f, -32.f, 0.f,0.f));
				pCody->Get_Model()->Set_Animation(551, 2913.0);
				CPerformer* pBelt = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltCutScene1"));
				pBelt->Set_Position(_float3(0, 0, 0));
				m_bIsChangeToCody = true;
			}
		}
		if (m_dTime >= 99.92)//~코디 사이즈키우기 M~L 5
		{
			EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::Cody_Size_ML);
			StartCodyLerp(_float3(63.9f, 0.2f, 0.9f), _float3(1.f,1.f,1.f),
				_float3(57.8f, -8.1f, 3.5f), _float3(5.f, 5.f, 5.f), 7);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 2)
	{
		if (m_dTime >= 103.4)//코디 사이즈키우기 L~S 0.1
		{
			EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::Cody_Size_LS);
			StartCodyLerp(_float3(57.8f, -8.1f, 3.5f), _float3(5.f, 5.f, 5.f), 
				_float3(65.f, 0.2f, 0.3f), _float3(0.1f, 0.1f, 0.1f), 3);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 3)
	{
		if (m_dTime >= 108.9f)//코디 사이즈키우기 S~L
		{
			EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::Cody_Size_SL);
			StartCodyLerp(_float3(65.f, 0.2f, 0.3f), _float3(0.1f, 0.1f, 0.1f), 
				_float3(57.8f, -8.1f, 3.5f), _float3(5.f, 5.f, 5.f),  7);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 4)
	{
		if (m_dTime >= 111.32f)//코디 사이즈키우기 L~S
		{
			EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::Cody_Size_LS);
			StartCodyLerp(_float3(57.8f, -8.1f, 3.5f), _float3(5.f, 5.f, 5.f), 
				_float3(65.f, 0.2f, 0.3f), _float3(0.1f, 0.1f, 0.1f), 3);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 5)
	{
		if (m_dTime >= 116.49f)//코디 사이즈키우기 S~M 1
		{
			EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::Cody_Size_SM);
			StartCodyLerp(_float3(65.f, 0.2f, 0.3f), _float3(0.1f, 0.1f, 0.1f),
				_float3(63.9f, 0.2f, 0.9f), _float3(1.f, 1.f, 1.f), 7);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 6)
	{
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
			pMay->Get_Actor()->Set_Position(XMVectorSet(62.8f, 0.15f, 0.3f,1.f));
			pMay->Get_Transform()->Set_Rotaion(XMVectorSet(0.f, -32.f, 0.f, 0.f));
			pMay->Get_Model()->Set_Animation(48,3741.0);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 8)
		{
			if (m_dTime >= 128.51)
			{
				((CCody*)DATABASE->GetCody())->Get_Actor()->Set_Position(XMVectorSet(65.4f, 0.2f, 1.1f,1.f));
				((CMay*)DATABASE->GetMay())->Get_Actor()->Set_Position(XMVectorSet(63.5f, 0.15f, 0.3f,1.f));
			}
		}

	CodyLerp();

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

_bool CCutScene::Tick_CutScene_Clear_Umbrella(_double dTimeDelta)
{
	CCam_Helper* pCamHelper = static_cast<CMainCamera*>(CDataStorage::GetInstance()->Get_MainCam())->Get_CamHelper();

	if (!m_bIsStartFilm &&pCamHelper->Get_CamHelperState(CFilm::LScreen) != CCam_Helper::CamHelperState::Helper_SeeCamNode)
	{
		pCamHelper->Start_Film(TEXT("Film_Clear_Umbrella"), CFilm::LScreen);
		m_bIsStartFilm = true;
	}
	if (m_iCutSceneTake == 0)
	{
		if (m_dTime >= 6.0)
		{
			_matrix matRot = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
			static_cast<CCody*>(DATABASE->GetCody())->Get_Transform()->Set_WorldMatrix(matRot);
			static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_Position(XMVectorSet(-635.f, 756.f, 195.f, 1.f));
			static_cast<CCody*>(DATABASE->GetCody())->Enforce_IdleState();
			static_cast<CCody*>(DATABASE->GetCody())->Set_Change_Size_After_UmbrellaCutScene();
			static_cast<CMay*>(DATABASE->GetMay())->Get_Transform()->Set_WorldMatrix(matRot);
			static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_Position(XMVectorSet(-635.f, 756.f, 197.f, 1.f));
			// 메이 중력 초기화
			static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_Gravity_Normally();
			m_iCutSceneTake++;
		}
	}
	return true;
}

_bool CCutScene::Tick_CutScene_Clear_Rail(_double dTimeDelta)
{
	CCam_Helper* pCamHelper = static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Get_CamHelper();

	if (!m_bIsStartFilm &&pCamHelper->Get_CamHelperState(CFilm::RScreen) != CCam_Helper::CamHelperState::Helper_SeeCamNode)
	{
		pCamHelper->Start_Film(TEXT("Film_Clear_Rail"), CFilm::RScreen);
		m_bIsStartFilm = true;
	}
	if (m_iCutSceneTake == 0)
	{
		if (m_dTime >= 6.0)
		{
			_matrix matRot = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			static_cast<CCody*>(DATABASE->GetCody())->Enforce_IdleState();
			static_cast<CCody*>(DATABASE->GetCody())->Set_Change_Size_After_UmbrellaCutScene();
			static_cast<CCody*>(DATABASE->GetCody())->Get_Transform()->Set_WorldMatrix(matRot);
			static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_Position(XMVectorSet(633.f,756.f,196.f,1.f));
			static_cast<CMay*>(DATABASE->GetMay())->Get_Transform()->Set_WorldMatrix(matRot);
			static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_Position(XMVectorSet(633.f,756.f,193.f,1.f));

			m_iCutSceneTake++;
		}
	}
	return true;
}

_bool CCutScene::Tick_CutScene_Boss_Intro(_double dTimeDelta)
{
	Script_Boss_Intro(dTimeDelta);
	
	CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
	CMay* pMay = static_cast<CMay*>(DATABASE->GetMay());
	if(m_dTime > 1.5f && m_dTime < 1.5f + dTimeDelta)
		CSound_Manager::GetInstance()->Play_Sound(TEXT("CutScene02.wav"), CHANNEL_CUTSCENE, 1.f, false);
	if (m_iCutSceneTake == 0)
	{
		if (m_dTime < 25.02)
		{
			pCody->Get_Actor()->Set_Position(XMVectorSet(73.5f, 244.5f, 168.5f, 1.f));
			pMay->Get_Actor()->Set_Position(XMVectorSet(72.f, 243.8f, 170.4f, 1.f));
			((CUFO*)DATABASE->Get_BossUFO())->Set_Active(true);
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 1)
	{
		if (25.02 < m_dTime && m_dTime <= 36.65)
		{
			pCody->Get_Actor()->Set_Position(XMVectorSet(74.7f, 244.5f, 167.5f, 1.f));
			pMay->Get_Actor()->Set_Position(XMVectorSet(73.3f, 243.8f, 168.7f, 1.f));
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 2)
	{
		if (36.65 < m_dTime && m_dTime <= 46.98)
		{
			pMay->Get_Actor()->Set_Position(XMVectorSet(73.5f,	243.8f,	168.7f, 1.f));
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 3)
	{
		if (36.65 < m_dTime && m_dTime <= 57.3)
		{
			pCody->Get_Actor()->Set_Position(XMVectorSet(74.7f,244.6f,167.5f,1.f));
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 4)
	{
		if (46.98 < m_dTime && m_dTime <= 64.5)
		{
			pMay->Get_Actor()->Set_Position(XMVectorSet(73.5f, 243.8f, 168.7f, 1.f));
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 5)
	{
		if (57.3 < m_dTime && m_dTime <= 64.5)
		{
			pCody->Get_Actor()->Set_Position(XMVectorSet(74.7f, 244.6f, 167.5f, 1.f));
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 6)
	{
		if (64.5 < m_dTime && m_dTime <= 71.5)
		{
			pCody->Get_Actor()->Set_Position(XMVectorSet(74.2f,244.4f,169.f,1.f));
			pMay->Get_Actor()->Set_Position(XMVectorSet(73.5f,243.8f,169.4f, 1.f));
			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 7)
	{
		if (m_dTime >= 74.0)
		{
			
			pCody->Get_Actor()->Set_Position(XMVectorSet(64.f, 244.4f, 220.6f, 1.f));
			pMay->Get_Actor()->Set_Position(XMVectorSet(65.5f, 244.4f, 220.4f, 1.f));

			m_iCutSceneTake++;
		}
	}
	else if (m_iCutSceneTake == 8)
	{
		if (m_dTime >= 78.5)
		{
			pCody->Get_Transform()->Set_RotateAxis(XMVectorSet(0.f,1.f,0.f,0.f),XMConvertToRadians(180.f));
			pMay->Get_Transform()->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
			
			m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), true);
		}
	}
	//ButtonPress 
	//31.32	Fourth
	//46.84 Third
	//64.77	Big
	if (m_bButtonPress[4] == false)
	{
		if (m_dTime >= 31.32f)
		{
			CPerformer* pButton = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(TEXT("Component_Model_ControlRoom_Keyboard_01_Button3")));
			pButton->ButtonDown();
			m_bButtonPress[3] = true;
			static_cast<CControlRoom_Monitor*>(DATABASE->Get_ControlRoomMonitor())->Change_Screen();
		}
	}
	if (m_bButtonPress[3] == false)
	{
		if (m_dTime >= 46.84f)
		{
			CPerformer* pButton = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(TEXT("Component_Model_ControlRoom_Keyboard_01_Button4")));
			pButton->ButtonDown();
			m_bButtonPress[3] = true;
			static_cast<CControlRoom_Monitor*>(DATABASE->Get_ControlRoomMonitor())->Change_Screen();
		}
	}
	if (m_bButtonPress[0] == false)	
	{
		if (m_dTime >= 64.77f)
		{
			CPerformer* pButton = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(TEXT("Component_Model_ControlRoom_Button_Large_01_Button")));
			pButton->ButtonDown();
			m_bButtonPress[0] = true;
		}
	}
	return true;
}

_bool CCutScene::Tick_CutScene_Eject_InUFO(_double dTimeDelta)
{
	Script_Eject_InUFO(dTimeDelta);
	return true;
}

_bool CCutScene::Tick_CutScene_GotoMoon(_double dTimeDelta)
{
	Script_GotoMoon(dTimeDelta);
	CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
	CMay* pMay = static_cast<CMay*>(DATABASE->GetMay());
	CMoonBaboon* pMoonBaboon = static_cast<CMoonBaboon*>(DATABASE->Get_MoonBaboon());
	CUFO* pUfo = static_cast<CUFO*>(DATABASE->Get_BossUFO());
	if (m_dTime < 1.0)
	{
		_matrix matUfo =
			XMMatrixRotationRollPitchYaw(XMConvertToRadians(-90.f), XMConvertToRadians(-90.f), 0.f)*
			XMMatrixScaling(95.f, 95.f, 95.f)*
			pUfo->Get_Model()->Get_BoneMatrix("Chair")* pUfo->Get_Transform()->Get_WorldMatrix();
		matUfo.r[3] = XMVectorSetY(matUfo.r[3], XMVectorGetY(matUfo.r[3])+2.f);
		pMoonBaboon->Get_Transform()->Set_WorldMatrix(matUfo);
	}
	if (m_dTime > 7.6)
	{
		_matrix matUfo = 
			XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(90.f), XMConvertToRadians(270.f))*
			XMMatrixScaling(100.f, 100.f, 100.f)* pUfo->Get_Model()->Get_BoneMatrix("Align")
			*pUfo->Get_Transform()->Get_WorldMatrix() ;
		matUfo.r[3] = XMVectorSetY(matUfo.r[3], XMVectorGetY(matUfo.r[3]));
		pMay->Get_Transform()->Set_WorldMatrix(matUfo);
	}
	if (m_dTime > 24.31)
	{
		_matrix matUfo =
			XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(90.f), XMConvertToRadians(90.f))*
			XMMatrixScaling(100.f, 100.f, 100.f)* pUfo->Get_Model()->Get_BoneMatrix("Chair")
			*pUfo->Get_Transform()->Get_WorldMatrix();
		matUfo.r[3] = XMVectorSetY(matUfo.r[3], XMVectorGetY(matUfo.r[3]) + 2.f);
		pMay->Get_Transform()->Set_WorldMatrix(matUfo);
	}

	if (m_dTime >= 24.31&&m_dTime < 24.31 + dTimeDelta)
	{
		pCody->Get_Actor()->Set_Position(XMVectorSet(64.0674942f, 601.063843f + 0.14f, 1011.77844f - 0.04f - 0.5f,1.f));
		pUfo->Get_Transform()->Set_WorldMatrix(
			MakeRollPitchYawMatrix({ 75.f,105.f,199.f }, { 1.f, 1.f,1.f }, { 10.f,0.f,0.f }));
		pMoonBaboon->Get_Transform()->Set_WorldMatrix(MakeRollPitchYawMatrix({ 88.f, 753.5f, 202.f }, { 1.f,1.f,1.f }, { -90.f, -90.f, 0.f }));
		pMoonBaboon->Set_CutSceneAnimation(Z_Moon_Eject_NoneMove,730.0);

	}

	
	return true;
}

_bool CCutScene::Tick_CutScene_Outro(_double dTimeDelta)
{
	Script_Outro(dTimeDelta);
	CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
	CMay* pMay = static_cast<CMay*>(DATABASE->GetMay());
	if (m_dTime >= 7.5 && m_dTime < 11.5f)
		CodyMove_EjectUfo(dTimeDelta);
	if (m_dTime >= 11.38 && m_dTime < 11.38+ dTimeDelta)
	{
		CFilm* pFilm = static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Get_CamHelper()->Get_Film(L"Film_Outro");
		pFilm->Set_Up(XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), -0.3f).r[1]);
		_matrix matWorld = MakeRollPitchYawMatrix(_float3(80.f, 758.f, 205.f), _float3(1.f, 1.f, 1.f), _float3(90.f, 0.f, 30.f));
		pCody->Get_Transform()->Set_WorldMatrix(matWorld);
		pCody->Get_Actor()->Set_Position(XMVectorSet(80.f, 758.f, 205.f, 1.f));
		pMay->Get_Transform()->Set_WorldMatrix(matWorld);
		pMay->Get_Actor()->Set_Position(XMVectorSet(80.f, 758.f, 205.f, 1.f)); 
	}

	return true;
}

HRESULT CCutScene::Start_CutScene()
{
	if (m_eCutSceneOption != CutSceneOption::CutScene_Eject_InUFO)
	{
		m_dTime = 0.0;
		m_iCutSceneTake = 0;
		UI_CreateOnlyOnce(Cody, CutSceneBar);
		UI_CreateOnlyOnce(May, CutSceneBar);

		UI_Generator->Set_AllActivation(false);
		
	}
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
	case CutSceneOption::CutScene_Clear_Umbrella:
		if (FAILED(Start_CutScene_Clear_Umbrella()))
			return E_FAIL;
		break;
	case CutSceneOption::CutScene_Clear_Rail:
		if (FAILED(Start_CutScene_Clear_Rail()))
			return E_FAIL;
		break;
	case CutSceneOption::CutScene_Boss_Intro:
		if (FAILED(Start_CutScene_Boss_Intro()))
			return E_FAIL;
		break;
	case CutSceneOption::CutScene_Eject_InUFO:
		if (FAILED(Start_CutScene_Eject_UFO()))
			return E_FAIL;
		break;
	case CutSceneOption::CutScene_GotoMoon:
		if (FAILED(Start_CutScene_GotoMoon()))
			return E_FAIL;
		break;
	case CutSceneOption::CutScene_Outro:
		if (FAILED(Start_CutScene_Outro()))
			return E_FAIL;
		break;
	}

	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Intro()
{
	CSound_Manager::GetInstance()->Play_Sound(TEXT("CutScene01.wav"), CHANNEL_CUTSCENE, 1.f, false);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_Position(XMVectorSet(0.f, -100.f, -100.f, 1.f));
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_Position(XMVectorSet(0.f, -100.f, -100.f, 1.f));

	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_ZeroGravity(true, true, true);
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_ZeroGravity(true, true, true);

	_double dTime = 0.0;

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
	static_cast<CMoonBaboon*>(pPerformer)->Get_Model()->Set_Animation(Moon_BeamUp_Intro);

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

	pPerformer = m_pCutScenePlayer->Find_Performer(L"Component_Model_SizeBeltRemoteControllerCutScene1");
	if (nullptr == pPerformer)
		return E_FAIL;
	static_cast<CPerformer*>(pPerformer)->Set_PerformerDesc(tDesc);
	static_cast<CPerformer*>(pPerformer)->Start_Perform(0, dTime);

	EFFECT->Add_Effect_CutScene_Timer(5.26f, Effect_Value_CutScene::MoonBaboon_Land);


	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Active_GravityPath_01()
{
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_ZeroGravity(true, true, true);
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_ZeroGravity(true, true, true);
	m_bIsStartFilm = false;
	CCam_Helper* pCamHelper = static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Get_CamHelper();
	if (nullptr == pCamHelper)
		return E_FAIL;
	CFilm::CamNode* pFirstNode = pCamHelper->Get_Film(TEXT("Film_Active_GravityPath_01"))->Get_CamNodes()->front();
	pCamHelper->SeeCamNode(pFirstNode, CFilm::RScreen);
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), true, 1.f);
	
	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Clear_Umbrella()
{
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_ZeroGravity(true, true, true);
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_ZeroGravity(true, true, true);
	m_bIsStartFilm = false;
	CCam_Helper* pCamHelper = static_cast<CMainCamera*>(CDataStorage::GetInstance()->Get_MainCam())->Get_CamHelper();
	if (nullptr == pCamHelper)
		return E_FAIL;
	CFilm::CamNode* pFirstNode = pCamHelper->Get_Film(TEXT("Film_Clear_Umbrella"))->Get_CamNodes()->front();
	pCamHelper->SeeCamNode(pFirstNode, CFilm::LScreen);
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 0.f, 1.f, 1.f), true, 1.f);
	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Clear_Rail()
{
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_ZeroGravity(true, true, true);
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_ZeroGravity(true, true, true);
	m_bIsStartFilm = false;
	CCam_Helper* pCamHelper = static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Get_CamHelper();
	if (nullptr == pCamHelper)
		return E_FAIL;
	CFilm::CamNode* pFirstNode = pCamHelper->Get_Film(TEXT("Film_Clear_Rail"))->Get_CamNodes()->front();
	pCamHelper->SeeCamNode(pFirstNode, CFilm::RScreen);
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), true, 1.f);
	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Boss_Intro()
{
	CGameInstance::GetInstance()->Sound_FadeOut(CHANNEL_BGM, 0.f, 1.f);
	static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Start_Film(L"Film_Boss_Intro");
	static_cast<CMainCamera*>(CDataStorage::GetInstance()->Get_MainCam())->Start_Film(L"Film_Boss_Intro");
	m_bIsStartFilm = false;
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), false);

	CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
	CMay* pMay = static_cast<CMay*>(DATABASE->GetMay());

	pCody->Get_Actor()->Set_ZeroGravity(true, true, true);
	pMay->Get_Actor()->Set_ZeroGravity(true, true, true);


	pCody->Get_Transform()->Set_WorldMatrix(XMMatrixIdentity()/*matRot*/);
	pCody->Get_Actor()->Set_Position(XMVectorSet(73.5f, 244.5f, 168.5f, 1.f));
	pCody->Get_Model()->Set_Animation(ANI_C_CutScene_BossFight_Intro);

	pMay->Get_Transform()->Set_WorldMatrix(XMMatrixIdentity()/*matRot*/);
	pMay->Get_Actor()->Set_Position(XMVectorSet(72.f,243.8f,170.4f, 1.f));
	pMay->Get_Model()->Set_Animation(ANI_M_SpaceStation_BossFight_Intro);

	CUFO* pUfo = static_cast<CUFO*>(DATABASE->Get_BossUFO());
	pUfo->Get_Model()->Set_Animation(CutScene_UFO_Boss_Intro);
	
	CPerformer* pButton = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(TEXT("Component_Model_ControlRoom_Keyboard_01_Button1")));
	pButton->Set_Position(_float3(62.457f, 219.084f, 238.85f));

	pButton = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(TEXT("Component_Model_ControlRoom_Keyboard_01_Button2")));
	pButton->Set_Position(_float3(63.33f, 219.131f, 239.23f));

	pButton = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(TEXT("Component_Model_ControlRoom_Keyboard_01_Button3")));
	pButton->Set_Position(_float3(64.33f, 219.131f, 239.23f));

	pButton = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(TEXT("Component_Model_ControlRoom_Keyboard_01_Button4")));
	pButton->Set_Position(_float3(65.2f, 219.084f, 238.85f));

	pButton = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(TEXT("Component_Model_ControlRoom_Button_Large_01_Button")));
	pButton->Set_Position(_float3(60.4f, 218.652f, 238.221f));

	EFFECT->Add_Effect_CutScene_Timer(64.77f, Effect_Value_CutScene::Levitation_Beam);
	EFFECT->Add_Effect_CutScene_Timer(64.77f, Effect_Value_CutScene::Levitation_Beam);

	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Eject_UFO()
{
	((CCody*)DATABASE->GetCody())->Get_Actor()->Set_Position(XMVectorSet(67.3511f, 599.567f, 1002.51f, 1.f));
	((CCody*)DATABASE->GetCody())->Get_Actor()->Set_IsPlayerInUFO(true);
	((CCody*)DATABASE->GetCody())->Set_PlayerSizeSmall_INUFO();

	// 우주선 들어가는거
	CGameInstance::GetInstance()->Set_MainViewFog(true);
	CGameInstance::GetInstance()->Set_GoalViewportInfo(XMVectorSet(0.0f, 0.f, 0.6f, 1.f), XMVectorSet(0.6f, 0.f, 0.4f, 1.f));
	static_cast<CMainCamera*>(CDataStorage::GetInstance()->Get_MainCam())->Start_Film(L"Film_Eject_InUFO");
	UI_Generator->Set_AllActivation(false);

	return S_OK;
}

HRESULT CCutScene::Start_CutScene_GotoMoon()
{
	CSound_Manager::GetInstance()->Play_Sound(TEXT("CutScene03.wav"), CHANNEL_CUTSCENE, 1.f, false);
	static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Start_Film(L"Film_GotoMoon");
	
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f),false);

	CCody* pCody = ((CCody*)DATABASE->GetCody());
	CMay* pMay = ((CMay*)DATABASE->GetMay());

	pCody->Get_Actor()->Set_ZeroGravity(true, true, true);
	pCody->Get_Transform()->Set_WorldMatrix(XMMatrixScaling(0.1f,0.1f,0.1f));
	pCody->Get_Actor()->Set_Position(XMVectorSet(64.0174942f + 0.04f, 601.063843f + 0.076f,1011.04f, 1.f));
	pCody->Get_Model()->Set_Animation(ANI_C_CutScene_BossFight_Eject);

	pMay->Get_Actor()->Set_ZeroGravity(true, true, true);
	pMay->Get_Transform()->Set_WorldMatrix(MakeRollPitchYawMatrix({ 64.f,345.f,195.f}, { 1.f,1.f,1.f }, {90.f,0.f,0.f}));
	pMay->Get_Actor()->Set_Position(XMVectorSet(64.f, 345.f+ 98.3895f, 195.f, 1.f));
	pMay->Get_Model()->Set_Animation(ANI_M_CutScene_SpaceStation_BossFight_Eject);

	CUFO* pUfo = static_cast<CUFO*>(DATABASE->Get_BossUFO());
	pUfo->Get_Transform()->Set_WorldMatrix(MakeRollPitchYawMatrix(
		_float3(64.f, 357.5f + 98.3895f, 195.f), _float3(1.f, 1.f, 1.f), _float3(90.f, 0.f, 0.f)));
	pUfo->Get_Model()->Set_Animation(CutScene_Eject_FlyingSaucer);
	pUfo->Set_Active(true);

	CMoonBaboon* pMoonBaboon = static_cast<CMoonBaboon*>(DATABASE->Get_MoonBaboon());
	pMoonBaboon->Set_Animation(Moon_Eject, Moon_Ufo_MH);
	pMoonBaboon->Set_Active(true);
	return S_OK;
}

HRESULT CCutScene::Start_CutScene_Outro()
{
	CSound_Manager::GetInstance()->Sound_FadeOut(CHANNEL_BGM, 0.f, 1.f);
	CSound_Manager::GetInstance()->Sound_FadeOut(CHANNEL_BGM2, 0.f, 1.f);

	CSound_Manager::GetInstance()->Play_Sound(TEXT("CutScene04.wav"), CHANNEL_CUTSCENE, 1.f, false);
	m_bCodyEjectUFO = false;
	m_bIsStartFilm = false;
	m_fCodyMove_EjectUfoTime = 0.f;
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), false);

	CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
	CMay* pMay = static_cast<CMay*>(DATABASE->GetMay());
	
	pCody->Get_Actor()->Set_ZeroGravity(true, true, true);
	pMay->Get_Actor()->Set_ZeroGravity(true, true, true);
	
	static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Start_Film(L"Film_Outro");
	
	CFilm* pFilm = static_cast<CSubCamera*>(CDataStorage::GetInstance()->Get_SubCam())->Get_CamHelper()->Get_Film(L"Film_Outro");
	pFilm->Set_Up(XMVectorSet(0.f,1.f,0.f,0.f));
	
	_matrix matRot = XMMatrixRotationRollPitchYaw( XMConvertToRadians(4.f), 0.f, XMConvertToRadians(9.f));
	pCody->Get_Transform()->Set_WorldMatrix(XMMatrixScaling(0.1f, 0.1f, 0.1f) *matRot);
	pCody->Get_Actor()->Set_Position(XMVectorSet(30.f, 758.f, 203.f, 1.f));
	pCody->Get_Model()->Set_Animation(ANI_C_CutScene_BossFight_Outro);

	pMay->Get_Transform()->Set_WorldMatrix(matRot);
	pMay->Get_Actor()->Set_Position(XMVectorSet(30.f,758.f,203.f, 1.f));
	pMay->Get_Model()->Set_Animation(ANI_M_SpaceStation_BossFight_Outro);

	CMoonBaboon* pMoonBaboon = static_cast<CMoonBaboon*>(DATABASE->Get_MoonBaboon());
	pMoonBaboon->Get_Transform()->Set_WorldMatrix(MakeRollPitchYawMatrix(_float3(80.f,758.f,205.f),_float3(1.f,1.f,1.f),_float3(-90.f, 270.f, -30.f)));
	pMoonBaboon->Set_Animation(CutScene_Outro_MoonBaboon, Moon_Ufo_MH);

	CPerformer* pRemoteController = static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(TEXT("Component_Model_RemoteContollerCutScene1")));
	

	pRemoteController->Set_Position({ 80.f,758.f,205.f });
	pRemoteController->Set_Rotation({ -90.f,270.f,-30.f });
	pRemoteController->Start_Perform(1);

	CUFO* pUfo = static_cast<CUFO*>(DATABASE->Get_BossUFO());
	pUfo->Get_Transform()->Set_WorldMatrix(MakeRollPitchYawMatrix(_float3(30.f, 758.f, 203.f), _float3(1.f, 1.f, 1.f), _float3(0.f, 9.f, 4.f)));
	pUfo->Get_Model()->Set_Animation(CutScene_UFO_Outro);

	EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::MoonBaboon_FallDown_Smoke);

	return S_OK;
}

HRESULT CCutScene::End_CutScene_Intro()
{
	CGameInstance::GetInstance()->Play_Sound(TEXT("Bgm_Main.wav"), CHANNEL_BGM, 0.f, true);
	CGameInstance::GetInstance()->Sound_FadeIn(CHANNEL_BGM, 0.15f, 3.f);

	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), true, 1.f);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_ZeroGravity(false, false, false);
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_ZeroGravity(false, false, false);
	CMainCamera* pMainCam = static_cast<CMainCamera*>(DATABASE->Get_MainCam());
	pMainCam->ReSet_Cam_FreeToAuto(false, false, 1.f);
	CSubCamera* pSubCam = static_cast<CSubCamera*>(DATABASE->Get_SubCam());
	pSubCam->ReSet_Cam_FreeToAuto(false, false, 1.f);
	UI_Generator->Set_AllActivation(true);
	((CUFO*)DATABASE->Get_BossUFO())->Set_Active(false);
	static_cast<CPerformer*>(m_pCutScenePlayer->Find_Performer(L"Component_Model_RemoteContollerCutScene1"))->Set_Position({ 0.f,0.f,0.f });
	
	return S_OK;
}

HRESULT CCutScene::End_CutScene_Active_GravityPath_01()
{
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), true, 1.f);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_ZeroGravity(false, false, false);
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_ZeroGravity(false, false, false);
	CMainCamera* pMainCam = static_cast<CMainCamera*>(DATABASE->Get_MainCam());
	pMainCam->ReSet_Cam_FreeToAuto();
	CSubCamera* pSubCam = static_cast<CSubCamera*>(DATABASE->Get_SubCam());
	pSubCam->ReSet_Cam_FreeToAuto();
	UI_Generator->Set_AllActivation(true);

	return S_OK;
}

HRESULT CCutScene::End_CutScene_Clear_Umbrella()
{
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), true, 1.f);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_ZeroGravity(false, false, false);
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_ZeroGravity(false, false, false);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_IsPlayerSizeSmall(false);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Get_Controller()->setStepOffset(0.707f);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Get_Controller()->setSlopeLimit(0.5f);
	CMainCamera* pMainCam = static_cast<CMainCamera*>(DATABASE->Get_MainCam());
	pMainCam->ReSet_Cam_FreeToAuto(true, false , 1.f);
	CSubCamera* pSubCam = static_cast<CSubCamera*>(DATABASE->Get_SubCam());
	pSubCam->ReSet_Cam_FreeToAuto(true, false, 1.f);
	pSubCam->Get_Transform()->Set_WorldMatrix(pMainCam->Get_Transform()->Get_WorldMatrix());
	UI_Generator->Set_AllActivation(true);

	return S_OK;
}

HRESULT CCutScene::End_CutScene_Clear_Rail()
{
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), true, 1.f);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_ZeroGravity(false, false, false);
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_ZeroGravity(false, false, false);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_IsPlayerSizeSmall(false);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Get_Controller()->setStepOffset(0.707f);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Get_Controller()->setSlopeLimit(0.5f);
	CMainCamera* pMainCam = static_cast<CMainCamera*>(DATABASE->Get_MainCam());
	pMainCam->ReSet_Cam_FreeToAuto(true, false, 1.f);
	CSubCamera* pSubCam = static_cast<CSubCamera*>(DATABASE->Get_SubCam());
	pSubCam->ReSet_Cam_FreeToAuto(true, false, 1.f);
	pMainCam->Get_Transform()->Set_WorldMatrix(pSubCam->Get_Transform()->Get_WorldMatrix());
	UI_Generator->Set_AllActivation(true);

	return S_OK;
}

HRESULT CCutScene::End_CutScene_Boss_Intro()
{
	CGameInstance::GetInstance()->Play_Sound(TEXT("Bgm_Boss.wav"), CHANNEL_BGM, 0.f, true);
	CGameInstance::GetInstance()->Sound_FadeIn(CHANNEL_BGM, 0.15f, 3.f);

	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_IsPlayerSizeSmall(false);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_ZeroGravity(false, false, false);
	static_cast<CMay*>(DATABASE->GetMay())->Get_Actor()->Set_ZeroGravity(false, false, false);
	/* 상태 초기화 */
	static_cast<CCody*>(DATABASE->GetCody())->Enforce_IdleState();
	static_cast<CMay*>(DATABASE->GetMay())->Enforce_IdleState();
	//static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Set_Gravity_Normally();
	static_cast<CCody*>(DATABASE->GetCody())->Set_Change_Size_After_UmbrellaCutScene();
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Get_Controller()->setStepOffset(0.707f);
	static_cast<CCody*>(DATABASE->GetCody())->Get_Actor()->Get_Controller()->setSlopeLimit(0.5f);
	/* 상태 초기화 */
	CMainCamera* pMainCam = static_cast<CMainCamera*>(DATABASE->Get_MainCam());
	pMainCam->ReSet_Cam_FreeToAuto(true, false, 1.f);
	CSubCamera* pSubCam = static_cast<CSubCamera*>(DATABASE->Get_SubCam());
	pSubCam->ReSet_Cam_FreeToAuto(true, false, 1.f);
	UI_Generator->Set_AllActivation(true);

	((CUFO*)DATABASE->Get_BossUFO())->Set_EndIntroCutScene();

	return S_OK;
}

HRESULT CCutScene::End_CutScene_Eject_InUFO()
{
	return S_OK;
}

HRESULT CCutScene::End_CutScene_GotoMoon()
{
	CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
	CMay* pMay = static_cast<CMay*>(DATABASE->GetMay());
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), true);
	pCody->Get_Actor()->Set_ZeroGravity(false, false, false);
	pMay->Get_Actor()->Set_ZeroGravity(false, false, false);
	
	pCody->Set_InJoyStick();
	pMay->Set_UFO(true);

	return S_OK;
}

HRESULT CCutScene::End_CutScene_Outro()
{
	m_pCutScenePlayer->Set_ViewPort(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), true, 1.f);

	m_pCutScenePlayer->Set_IsEndingCredit(true);
	
	return S_OK;
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

HRESULT CCutScene::Ready_CutScene_Clear_Umbrella()
{
	m_dDuration = 8.3;
	return S_OK;
}

HRESULT CCutScene::Ready_CutScene_Clear_Rail()
{
	m_dDuration = 8.0;
	return S_OK;
}

HRESULT CCutScene::Ready_CutScene_Boss_Intro()
{
	m_dDuration = 80.6;
	return S_OK;
}

HRESULT CCutScene::Ready_CutScene_Eject_InUFO()
{
	m_dDuration = 7.0;
	return S_OK;
}

HRESULT CCutScene::Ready_CutScene_GotoMoon()
{
	m_dDuration = 36.13;
	return S_OK;
}

HRESULT CCutScene::Ready_CutScene_Outro()
{
	m_dDuration = 62.89;
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
	case Client::CCutScene::CutSceneOption::CutScene_Clear_Umbrella:
		if (FAILED(Ready_CutScene_Clear_Umbrella()))
			return E_FAIL;
		break;
	case Client::CCutScene::CutSceneOption::CutScene_Clear_Rail:
		if (FAILED(Ready_CutScene_Clear_Rail()))
			return E_FAIL;
		break;
	case Client::CCutScene::CutSceneOption::CutScene_Boss_Intro:
		if (FAILED(Ready_CutScene_Boss_Intro()))
			return E_FAIL;
		break;
	case Client::CCutScene::CutSceneOption::CutScene_Eject_InUFO:
		if (FAILED(Ready_CutScene_Eject_InUFO()))
			return E_FAIL;
		break;
	case Client::CCutScene::CutSceneOption::CutScene_GotoMoon:
		if (FAILED(Ready_CutScene_GotoMoon()))
			return E_FAIL;
		break;
	case Client::CCutScene::CutSceneOption::CutScene_Outro:
		if (FAILED(Ready_CutScene_Outro()))
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
	pCody->Get_Actor()->Set_Position(XMVectorSetW(vCurPos,1.f));

	if (m_iCodyLerpCount == m_iMaxCodyLerpCount)
	{
		pCody->Get_Transform()->Set_Scale(XMLoadFloat3(&m_vTargetCodyScale));
		pCody->Set_Position(XMVectorSetW(XMLoadFloat3(&m_vTargetCodyPos), 1.f));

		m_bStartLerpCody = false;
	}
}

void CCutScene::CodyMove_EjectUfo(_double dTimeDelta)
{
	if (m_bCodyEjectUFO)
		return;
	CCody* pCody = static_cast<CCody*>(DATABASE->GetCody());
	if (m_dTime < 10.32)
	{
		_float fStartTime = 7.5f;
		_float fEndTime = 10.32f;
		m_fCodyMove_EjectUfoTime += (_float)dTimeDelta/(fEndTime - fStartTime) ;
		_fvector vStartPos = XMVectorSet(66.f, 754.05f, 219.f, 1.f);
		_fvector vMoveToPos = XMVectorSet(63.f, 754.05f, 219.f, 1.f);

		pCody->Get_Actor()->Set_Position(XMVectorLerp(vStartPos,vMoveToPos,m_fCodyMove_EjectUfoTime));
	}
	else
	{
		EFFECT->Add_Effect_CutScene_Timer(0.1f, Effect_Value_CutScene::Cody_Size_SM);
		m_bCodyEjectUFO = true;
		pCody->Get_Transform()->Set_WorldMatrix(XMMatrixRotationRollPitchYaw(XMConvertToRadians(4.f), 0.f, XMConvertToRadians(9.f)));
		pCody->Get_Actor()->Set_Position(XMVectorSet(29.f,757.4f,203.f,1.f));
	}
}

void CCutScene::Script_Intro(_double dTimeDelta)
{
	_double dSoundTime = m_dTime;
	if (dSoundTime >= 1.5 && dSoundTime < 1.5 + dTimeDelta)
		SCRIPT->Render_Script(123, CScript::SCREEN::FULL, 0.8f);
	else if (dSoundTime >= 2.5 && dSoundTime < 2.5 + dTimeDelta)
		SCRIPT->Render_Script_DoubleLine(124, 125, 1.4f);
	else if (dSoundTime >= 8.0 && dSoundTime < 8.0 + dTimeDelta)
		SCRIPT->Render_Script(126, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 9.0 && dSoundTime < 9.0 + dTimeDelta)
		SCRIPT->Render_Script(127, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 11.0 && dSoundTime < 11.0 + dTimeDelta)
		SCRIPT->Render_Script(128, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 14.0 && dSoundTime < 14.0 + dTimeDelta)
		SCRIPT->Render_Script(129, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 15.0 && dSoundTime < 15.0 + dTimeDelta)
		SCRIPT->Render_Script(130, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 17.0 && dSoundTime < 17.0 + dTimeDelta)
		SCRIPT->Render_Script(131, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 19.0 && dSoundTime < 19.0 + dTimeDelta)
		SCRIPT->Render_Script(132, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 22.5 && dSoundTime < 22.5 + dTimeDelta)
		SCRIPT->Render_Script(133, CScript::SCREEN::FULL, 2.4f);
	else if (dSoundTime >= 26.0 && dSoundTime < 26.0 + dTimeDelta)
		SCRIPT->Render_Script(134, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 28.0 && dSoundTime < 28.0 + dTimeDelta)
		SCRIPT->Render_Script(135, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 30.0 && dSoundTime < 30.0 + dTimeDelta)
		SCRIPT->Render_Script(136, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 32.0 && dSoundTime < 32.0 + dTimeDelta)
		SCRIPT->Render_Script(137, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 36.0 && dSoundTime < 36.0 + dTimeDelta)
		SCRIPT->Render_Script(138, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 38.0 && dSoundTime < 38.0 + dTimeDelta)
		SCRIPT->Render_Script(139, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 40.5 && dSoundTime < 40.5 + dTimeDelta)
		SCRIPT->Render_Script(140, CScript::SCREEN::FULL, 1.4f);
	else if (dSoundTime >= 43.0 && dSoundTime < 43.0 + dTimeDelta)
		SCRIPT->Render_Script(141, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 44.0 && dSoundTime < 44.0 + dTimeDelta)
		SCRIPT->Render_Script(142, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 46.0 && dSoundTime < 46.0 + dTimeDelta)
		SCRIPT->Render_Script(143, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 48.0 && dSoundTime < 48.0 + dTimeDelta)
		SCRIPT->Render_Script(144, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 50.0 && dSoundTime < 50.0 + dTimeDelta)
		SCRIPT->Render_Script(145, CScript::SCREEN::FULL, 2.9f);
	else if (dSoundTime >= 53.0 && dSoundTime < 53.0 + dTimeDelta)
		SCRIPT->Render_Script(146, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 61.0 && dSoundTime < 61.0 + dTimeDelta)
		SCRIPT->Render_Script(147, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 63.0 && dSoundTime < 63.0 + dTimeDelta)
		SCRIPT->Render_Script(148, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 66.0 && dSoundTime < 66.0 + dTimeDelta)
		SCRIPT->Render_Script(149, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 68.0 && dSoundTime < 68.0 + dTimeDelta)
		SCRIPT->Render_Script(150, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 71.0 && dSoundTime < 71.0 + dTimeDelta)
		SCRIPT->Render_Script(151, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 74.0 && dSoundTime < 74.0 + dTimeDelta)
		SCRIPT->Render_Script(152, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 76.0 && dSoundTime < 76.0 + dTimeDelta)
		SCRIPT->Render_Script(153, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 78.0 && dSoundTime < 78.0 + dTimeDelta)
		SCRIPT->Render_Script(154, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 79.0 && dSoundTime < 79.0 + dTimeDelta)
		SCRIPT->Render_Script(155, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 80.0 && dSoundTime < 80.0 + dTimeDelta)
		SCRIPT->Render_Script(156, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 81.0 && dSoundTime < 81.0 + dTimeDelta)
		SCRIPT->Render_Script(157, CScript::SCREEN::FULL, 2.9f);
	else if (dSoundTime >= 84.0 && dSoundTime < 84.0 + dTimeDelta)
		SCRIPT->Render_Script(158, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 85.0 && dSoundTime < 85.0 + dTimeDelta)
		SCRIPT->Render_Script(159, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 92.0 && dSoundTime < 92.0 + dTimeDelta)
		SCRIPT->Render_Script(160, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 94.0 && dSoundTime < 94.0 + dTimeDelta)
		SCRIPT->Render_Script(161, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 95.0 && dSoundTime < 95.0 + dTimeDelta)
		SCRIPT->Render_Script(162, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 97.0 && dSoundTime < 97.0 + dTimeDelta)
		SCRIPT->Render_Script(163, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 98.0 && dSoundTime < 98.0 + dTimeDelta)
		SCRIPT->Render_Script(164, CScript::SCREEN::FULL, 1.4f);
	else if (dSoundTime >= 100.0 && dSoundTime < 100.0 + dTimeDelta)
		SCRIPT->Render_Script(165, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 102.0 && dSoundTime < 102.0 + dTimeDelta)
		SCRIPT->Render_Script(166, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 104.0 && dSoundTime < 104.0 + dTimeDelta)
		SCRIPT->Render_Script(167, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 106.0 && dSoundTime < 106.0 + dTimeDelta)
		SCRIPT->Render_Script(168, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 108.0 && dSoundTime < 108.0 + dTimeDelta)
		SCRIPT->Render_Script(169, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 110.5 && dSoundTime < 110.5 + dTimeDelta)
		SCRIPT->Render_Script(170, CScript::SCREEN::FULL, 1.4f);
	else if (dSoundTime >= 112.0 && dSoundTime < 112.0 + dTimeDelta)
		SCRIPT->Render_Script(171, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 113.0 && dSoundTime < 113.0 + dTimeDelta)
		SCRIPT->Render_Script(172, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 114.0 && dSoundTime < 114.0 + dTimeDelta)
		SCRIPT->Render_Script(173, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 116.0 && dSoundTime < 116.0 + dTimeDelta)
		SCRIPT->Render_Script(174, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 122.5 && dSoundTime < 122.5 + dTimeDelta)
		SCRIPT->Render_Script(175, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 125.0 && dSoundTime < 125.0 + dTimeDelta)
		SCRIPT->Render_Script(176, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 132.0 && dSoundTime < 132.0 + dTimeDelta)
		SCRIPT->Render_Script(177, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 134.0 && dSoundTime < 134.0 + dTimeDelta)
		SCRIPT->Render_Script(178, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 136.0 && dSoundTime < 136.0 + dTimeDelta)
		SCRIPT->Render_Script(179, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 138.0 && dSoundTime < 138.0 + dTimeDelta)
		SCRIPT->Render_Script(180, CScript::SCREEN::FULL, 1.9f);


}

void CCutScene::Script_Boss_Intro(_double dTimeDelta)
{
	_double dSoundTime = m_dTime - 1.5;
	if (dSoundTime >= 11.3 && dSoundTime < 11.3 + dTimeDelta)
		SCRIPT->Render_Script_DoubleLine(181, 182, 2.5f);
	else if (dSoundTime >= 14.5 && dSoundTime < 14.5 + dTimeDelta)
		SCRIPT->Render_Script(183, CScript::SCREEN::FULL, 3.f);
	else if (dSoundTime >= 18.0 && dSoundTime < 18.0 + dTimeDelta)
		SCRIPT->Render_Script(184, CScript::SCREEN::FULL, 1.5f);
	else if(dSoundTime >= 21.0 && dSoundTime < 21.0 + dTimeDelta)
		SCRIPT->Render_Script(185, CScript::SCREEN::FULL, 2.f);
	else if(dSoundTime >= 24.0 && dSoundTime < 24.0 + dTimeDelta)
		SCRIPT->Render_Script(186, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 26.0 && dSoundTime < 26.0 + dTimeDelta)
		SCRIPT->Render_Script(187, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 30.5 && dSoundTime < 30.5 + dTimeDelta)
		SCRIPT->Render_Script(188, CScript::SCREEN::FULL, 1.4f);
	else if (dSoundTime >= 32.0 && dSoundTime < 32.0 + dTimeDelta)
		SCRIPT->Render_Script(189, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 33.0 && dSoundTime < 33.0 + dTimeDelta)
		SCRIPT->Render_Script(190, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 34.0 && dSoundTime < 34.0 + dTimeDelta)
		SCRIPT->Render_Script(191, CScript::SCREEN::FULL, 2.f);
	else if (dSoundTime >= 37.0 && dSoundTime < 37.0 + dTimeDelta)
		SCRIPT->Render_Script(192, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 39.0 && dSoundTime < 39.0 + dTimeDelta)
		SCRIPT->Render_Script(193, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 41.0 && dSoundTime < 41.0 + dTimeDelta)
		SCRIPT->Render_Script(194, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 46.0 && dSoundTime < 46.0 + dTimeDelta)
		SCRIPT->Render_Script(195, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 48.0 && dSoundTime < 48.0 + dTimeDelta)
		SCRIPT->Render_Script(196, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 50.0 && dSoundTime < 50.0 + dTimeDelta)
		SCRIPT->Render_Script(197, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 52.0 && dSoundTime < 52.0 + dTimeDelta)
		SCRIPT->Render_Script(198, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 53.0 && dSoundTime < 53.0 + dTimeDelta)
		SCRIPT->Render_Script(199, CScript::SCREEN::FULL, 0.4f);
	else if (dSoundTime >= 53.5 && dSoundTime < 53.5 + dTimeDelta)
		SCRIPT->Render_Script(200, CScript::SCREEN::FULL, 1.4f);
	else if (dSoundTime >= 56.0 && dSoundTime < 56.0 + dTimeDelta)
		SCRIPT->Render_Script(201, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 57.0 && dSoundTime < 57.0 + dTimeDelta)
		SCRIPT->Render_Script(202, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 58.0 && dSoundTime < 58.0 + dTimeDelta)
		SCRIPT->Render_Script(203, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 60.0 && dSoundTime < 60.0 + dTimeDelta)
		SCRIPT->Render_Script(204, CScript::SCREEN::FULL, 2.f);
	else if (dSoundTime >= 64.0 && dSoundTime < 64.0 + dTimeDelta)
		SCRIPT->Render_Script(205, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 65.0 && dSoundTime < 65.0 + dTimeDelta)
		SCRIPT->Render_Script(206, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 66.0 && dSoundTime < 66.0 + dTimeDelta)
		SCRIPT->Render_Script(207, CScript::SCREEN::FULL, 0.9f);
	else if (dSoundTime >= 67.0 && dSoundTime < 67.0 + dTimeDelta)
		SCRIPT->Render_Script(208, CScript::SCREEN::FULL, 2.f);
	else if (dSoundTime >= 72.0 && dSoundTime < 72.0 + dTimeDelta)
		SCRIPT->Render_Script(209, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 74.0 && dSoundTime < 74.0 + dTimeDelta)
		SCRIPT->Render_Script(210, CScript::SCREEN::FULL, 1.9f);
	else if (dSoundTime >= 77.0 && dSoundTime < 77.0 + dTimeDelta)
		SCRIPT->Render_Script(211, CScript::SCREEN::FULL, 1.f);
}

void CCutScene::Script_Eject_InUFO(_double dTimeDelta)
{
	_double dSoundTime = m_dTime;
	if (dSoundTime >= 1.0 && dSoundTime < 1.0 + dTimeDelta)
		SCRIPT->Render_Script(91, CScript::HALF, 1.0f);
	else if (dSoundTime >= 2.0 && dSoundTime < 2.0 + dTimeDelta)
		SCRIPT->Render_Script(92, CScript::HALF, 2.9f);
	else if (dSoundTime >= 5.0 && dSoundTime < 5.0 + dTimeDelta)
		SCRIPT->Render_Script(93, CScript::HALF, 2.0f);

}

void CCutScene::Script_GotoMoon(_double dTimeDelta)
{
	_double dSoundTime = m_dTime;
	if (dSoundTime >= 12.0 && dSoundTime < 12.0 + (_float)dTimeDelta)
		SCRIPT->Render_Script(212, CScript::FULL, 0.9f);
	else if (dSoundTime >= 13.0 && dSoundTime < 13.0 + (_float)dTimeDelta)
		SCRIPT->Render_Script(213, CScript::FULL, 1.9f);
	else if (dSoundTime >= 15.0 && dSoundTime < 15.0 + (_float)dTimeDelta)
		SCRIPT->Render_Script(214, CScript::FULL, 1.9f);
	else if (dSoundTime >= 17.0 && dSoundTime < 17.0 + (_float)dTimeDelta)
		SCRIPT->Render_Script(215, CScript::FULL, 1.9f);
	else if (dSoundTime >= 19.0 && dSoundTime < 19.0 + (_float)dTimeDelta)
		SCRIPT->Render_Script(216, CScript::FULL, 1.9f);
	else if (dSoundTime >= 21.0 && dSoundTime < 21.0 + (_float)dTimeDelta)
		SCRIPT->Render_Script(217, CScript::FULL, 1.9f);
	else if (dSoundTime >= 23.0 && dSoundTime < 23.0 + (_float)dTimeDelta)
		SCRIPT->Render_Script(218, CScript::FULL, 1.9f);
	else if (dSoundTime >= 25.0 && dSoundTime < 25.0 + (_float)dTimeDelta)
		SCRIPT->Render_Script(219, CScript::FULL, 1.9f);

}

void CCutScene::Script_Outro(_double dTimeDelta)
{
	_double dSoundTime = m_dTime;

	if (dSoundTime >= 0.8 && dSoundTime < 0.8 + dTimeDelta)
		EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::MoonBaboon_Land);
	if (dSoundTime >= 1.2 && dSoundTime < 1.2 + dTimeDelta)
		EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::MoonBaboon_Land);
	//else if (dSoundTime >= 4.4 && dSoundTime < 4.4 + dTimeDelta)
	//	EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::UFO_Land);
	else if (dSoundTime >= 9.9 && dSoundTime < 9.9 + dTimeDelta)
		EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::Cody_Size_SM);
	else if (dSoundTime >= 10.2 && dSoundTime < 10.2 + dTimeDelta)
		EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::Cody_Size_SM);
	else if (dSoundTime >= 10.5 && dSoundTime < 10.5 + dTimeDelta)
		EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::Cody_Size_SM);
	else if (dSoundTime >= 11.0 && dSoundTime < 11.0 + dTimeDelta)
	{
		EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::MoonBaboon_Land);
		EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::MoonBaboon_Land);
	}

	if(dSoundTime >= 12.0 && dSoundTime < 12.0 +dTimeDelta)
		SCRIPT->Render_Script(220, CScript::FULL, 0.9f);
	else if (dSoundTime >= 13.0 && dSoundTime < 13.0 + dTimeDelta)
		SCRIPT->Render_Script(221, CScript::FULL, 2.9f);
	else if (dSoundTime >= 16.0 && dSoundTime < 16.0 + dTimeDelta)
		SCRIPT->Render_Script(222, CScript::FULL, 1.9f);
	else if (dSoundTime >= 18.0 && dSoundTime < 18.0 + dTimeDelta)
		SCRIPT->Render_Script(223, CScript::FULL, 1.4f);
	else if (dSoundTime >= 19.5 && dSoundTime < 19.5+ dTimeDelta)
		SCRIPT->Render_Script(224, CScript::FULL, 2.4f);
	else if (dSoundTime >= 22.0 && dSoundTime < 22.0 + dTimeDelta)
		SCRIPT->Render_Script(225, CScript::FULL, 0.9f);
	else if (dSoundTime >= 23.0 && dSoundTime < 23.0 + dTimeDelta)
		SCRIPT->Render_Script(226, CScript::FULL, 1.9f);
	else if (dSoundTime >= 25.0 && dSoundTime < 25.0 + dTimeDelta)
		SCRIPT->Render_Script(227, CScript::FULL, 0.9f);
	else if (dSoundTime >= 27.0 && dSoundTime < 27.0 + dTimeDelta)
		SCRIPT->Render_Script_DoubleLine(228,229, 1.9f);
	else if (dSoundTime >= 29.0 && dSoundTime < 29.0 + dTimeDelta)
		SCRIPT->Render_Script(230, CScript::FULL, 0.9f);
	else if (dSoundTime >= 30.0 && dSoundTime < 30.0 + dTimeDelta)
		SCRIPT->Render_Script(231, CScript::FULL, 0.9f);
	else if (dSoundTime >= 31.0 && dSoundTime < 31.0 + dTimeDelta)
		SCRIPT->Render_Script(232, CScript::FULL, 0.9f);
	else if (dSoundTime >= 32.0 && dSoundTime < 32.0 + dTimeDelta)
		SCRIPT->Render_Script(233, CScript::FULL, 1.9f);
	else if (dSoundTime >= 34.0 && dSoundTime < 34.0 + dTimeDelta)
		SCRIPT->Render_Script(234, CScript::FULL, 1.9f);
	else if (dSoundTime >= 40.5 && dSoundTime < 40.5 + dTimeDelta)
		SCRIPT->Render_Script(236, CScript::FULL, 0.9f);
	else if (dSoundTime >= 45.0 && dSoundTime < 45.0 + dTimeDelta)
		SCRIPT->Render_Script(237, CScript::FULL, 1.9f);
	else if (dSoundTime >= 47.0 && dSoundTime < 47.0 + dTimeDelta)
		SCRIPT->Render_Script(238, CScript::FULL, 1.9f);
	else if (dSoundTime >= 49.0 && dSoundTime < 49.0 + dTimeDelta)
		SCRIPT->Render_Script(239, CScript::FULL, 1.9f);
	else if (dSoundTime >= 51.0 && dSoundTime < 51.0 + dTimeDelta)
		SCRIPT->Render_Script(240, CScript::FULL, 0.9f);
	else if (dSoundTime >= 52.0 && dSoundTime < 52.0 + dTimeDelta)
		SCRIPT->Render_Script(241, CScript::FULL, 1.9f);
	else if (dSoundTime >= 54.0 && dSoundTime < 54.0 + dTimeDelta)
		SCRIPT->Render_Script(242, CScript::FULL, 1.9f);
	else if (dSoundTime >= 56.0 && dSoundTime < 56.0 + dTimeDelta)
		SCRIPT->Render_Script(243, CScript::FULL, 1.9f);
	else if (dSoundTime >= 59.0 && dSoundTime < 59.0 + dTimeDelta)
	{
		EFFECT->Add_Effect_CutScene(Effect_Value_CutScene::Levitation_Beam_Moon);
		SCRIPT->Render_Script(243, CScript::FULL, 1.9f);
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
