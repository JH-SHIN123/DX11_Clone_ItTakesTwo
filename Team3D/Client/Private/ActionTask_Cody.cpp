#include "stdafx.h"
#include "..\Public\ActionTask_Cody.h"
#include "Cody.h"

#pragma region Idle
_int CActionTask_Cody_Idle::OnStart()
{
	_int iReturn = RETURN_FALSE;

	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return iReturn;

	if (CCody::MH_BIGWAITING_ENTER != pCody->Get_CurState() &&
		CCody::MH_BIGWAITING != pCody->Get_CurState()) {
		pCody->Set_NextState(CCody::MH);
		m_dWaitingDeltaT = 0.0;
		iReturn = RETURN_TRUE;
	}

	return iReturn;
}

_int CActionTask_Cody_Idle::OnUpdate(_double TimeDelta)
{
	_int iReturn = RETURN_FALSE;

	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return iReturn;

	if (m_dWaitingDeltaT >= 2.0)
	{
		m_dWaitingDeltaT = 0.0;
	}
	else {
		m_dWaitingDeltaT += TimeDelta;
		iReturn = RETURN_RUNNING;
	}

	return iReturn;
}

CActionTask_Cody_Idle* CActionTask_Cody_Idle::Create(CBehaviorTree* pBehaviorTree)
{
	CActionTask_Cody_Idle* pInstance = new CActionTask_Cody_Idle(pBehaviorTree);
	if (FAILED(pInstance->OnStart())) {
		MSG_BOX("Failed to Creating Instance (CActionTask_Cody_Idle) ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_int CActionTask_Cody_BigWaiting_Enter::OnStart()
{
	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return RETURN_FALSE;

	pCody->Set_NextState(CCody::MH_BIGWAITING_ENTER);
	m_dRunningDeltaT = 0.0;

	return RETURN_TRUE;
}

_int CActionTask_Cody_BigWaiting_Enter::OnUpdate(_double TimeDelta)
{
	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return RETURN_FALSE;

	if (m_dRunningDeltaT >= 2.5)
	{
		m_dRunningDeltaT = 0.0;
		return RETURN_TRUE;
	}
	else
		m_dRunningDeltaT += TimeDelta;
	//pCody->

	return RETURN_RUNNING;
}

CActionTask_Cody_BigWaiting_Enter* CActionTask_Cody_BigWaiting_Enter::Create(CBehaviorTree* pBehaviorTree)
{
	CActionTask_Cody_BigWaiting_Enter* pInstance = new CActionTask_Cody_BigWaiting_Enter(pBehaviorTree);
	if (FAILED(pInstance->OnStart())) {
		MSG_BOX("Failed to Creating Instance (CActionTask_Cody_BigWaiting_Enter) ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_int CActionTask_Cody_BigWaiting::OnStart()
{
	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return RETURN_FALSE;

	pCody->Set_NextState(CCody::MH_BIGWAITING);

	return RETURN_TRUE;
}

_int CActionTask_Cody_BigWaiting::OnUpdate(_double TimeDelta)
{
	return RETURN_RUNNING;
}

CActionTask_Cody_BigWaiting* CActionTask_Cody_BigWaiting::Create(CBehaviorTree* pBehaviorTree)
{
	CActionTask_Cody_BigWaiting* pInstance = new CActionTask_Cody_BigWaiting(pBehaviorTree);
	if (FAILED(pInstance->OnStart())) {
		MSG_BOX("Failed to Creating Instance (CActionTask_Cody_BigWaiting_Enter) ");
		Safe_Release(pInstance);
	}
	return pInstance;
}
#pragma endregion

#pragma region Jog
_int CActionTask_Cody_Jog_Fwd_Left::OnStart()
{
	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return RETURN_FALSE;

	if (pCody->m_bJog && pCody->m_bJog_Left) {
		pCody->Set_NextState(CCody::JOG_LEFT);
		return RETURN_TRUE;
	}

	return RETURN_FALSE;
}

_int CActionTask_Cody_Jog_Fwd_Left::OnUpdate(_double TimeDelta)
{
	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return RETURN_FALSE;

	if (!pCody->m_bJog || !pCody->m_bJog_Left) {
		return RETURN_FALSE;
	}

	return RETURN_RUNNING;
}

CActionTask_Cody_Jog_Fwd_Left* CActionTask_Cody_Jog_Fwd_Left::Create(CBehaviorTree* pBehaviorTree)
{
	CActionTask_Cody_Jog_Fwd_Left* pInstance = new CActionTask_Cody_Jog_Fwd_Left(pBehaviorTree);
	if (FAILED(pInstance->OnStart())) {
		MSG_BOX("Failed to Creating Instance (CActionTask_Cody_Jog_Fwd_Left) ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_int CActionTask_Cody_Jog_Fwd_Right::OnStart()
{
	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return RETURN_FALSE;

	if (pCody->m_bJog && pCody->m_bJog_Right) {
		pCody->Set_NextState(CCody::JOG_RIGHT);
		return RETURN_TRUE;
	}

	return RETURN_FALSE;
}

_int CActionTask_Cody_Jog_Fwd_Right::OnUpdate(_double TimeDelta)
{
	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return RETURN_FALSE;

	if (!pCody->m_bJog || !pCody->m_bJog_Right) {
		return RETURN_FALSE;
	}

	return RETURN_RUNNING;
}

CActionTask_Cody_Jog_Fwd_Right* CActionTask_Cody_Jog_Fwd_Right::Create(CBehaviorTree* pBehaviorTree)
{
	CActionTask_Cody_Jog_Fwd_Right* pInstance = new CActionTask_Cody_Jog_Fwd_Right(pBehaviorTree);
	if (FAILED(pInstance->OnStart())) {
		MSG_BOX("Failed to Creating Instance (CActionTask_Cody_Jog_Fwd_Right) ");
		Safe_Release(pInstance);
	}
	return pInstance;
}
_int CActionTask_Cody_Jog_Fwd::OnStart()
{
	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return RETURN_FALSE;

	if (pCody->m_bJog) {
		pCody->Set_NextState(CCody::JOG);
		return RETURN_TRUE;
	}

	return RETURN_FALSE;
}
_int CActionTask_Cody_Jog_Fwd::OnUpdate(_double TimeDelta)
{
	CCody* pCody = (CCody*)m_pBehaviorTree->Get_Subject();
	if (nullptr == pCody) return RETURN_FALSE;

	if (!pCody->m_bJog) {
		return RETURN_FALSE;
	}

	return RETURN_RUNNING;
}

CActionTask_Cody_Jog_Fwd* CActionTask_Cody_Jog_Fwd::Create(CBehaviorTree* pBehaviorTree)
{
	CActionTask_Cody_Jog_Fwd* pInstance = new CActionTask_Cody_Jog_Fwd(pBehaviorTree);
	if (FAILED(pInstance->OnStart())) {
		MSG_BOX("Failed to Creating Instance (CActionTask_Cody_Jog_Fwd) ");
		Safe_Release(pInstance);
	}
	return pInstance;
}
#pragma endregion