#include "stdafx.h"
#include "BehaviorTree.h"
#include "CompositeTask.h"
#include "ActionTask.h"
#include "GameObject.h"

CBehaviorTree::CBehaviorTree(CCharacter* pCharacter)
	: m_pCharacter(pCharacter)
{
	Safe_AddRef(m_pCharacter);
}

HRESULT CBehaviorTree::NativeConstruct()
{
	/* Tree ±¸¼º */

	// RootNode (Locomotion Selector)
	m_pRootTask = CCompositeTask::Create(this, CCompositeTask::COMP_SELECT);

	// Sprint Seq
	CCompositeTask* pSprintSequence = CCompositeTask::Create(this, CCompositeTask::COMP_SEQUENCE);
	pSprintSequence->Add_ChildTask(CActionTask_SprintStart::Create(this));
	pSprintSequence->Add_ChildTask(CActionTask_Sprint::Create(this));
	m_pRootTask->Add_ChildTask(pSprintSequence);

	// Jog Seq
	CCompositeTask* pJogSequence = CCompositeTask::Create(this, CCompositeTask::COMP_SEQUENCE);
	pJogSequence->Add_ChildTask(CActionTask_JogStart::Create(this));
	pJogSequence->Add_ChildTask(CActionTask_Jog::Create(this));
	m_pRootTask->Add_ChildTask(pJogSequence);

	// Jog Stop
	m_pRootTask->Add_ChildTask(CActionTask_Stop::Create(this));

	// Idle
	m_pRootTask->Add_ChildTask(CActionTask_Idle::Create(this));

	return S_OK;
}

_uint CBehaviorTree::Running(_double TimeDelta)
{
	if (nullptr == m_pRootTask) return -1;

	return m_pRootTask->OnUpdate(TimeDelta);
}

CBehaviorTree* CBehaviorTree::Create(CCharacter* pCharacter)
{
	CBehaviorTree* pInstance = new CBehaviorTree(pCharacter);

	if (FAILED(pInstance->NativeConstruct())) {
		MSG_BOX("Failed to Creating Instance (CBehaviorTree) ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBehaviorTree::Free()
{
	Safe_Release(m_pRootTask);
	Safe_Release(m_pCharacter);
}
