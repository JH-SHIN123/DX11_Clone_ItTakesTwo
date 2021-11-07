#pragma once

#include "ActionTask.h"

BEGIN(Client)
#pragma region Idle
class CActionTask_Cody_Idle : public CActionTask
{
public:
	explicit CActionTask_Cody_Idle(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Cody_Idle() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;

public:
	static CActionTask_Cody_Idle* Create(CBehaviorTree* pBehaviorTree);

private:
	_double m_dWaitingDeltaT = 0.0;
};

class CActionTask_Cody_BigWaiting_Enter : public CActionTask
{
public:
	explicit CActionTask_Cody_BigWaiting_Enter(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Cody_BigWaiting_Enter() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;

public:
	static CActionTask_Cody_BigWaiting_Enter* Create(CBehaviorTree* pBehaviorTree);

private:
	_double m_dRunningDeltaT = 0.0;
};

class CActionTask_Cody_BigWaiting : public CActionTask
{
public:
	explicit CActionTask_Cody_BigWaiting(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Cody_BigWaiting() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;

public:
	static CActionTask_Cody_BigWaiting* Create(CBehaviorTree* pBehaviorTree);
};
#pragma endregion

#pragma region Jog
class CActionTask_Cody_Jog_StartInMotion : public CActionTask
{
public:
	explicit CActionTask_Cody_Jog_StartInMotion(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Cody_Jog_StartInMotion() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;

public:
	static CActionTask_Cody_Jog_StartInMotion* Create(CBehaviorTree* pBehaviorTree);
};

class CActionTask_Cody_Jog_StartFwd : public CActionTask
{
public:
	explicit CActionTask_Cody_Jog_StartFwd(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Cody_Jog_StartFwd() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;

public:
	static CActionTask_Cody_Jog_StartFwd* Create(CBehaviorTree* pBehaviorTree);

private:
	_double m_dRunningTime = 0.0;
};

class CActionTask_Cody_Jog_StartFwd_RightLeft180 : public CActionTask
{
public:
	explicit CActionTask_Cody_Jog_StartFwd_RightLeft180(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Cody_Jog_StartFwd_RightLeft180() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;

public:
	static CActionTask_Cody_Jog_StartFwd_RightLeft180* Create(CBehaviorTree* pBehaviorTree);
};

class CActionTask_Cody_Jog_Fwd_Left : public CActionTask
{
public:
	explicit CActionTask_Cody_Jog_Fwd_Left(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Cody_Jog_Fwd_Left() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;

public:
	static CActionTask_Cody_Jog_Fwd_Left* Create(CBehaviorTree* pBehaviorTree);
};

class CActionTask_Cody_Jog_Fwd_Right : public CActionTask
{
public:
	explicit CActionTask_Cody_Jog_Fwd_Right(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Cody_Jog_Fwd_Right() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;

public:
	static CActionTask_Cody_Jog_Fwd_Right* Create(CBehaviorTree* pBehaviorTree);
};

class CActionTask_Cody_Jog_Fwd : public CActionTask
{
public:
	explicit CActionTask_Cody_Jog_Fwd(CBehaviorTree* pBehaviorTree) : CActionTask(pBehaviorTree) {};
	virtual ~CActionTask_Cody_Jog_Fwd() = default;

public:
	virtual _int	OnStart() override;
	virtual _int	OnUpdate(_double TimeDelta) override;

public:
	static CActionTask_Cody_Jog_Fwd* Create(CBehaviorTree* pBehaviorTree);
};
#pragma endregion 



END