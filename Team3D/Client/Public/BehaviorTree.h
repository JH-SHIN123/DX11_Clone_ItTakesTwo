#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CCompositeTask;
class CCharacter;
class CBehaviorTree : public CBase
{
private:
	explicit CBehaviorTree(CCharacter* pCharacter);
	virtual ~CBehaviorTree() = default;

public:
	HRESULT NativeConstruct();
	_uint	Running(_double TimeDelta);

	void	BroadcastMessage(void* pArg); /* Service : BroadcastMessage (모든 Task들에게 메시지를 전송한다.) */

public:
	static CBehaviorTree* Create(CCharacter* pCharacter);
	virtual void Free() override;

private:
	CCompositeTask* m_pRootTask = nullptr;
	CCharacter*		m_pCharacter = nullptr;
};
END

/*
https://lifeisforu.tistory.com/327
https://engineering.linecorp.com/ko/blog/behavior-tree/
https://wergia.tistory.com/147
https://docs.unrealengine.com/4.27/ko/InteractiveExperiences/ArtificialIntelligence/BehaviorTrees/BehaviorTreeNodeReference/BehaviorTreeNodeReferenceComposites/
*/