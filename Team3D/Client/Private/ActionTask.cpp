#include "stdafx.h"
#include "ActionTask.h"

//_int CActionTask_SprintStart::OnUpdate(_double TimeDelta)
//{
//	if (nullptr == m_pBehaviorTree) return RETURN_FALSE;
//
//	// 몇초 동안만 활성화? -> 플레이어 컨트롤러쪽에서 설정
//
//	_int iReturn = RETURN_FALSE;
//
//	//if (m_pBehaviorTree->Get_Subject()->m_bSprint && m_pBehaviorTree->Get_Subject()->m_bJog) {
//	//	// if Normal / 180도 회전할때
//	//	cout << "Sprint Start" << endl;
//	//	iReturn = RETURN_TRUE;
//	//}
//
//	return iReturn;
//}
//
//void CActionTask_SprintStart::OnEnd()
//{
//}
//
//CActionTask_SprintStart* CActionTask_SprintStart::Create(CBehaviorTree* pBehaviorTree)
//{
//	CActionTask_SprintStart* pInstance = new CActionTask_SprintStart(pBehaviorTree);
//	if (FAILED(pInstance->OnStart()))
//	{
//		MSG_BOX("Failed to Creating Instance (CActionTask_SprintStart)");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//_int CActionTask_Sprint::OnUpdate(_double TimeDelta)
//{
//	// if Normal / left & right
//	_int iReturn = RETURN_FALSE;
//
//	//if (m_pBehaviorTree->Get_Subject()->m_bSprint && m_pBehaviorTree->Get_Subject()->m_bJog) {
//	//	// if Normal / 180도 회전할때
//	//	cout << "Sprint" << endl;
//	//	iReturn = RETURN_RUNNING;
//	//}
//	//else
//	//	iReturn = RETURN_TRUE;
//
//	return iReturn;
//}
//
//void CActionTask_Sprint::OnEnd()
//{
//}
//
//CActionTask_Sprint* CActionTask_Sprint::Create(CBehaviorTree* pBehaviorTree)
//{
//	CActionTask_Sprint* pInstance = new CActionTask_Sprint(pBehaviorTree);
//	if (FAILED(pInstance->OnStart()))
//	{
//		MSG_BOX("Failed to Creating Instance (CActionTask_Sprint) ");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//_int CActionTask_JogStart::OnStart()
//{
//	m_bRunning = true;
//	m_dExitDeltaT = 0.0;
//
//	return RETURN_TRUE;
//}
//
//_int CActionTask_JogStart::OnUpdate(_double TimeDelta)
//{
//	_int iReturn = RETURN_FALSE;
//
//	//if (m_pBehaviorTree->Get_Subject()->m_bJog) {
//	//	cout << "Jog Start" << endl;
//	//	iReturn = RETURN_TRUE;
//	//}
//	//CCharacter* pCharacter = m_pBehaviorTree->Get_Character();
//	//if (nullptr == pCharacter) return RETURN_FALSE;
//
//	//CPlayerController* pController = pCharacter->Get_Controller();
//	//if (nullptr == pController) return RETURN_FALSE;
//
//	//if (pController->m_bJog) {
//	//	pCharacter->Change_Animation(CCharacter::ANIM_STATE::JOG_START_FWD);
//	//	iReturn = RETURN_TRUE;
//
//	//	if (m_bRunning)
//	//	{
//	//		if (m_dExitDeltaT >= 0.501) {
//	//			// 애니메이션이 끝나면 Return True
//	//			iReturn = RETURN_TRUE;
//	//			pController->m_bJogStart = true;
//	//			m_bRunning = false;
//	//			m_dExitDeltaT = 0.0;
//	//		}
//	//		else {
//	//			m_dExitDeltaT += TimeDelta;
//	//			iReturn = RETURN_RUNNING;
//	//		}
//	//	}
//	//	else iReturn = RETURN_TRUE;
//	//}
//
//	return iReturn;
//}
//
//void CActionTask_JogStart::OnEnd()
//{
//
//}
//
//CActionTask_JogStart* CActionTask_JogStart::Create(CBehaviorTree* pBehaviorTree)
//{
//	CActionTask_JogStart* pInstance = new CActionTask_JogStart(pBehaviorTree);
//	if (FAILED(pInstance->OnStart()))
//	{
//		MSG_BOX("Failed to Creating Instance (CActionTask_JogStart) ");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//_int CActionTask_Jog::OnUpdate(_double TimeDelta)
//{
//	//// if Normal / left & right
//	_int iReturn = RETURN_FALSE;
//
//	//CCharacter* pCharacter = m_pBehaviorTree->Get_Character();
//	//if (nullptr == pCharacter) return RETURN_FALSE;
//
//	//CPlayerController* pController = pCharacter->Get_Controller();
//	//if (nullptr == pController) return RETURN_FALSE;
//
//	//if (pController->m_bJog)
//	//{
//	//	if (pController->Key_Pressing(pController->KEY_A))
//	//		pCharacter->Change_Animation(CCharacter::ANIM_STATE::JOG_LEFT);
//	//	else if (pController->Key_Pressing(pController->KEY_D))
//	//		pCharacter->Change_Animation(CCharacter::ANIM_STATE::JOG_RIGHT);
//	//	else
//	//		pCharacter->Change_Animation(CCharacter::ANIM_STATE::JOG);
//
//	//	iReturn = RETURN_RUNNING;
//	//}
//	//else
//	//	iReturn = RETURN_TRUE;
//
//	return iReturn;
//}
//
//void CActionTask_Jog::OnEnd()
//{
//}
//
//CActionTask_Jog* CActionTask_Jog::Create(CBehaviorTree* pBehaviorTree)
//{
//	CActionTask_Jog* pInstance = new CActionTask_Jog(pBehaviorTree);
//	if (FAILED(pInstance->OnStart()))
//	{
//		MSG_BOX("Failed to Creating Instance (CActionTask_Jog) ");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//_int CActionTask_Stop::OnStart()
//{
//	//CCharacter* pCharacter = m_pBehaviorTree->Get_Character();
//	//if (nullptr == pCharacter) return RETURN_FALSE;
//
//	//CPlayerController* pController = pCharacter->Get_Controller();
//	//if (nullptr == pController) return RETURN_FALSE;
//
//	//if (true == pController->m_bJogStart) {
//	//	m_bRunning = true;
//	//	m_dExitDeltaT = 0.0;
//
//	//	return RETURN_TRUE;
//	//}
//
//	return RETURN_FALSE;
//}
//
//_int CActionTask_Stop::OnUpdate(_double TimeDelta)
//{
//	_int iReturn = RETURN_FALSE;
//
//	//CCharacter* pCharacter = m_pBehaviorTree->Get_Character();
//	//if (nullptr == pCharacter) return RETURN_FALSE;
//
//	//CPlayerController* pController = pCharacter->Get_Controller();
//	//if (nullptr == pController) return RETURN_FALSE;
//
//	//pCharacter->Change_Animation(CCharacter::ANIM_STATE::JOG_STOP_FWD_EASY);
//
//	//if (m_bRunning)
//	//{
//	//	if (m_dExitDeltaT >= 0.6) {
//	//		// 애니메이션이 끝나면 Return True
//	//		iReturn = RETURN_FALSE;
//
//	//		pController->m_bJogStart = false;
//	//		m_bRunning = false;
//	//		m_dExitDeltaT = 0.0;
//	//	}
//	//	else {
//	//		m_dExitDeltaT += TimeDelta;
//	//		iReturn = RETURN_RUNNING;
//	//	}
//	//}
//
//	return iReturn;
//}
//
//void CActionTask_Stop::OnEnd()
//{
//
//}
//
//CActionTask_Stop* CActionTask_Stop::Create(CBehaviorTree* pBehaviorTree)
//{
//	CActionTask_Stop* pInstance = new CActionTask_Stop(pBehaviorTree);
//	if (FAILED(pInstance->OnStart()))
//	{
//		MSG_BOX("Failed to Creating Instance (CActionTask_Stop) ");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//_int CActionTask_Idle::OnUpdate(_double TimeDelta)
//{
//	//cout << "Idle" << endl;
//	_int iReturn = RETURN_FALSE;
//
//	//CCharacter* pCharacter = m_pBehaviorTree->Get_Character();
//	//if (nullptr == pCharacter) return RETURN_FALSE;
//
//	//CPlayerController* pController = pCharacter->Get_Controller();
//	//if (nullptr == pController) return RETURN_FALSE;
//
//	//if (!pController->m_bJog)
//	//{
//	//	pCharacter->Change_Animation(CCharacter::ANIM_STATE::IDLE);
//	//	iReturn = RETURN_RUNNING;
//	//}
//
//	return iReturn;
//}
//
//void CActionTask_Idle::OnEnd()
//{
//}
//
//CActionTask_Idle* CActionTask_Idle::Create(CBehaviorTree* pBehaviorTree)
//{
//	CActionTask_Idle* pInstance = new CActionTask_Idle(pBehaviorTree);
//	if (FAILED(pInstance->OnStart()))
//	{
//		MSG_BOX("Failed to Creating Instance (CActionTask_Idle) ");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
