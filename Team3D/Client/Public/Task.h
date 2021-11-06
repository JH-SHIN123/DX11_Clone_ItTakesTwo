#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "BehaviorTree.h"

BEGIN(Client)
class CTask abstract : public CBase {
public:
	enum RETURN_TYPE { RETURN_TRUE = 1, RETURN_FALSE = 0, RETURN_RUNNING = -1 };
	enum TASK_STATE { STATE_START, STATE_RUNNING, STATE_STOP };

protected:
	explicit CTask(CBehaviorTree* pBehaviorTree);
	virtual ~CTask() = default;

public:
	TASK_STATE	Get_CurrentState() { return m_eCurrentState; };
	void		Set_CurrentState(TASK_STATE eState) { m_eCurrentState = eState; }

public:
	virtual _int	OnStart() = 0;	/* 스택에 처음 올라갈때 OnStart가 호출 */
	virtual _int	OnUpdate(_double TimeDelta) = 0;	/* true 혹은 false가 반환될때, 스택에서 꺼내지면서 OnEnd 호출 */
	virtual void	OnEnd() = 0;
	virtual _int	OnNotify() = 0;

public:
	virtual void Free() = 0;

protected:
	CBehaviorTree* m_pBehaviorTree = nullptr;
	TASK_STATE		m_eCurrentState = STATE_START; /* 작동중인지, 작동중이지 않는지 */
};
END