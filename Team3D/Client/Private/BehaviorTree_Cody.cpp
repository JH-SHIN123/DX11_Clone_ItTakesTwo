#include "stdafx.h"
#include "..\Public\BehaviorTree_Cody.h"
#include "CompositeTask.h"
#include "Cody.h"
#include "ActionTask_Cody.h"

CBehaviorTree_Cody::CBehaviorTree_Cody(CGameObject* pSubject) :
	CBehaviorTree(pSubject)
{
}

HRESULT CBehaviorTree_Cody::NativeConstruct()
{
	/* Tree ±¸¼º */

	// RootNode (Locomotion Selector)
	m_pRootTask = CCompositeTask::Create(this, CCompositeTask::COMP_SELECT);

	// Jog
	CCompositeTask* pJogSeq = CCompositeTask::Create(this, CCompositeTask::COMP_SEQUENCE);
	CCompositeTask* pJogStartSel = CCompositeTask::Create(this, CCompositeTask::COMP_SELECT);
	pJogStartSel->Add_ChildTask(CActionTask_Cody_Jog_StartFwd::Create(this));
	//pJogStartSel->Add_ChildTask(CActionTask_Cody_Jog_Fwd_Left::Create(this));
	//pJogStartSel->Add_ChildTask(CActionTask_Cody_Jog_Fwd_Left::Create(this));
	m_pRootTask->Add_ChildTask(pJogStartSel);

	CCompositeTask* pJogSel = CCompositeTask::Create(this, CCompositeTask::COMP_SELECT);
	pJogSel->Add_ChildTask(CActionTask_Cody_Jog_Fwd_Left::Create(this));
	pJogSel->Add_ChildTask(CActionTask_Cody_Jog_Fwd_Right::Create(this));
	pJogSel->Add_ChildTask(CActionTask_Cody_Jog_Fwd::Create(this));
	pJogSeq->Add_ChildTask(pJogSel);
	m_pRootTask->Add_ChildTask(pJogSeq);

	// Idle
	CCompositeTask* pIdleSel = CCompositeTask::Create(this, CCompositeTask::COMP_SELECT);
	pIdleSel->Add_ChildTask(CActionTask_Cody_Idle::Create(this));
	CCompositeTask* pBigWaitingSeq = CCompositeTask::Create(this, CCompositeTask::COMP_SEQUENCE);
	pBigWaitingSeq->Add_ChildTask(CActionTask_Cody_BigWaiting_Enter::Create(this));
	pBigWaitingSeq->Add_ChildTask(CActionTask_Cody_BigWaiting::Create(this));
	pIdleSel->Add_ChildTask(pBigWaitingSeq);
	m_pRootTask->Add_ChildTask(pIdleSel);

	return S_OK;
}

CBehaviorTree_Cody* CBehaviorTree_Cody::Create(CGameObject* pSubject)
{
	CBehaviorTree_Cody* pInstance = new CBehaviorTree_Cody(pSubject);

	if (FAILED(pInstance->NativeConstruct())) {
		MSG_BOX("Failed to Creating Instance (CBehaviorTree_Cody) ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBehaviorTree_Cody::Free()
{
	__super::Free();
}
