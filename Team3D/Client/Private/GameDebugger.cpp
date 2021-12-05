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