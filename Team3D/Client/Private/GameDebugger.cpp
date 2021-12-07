#include "stdafx.h"
#include "..\Public\GameDebugger.h"
#include <iostream>
#include "MainApp.h"

_uint APIENTRY ConsoleThread(void* pArg)
{
	CGameDebugger* pDebugger = (CGameDebugger*)pArg;

	EnterCriticalSection(pDebugger->Get_CriticalSectionPtr());

	pDebugger->Debugging();

	LeaveCriticalSection(pDebugger->Get_CriticalSectionPtr());

	return NO_EVENT;
}

HRESULT CGameDebugger::NativeConstruct(CMainApp * pMainApp)
{
	m_pMainApp = pMainApp;

	InitializeCriticalSection(&m_CriticalSection);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ConsoleThread, this, 0, nullptr);

	return S_OK;
}

void CGameDebugger::Debugging()
{
#ifdef __DEBUGGING_PAD
	_bool& bExit = m_pMainApp->Get_DebuggerExit();
	DIJOYSTATE& m_JoyStickState = CInput_Device::GetInstance()->Get_PadState();

	while (!bExit)
	{
		system("cls");

		cout << "IR = " << m_JoyStickState.lRx << "  " << m_JoyStickState.lRy << "  " << m_JoyStickState.lRz << endl;
		cout << "IL = " << m_JoyStickState.lX << "  " << m_JoyStickState.lY << "  " << m_JoyStickState.lZ << endl;
		cout << "POV = " << m_JoyStickState.rgdwPOV[0] << "  " << m_JoyStickState.rgdwPOV[1] << "  " << m_JoyStickState.rgdwPOV[2] << "  " << m_JoyStickState.rgdwPOV[3] << endl;
		cout << "rglSlider = " << m_JoyStickState.rglSlider[0] << "  " << m_JoyStickState.rglSlider[1] << endl;
		cout << "B 0123 = " << endl;
		cout << m_JoyStickState.rgbButtons[0] << "  " << m_JoyStickState.rgbButtons[1] << "  " << m_JoyStickState.rgbButtons[2] << "  " << m_JoyStickState.rgbButtons[3] << "  " << endl;
		cout << "B 4567 = " << endl;
		cout << m_JoyStickState.rgbButtons[4] << "  " << m_JoyStickState.rgbButtons[5] << "  " << m_JoyStickState.rgbButtons[6] << "  " << m_JoyStickState.rgbButtons[7] << "  " << endl;
		cout << "B 891011 = " << endl;
		cout << m_JoyStickState.rgbButtons[8] << "  " << m_JoyStickState.rgbButtons[9] << "  " << m_JoyStickState.rgbButtons[10] << "  " << m_JoyStickState.rgbButtons[11] << "  " << endl;
		cout << "B 12131415 = " << endl;
		cout << m_JoyStickState.rgbButtons[12] << "  " << m_JoyStickState.rgbButtons[13] << "  " << m_JoyStickState.rgbButtons[14] << "  " << m_JoyStickState.rgbButtons[15] << "  " << endl;
		cout << "B 16171819 = " << endl;
		cout << m_JoyStickState.rgbButtons[16] << "  " << m_JoyStickState.rgbButtons[17] << "  " << m_JoyStickState.rgbButtons[18] << "  " << m_JoyStickState.rgbButtons[19] << "  " << endl;
		cout << "B 20212223 = " << endl;
		cout << m_JoyStickState.rgbButtons[20] << "  " << m_JoyStickState.rgbButtons[21] << "  " << m_JoyStickState.rgbButtons[22] << "  " << m_JoyStickState.rgbButtons[23] << "  " << endl;
		cout << "B 24252627 = " << endl;
		cout << m_JoyStickState.rgbButtons[24] << "  " << m_JoyStickState.rgbButtons[25] << "  " << m_JoyStickState.rgbButtons[26] << "  " << m_JoyStickState.rgbButtons[27] << "  " << endl;
		cout << "B 28293031 = " << endl;
		cout << m_JoyStickState.rgbButtons[28] << "  " << m_JoyStickState.rgbButtons[29] << "  " << m_JoyStickState.rgbButtons[30] << "  " << m_JoyStickState.rgbButtons[31] << "  " << endl;

		Sleep(500);
	}
#else
	_tchar szLayerTag[MAX_PATH] = TEXT("");

	_bool& bExit = m_pMainApp->Get_DebuggerExit();
	_bool& bBreak = m_pMainApp->Get_DebuggerBreak();
	CGameObject*& pGameObject = m_pMainApp->Get_DebuggerTarget();
	CGameInstance*& pGameInstance = m_pMainApp->Get_DebuggerGameInstance();
	CGameObject_Manager*& pGameObjectManager = m_pMainApp->Get_DebuggerGameObjectManager();

	while (!bExit)
	{
		system("cls");

		cout << "========================================================================" << endl;
		cout << "GAME DEBUGGER" << endl;
		cout << "========================================================================" << endl;

		if (pGameObjectManager == nullptr) break;

		cout << "레이어 태그 검색 : ";
		wcin >> szLayerTag;
		cout << "------------------------------------------------------------------------" << endl;

		if (pGameObjectManager == nullptr) break;

		unordered_map<const _tchar*, CLayer*> pLayers = pGameObjectManager->Get_Layers()[Level::LEVEL_STAGE];
		auto iter = find_if(pLayers.begin(), pLayers.end(), CTagFinder(szLayerTag));

		if (iter == pLayers.end())
		{
			cout << "해당 레이어가 존재하지 않습니다." << endl;
			system("pause");
			continue;
		}

		_uint iIndex = 0;
		CLayer* pLayer = (*iter).second;
		list<CGameObject*> pGameObjects = pLayer->Get_ObjectList();

		cout << "레이어 오브젝트 개수 : " << pGameObjects.size() << endl;
		cout << "몇 번째 오브젝트를 불러오시겠습니까? : ";
		cin >> iIndex;
		cout << "------------------------------------------------------------------------" << endl;

		--iIndex;

		if (iIndex >= pGameObjects.size() || iIndex < 0)
		{
			cout << "잘못된 인덱스입니다." << endl;
			system("pause");
			continue;
		}

		auto& iter_Object = pGameObjects.begin();

		for (_uint i = 0; i < iIndex; ++i)
			++iter_Object;

		m_pMainApp->Set_DebuggerTarget(*iter_Object);

		bBreak = false;

		while (!bBreak)
		{
			system("cls");

			cout << "========================================================================" << endl;
			cout << "GAME DEBUGGER" << endl;
			cout << "========================================================================" << endl;
			cout << "초기화 - 게임창 활성화 상태에서 Delete" << endl;
			cout << "------------------------------------------------------------------------" << endl;
			cout << "레이어 태그 : ";
			wcout << szLayerTag << endl;
			cout << "------------------------------------------------------------------------" << endl;

			if (pGameObjectManager == nullptr) break;

			if (pGameObject != nullptr)
			{
				_vector vPosition = pGameObject->Get_Position();
				cout << "Pos : ( " << XMVectorGetX(vPosition) << ", " << XMVectorGetY(vPosition) << ", " << XMVectorGetZ(vPosition) << " )" << endl;
			}

			cout << "------------------------------------------------------------------------" << endl;

			SleepEx(500, bBreak);
		}

		pGameObject = nullptr;
		bBreak = false;
		//system("pause>null");
		//system("pause");
	}
#endif
}

CGameDebugger * CGameDebugger::Create(CMainApp * pMainApp)
{
	CGameDebugger*	pInstance = new CGameDebugger;

	if (FAILED(pInstance->NativeConstruct(pMainApp)))
	{
		MSG_BOX("Failed to Create Instance - CGameDebugger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameDebugger::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	CloseHandle(m_hThread);
}