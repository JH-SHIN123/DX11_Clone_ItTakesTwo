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
	DIJOYSTATE& JoyStickState = CInput_Device::GetInstance()->Get_PadState();

	while (!bExit)
	{
		system("cls");

		cout << "IX = " << JoyStickState.lX << endl;
		cout << "lY = " << JoyStickState.lY << endl;
		cout << "lZ = " << JoyStickState.lZ << endl << endl;

		cout << "IRx = " << JoyStickState.lRx << endl;
		cout << "IRy = " << JoyStickState.lRy << endl;
		cout << "IRz = " << JoyStickState.lRz << endl << endl;

		cout << "rgdwPOV[0] = " << JoyStickState.rgdwPOV[0] << endl;
		cout << "rgdwPOV[1] = " << JoyStickState.rgdwPOV[1] << endl;
		cout << "rgdwPOV[2] = " << JoyStickState.rgdwPOV[2] << endl;
		cout << "rgdwPOV[3] = " << JoyStickState.rgdwPOV[3] << endl << endl;

		cout << "rglSlider[0] = " << JoyStickState.rglSlider[0] << endl;
		cout << "rglSlider[1] = " << JoyStickState.rglSlider[1] << endl << endl;

		cout << "Button[0] = " << (_int)JoyStickState.rgbButtons[0] << endl;
		cout << "Button[1] = " << (_int)JoyStickState.rgbButtons[1] << endl;
		cout << "Button[2] = " << (_int)JoyStickState.rgbButtons[2] << endl;
		cout << "Button[3] = " << (_int)JoyStickState.rgbButtons[3] << endl;
		cout << "Button[4] = " << (_int)JoyStickState.rgbButtons[4] << endl;
		cout << "Button[5] = " << (_int)JoyStickState.rgbButtons[5] << endl;
		cout << "Button[6] = " << (_int)JoyStickState.rgbButtons[6] << endl;
		cout << "Button[7] = " << (_int)JoyStickState.rgbButtons[7] << endl;
		cout << "Button[8] = " << (_int)JoyStickState.rgbButtons[8] << endl;
		cout << "Button[9] = " << (_int)JoyStickState.rgbButtons[9] << endl;
		cout << "Button[10] = " << (_int)JoyStickState.rgbButtons[10] << endl;
		cout << "Button[11] = " << (_int)JoyStickState.rgbButtons[11] << endl;
		cout << "Button[12] = " << (_int)JoyStickState.rgbButtons[12] << endl;
		cout << "Button[13] = " << (_int)JoyStickState.rgbButtons[13] << endl;
		cout << "Button[14] = " << (_int)JoyStickState.rgbButtons[14] << endl;
		cout << "Button[15] = " << (_int)JoyStickState.rgbButtons[15] << endl;

		cout << "Button[16] = " << (_int)JoyStickState.rgbButtons[16] << endl;
		cout << "Button[17] = " << (_int)JoyStickState.rgbButtons[17] << endl;
		cout << "Button[18] = " << (_int)JoyStickState.rgbButtons[18] << endl;
		cout << "Button[19] = " << (_int)JoyStickState.rgbButtons[19] << endl;
		cout << "Button[20] = " << (_int)JoyStickState.rgbButtons[20] << endl;
		cout << "Button[21] = " << (_int)JoyStickState.rgbButtons[21] << endl;
		cout << "Button[22] = " << (_int)JoyStickState.rgbButtons[22] << endl;
		cout << "Button[23] = " << (_int)JoyStickState.rgbButtons[23] << endl;
		cout << "Button[24] = " << (_int)JoyStickState.rgbButtons[24] << endl;
		cout << "Button[25] = " << (_int)JoyStickState.rgbButtons[25] << endl;
		cout << "Button[26] = " << (_int)JoyStickState.rgbButtons[26] << endl;
		cout << "Button[27] = " << (_int)JoyStickState.rgbButtons[27] << endl;
		cout << "Button[28] = " << (_int)JoyStickState.rgbButtons[28] << endl;
		cout << "Button[29] = " << (_int)JoyStickState.rgbButtons[29] << endl;
		cout << "Button[30] = " << (_int)JoyStickState.rgbButtons[30] << endl;
		cout << "Button[31] = " << (_int)JoyStickState.rgbButtons[31] << endl;

		Sleep(400);
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