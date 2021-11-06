#pragma once

#include "Task.h"

BEGIN(Client)
/* 실제 행동을 표현하는 단말노드 */
/* ActionTask에서 DecorateTask가 수행하는 작업또한 포함시킬거임. */
class CActionTask abstract : public CTask
{
public:
	explicit CActionTask(CBehaviorTree* pBehaviorTree) : CTask(pBehaviorTree) {};
	virtual ~CActionTask() = default;

public:
	virtual _int	OnStart() override { return RETURN_TRUE; };
	virtual _int	OnUpdate(_double TimeDelta) override { return RETURN_TYPE::RETURN_TRUE; };
	virtual void	OnEnd() override {}
	virtual _int	OnNotify() override { return 0; };

public:
	virtual void Free() override {};
};
END