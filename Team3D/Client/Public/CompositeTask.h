#pragma once

#include "Task.h"

BEGIN(Client)
class CActionTask;
class CCompositeTask : public CTask
{
	/*
	여러개의 자식으로 구성된 태스크
	핵심용도는 노드의 플로우를 제어하는것
	노드의 실행순서는, 위에서 아래로, 왼쪽에서 오른쪽으로 (깊이우선탐색)
	*/

public:
	/*
	Select : 자식노드중 하나가 성공하면 실행 중지
	Sequence : 자식노드 중 하나가 실패하면 실행 중지
	*/
	enum TYPE { COMP_SELECT, COMP_SEQUENCE };

private:
	explicit CCompositeTask(CBehaviorTree* pBehaviorTree, TYPE eType);
	virtual ~CCompositeTask() = default;

public:
	CTask* Add_ChildTask(CTask* pTask);

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;
	virtual void	OnEnd() override;
	virtual _int	OnNotify() override;

private:
	_int OnUpdate_Select(_double TimeDelta);
	_int OnUpdate_Sequence(_double TimeDelta);
	_int OnUpdate_Tasks(_double TimeDelta);

public:
	static CCompositeTask* Create(CBehaviorTree* pBehaviorTree, TYPE eType);
	virtual void Free() override;

private:
	TYPE			m_eType = COMP_SELECT;
	vector<CTask*>	m_ChildTasks;
	vector<CTask*>	m_TaskStack;
};
END