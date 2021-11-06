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

#pragma region Locomotion
class CActionTask_SprintStart final : public CActionTask
{
public:
	explicit CActionTask_SprintStart(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_SprintStart() = default;

public:
	virtual _int	OnUpdate(_double TimeDelta) override;
	virtual void	OnEnd() override;
	virtual _int	OnNotify() override { return 0; };

public:
	static CActionTask_SprintStart* Create(CBehaviorTree* pBehaviorTree);
};

class CActionTask_Sprint final : public CActionTask
{
public:
	explicit CActionTask_Sprint(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Sprint() = default;

public:
	virtual _int	OnUpdate(_double TimeDelta) override;
	virtual void	OnEnd() override;
	virtual _int	OnNotify() override { return 0; };

public:
	static CActionTask_Sprint* Create(CBehaviorTree* pBehaviorTree);
};

class CActionTask_JogStart final : public CActionTask
{
public:
	explicit CActionTask_JogStart(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_JogStart() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;
	virtual void	OnEnd() override;
	virtual _int	OnNotify() override { return 0; };

public:
	static CActionTask_JogStart* Create(CBehaviorTree* pBehaviorTree);

private:
	_bool	m_bRunning = false;
	_double m_dExitDeltaT = 0.0;
};

class CActionTask_Jog final : public CActionTask
{
public:
	explicit CActionTask_Jog(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Jog() = default;

public:
	virtual _int	OnUpdate(_double TimeDelta) override;
	virtual void	OnEnd() override;
	virtual _int	OnNotify() override { return 0; };

public:
	static CActionTask_Jog* Create(CBehaviorTree* pBehaviorTree);
};

class CActionTask_Stop final : public CActionTask
{
public:
	explicit CActionTask_Stop(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Stop() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;
	virtual void	OnEnd() override;
	virtual _int	OnNotify() override { return 0; };

public:
	static CActionTask_Stop* Create(CBehaviorTree* pBehaviorTree);

private:
	_bool	m_bRunning = false;
	_double m_dExitDeltaT = 0.0;
};

class CActionTask_Idle final : public CActionTask
{
public:
	explicit CActionTask_Idle(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Idle() = default;

public:
	virtual _int	OnUpdate(_double TimeDelta) override;
	virtual void	OnEnd() override;
	virtual _int	OnNotify() override { return 0; };

public:
	static CActionTask_Idle* Create(CBehaviorTree* pBehaviorTree);
};
#pragma endregion
END