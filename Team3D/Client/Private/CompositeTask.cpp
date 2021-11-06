#include "stdafx.h"
#include "CompositeTask.h"
#include "ActionTask.h"

CCompositeTask::CCompositeTask(CBehaviorTree* pBehaviorTree, TYPE eType) :
	CTask(pBehaviorTree), m_eType(eType)
{
}

CTask* CCompositeTask::Add_ChildTask(CTask* pTask)
{
	if (nullptr == pTask) return nullptr;

	m_ChildTasks.push_back(pTask);

	return pTask;
}

_int CCompositeTask::OnStart()
{
	return RETURN_TRUE;
}

_int CCompositeTask::OnUpdate(_double TimeDelta)
{
	_int iResult = RETURN_FALSE;

	switch (m_eType)
	{
	case COMP_SELECT:
		iResult = OnUpdate_Select(TimeDelta);
		break;
	case COMP_SEQUENCE:
		iResult = OnUpdate_Sequence(TimeDelta);
		break;
	}

	return iResult;
}

void CCompositeTask::OnEnd()
{
	for (auto& pNode : m_ChildTasks)
		pNode->Set_CurrentState(STATE_START);
}

_int CCompositeTask::OnNotify()
{
	return _int();
}

_int CCompositeTask::OnUpdate_Select(_double TimeDelta)
{
	// Select: 자식노드중 하나가 성공하면 실행 중지

	_int iResult = RETURN_FALSE;

	if (m_ChildTasks.empty()) return iResult;

	/* DFS 탐색 */
	for (auto& pNode : m_ChildTasks)
	{
		if (nullptr == pNode) return iResult;

		if (STATE_START == pNode->Get_CurrentState()) // OnStart
		{
			iResult = pNode->OnStart();

			if (RETURN_FALSE == iResult) {
				continue;
			}
			else if (RETURN_TRUE == iResult || RETURN_RUNNING == iResult) {
				pNode->Set_CurrentState(STATE_RUNNING);
			}
		}

		if (STATE_RUNNING == pNode->Get_CurrentState()) // OnUpdate
		{
			iResult = pNode->OnUpdate(TimeDelta);

			if (RETURN_TRUE == iResult) {
				pNode->OnEnd();
				pNode->Set_CurrentState(STATE_START);
				return iResult;
			}
			else if (RETURN_FALSE == iResult) {
				pNode->OnEnd();
				pNode->Set_CurrentState(STATE_START);
				continue;
			}
			else if (RETURN_RUNNING == iResult) {
				return iResult;
			}
		}
	}

	return iResult;
}

_int CCompositeTask::OnUpdate_Sequence(_double TimeDelta)
{
	// Sequence : 자식노드 중 하나가 실패하면 실행 중지

	_int iResult = RETURN_FALSE;

	if (m_ChildTasks.empty()) return iResult;

	/* DFS 탐색 */
	for (auto& pNode : m_ChildTasks)
	{
		if (nullptr == pNode) return iResult;

		if (STATE_START == pNode->Get_CurrentState()) // OnStart
		{
			iResult = pNode->OnStart();

			if (RETURN_FALSE == iResult) {
				return iResult;
			}
			else if (RETURN_TRUE == iResult || RETURN_RUNNING == iResult) {
				pNode->Set_CurrentState(STATE_RUNNING);
			}
		}

		if (STATE_RUNNING == pNode->Get_CurrentState()) // OnUpdate
		{
			iResult = pNode->OnUpdate(TimeDelta);

			if (RETURN_TRUE == iResult) {
				pNode->OnEnd();
				pNode->Set_CurrentState(STATE_STOP);
				continue;
			}
			else if (RETURN_FALSE == iResult) {
				pNode->OnEnd();
				pNode->Set_CurrentState(STATE_START);
				return iResult;
			}
			else if (RETURN_RUNNING == iResult) {
				return iResult;
			}
		}
	}

	return iResult;
}

_int CCompositeTask::OnUpdate_Tasks(_double TimeDelta)
{
	for (auto& pTask : m_TaskStack) {
		pTask->OnUpdate(TimeDelta);
	}

	return _int();
}

CCompositeTask* CCompositeTask::Create(CBehaviorTree* pBehaviorTree, TYPE eType)
{
	CCompositeTask* pInstance = new CCompositeTask(pBehaviorTree, eType);
	if (FAILED(pInstance->OnStart())) {
		MSG_BOX("Failed to Creating Instance (CCompositeTask) ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCompositeTask::Free()
{
	for (auto& pTask : m_ChildTasks)
		Safe_Release(pTask);
}

