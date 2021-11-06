#include "stdafx.h"
#include "BehaviorTree.h"
#include "CompositeTask.h"
#include "GameObject.h"

CBehaviorTree::CBehaviorTree(CGameObject* pSubject)
	: m_pSubject(pSubject)
{
}

_uint CBehaviorTree::Running(_double TimeDelta)
{
	if (nullptr == m_pRootTask) return -1;

	return m_pRootTask->OnUpdate(TimeDelta);
}

void CBehaviorTree::Free()
{
	Safe_Release(m_pRootTask);
}
