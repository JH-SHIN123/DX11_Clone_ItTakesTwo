#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CCompositeTask;
class CBehaviorTree abstract : public CBase
{
protected:
	explicit CBehaviorTree(CGameObject* pSubject);
	virtual ~CBehaviorTree() = default;

public:
	class CGameObject* Get_Subject() const { return m_pSubject; };

public:
	virtual HRESULT NativeConstruct() = 0;

	_uint	Running(_double TimeDelta);
	void	BroadcastMessage(void* pArg); /* Service : BroadcastMessage (모든 Task들에게 메시지를 전송한다.) */

public:
	virtual void Free() override;

protected:
	CCompositeTask* m_pRootTask = nullptr;
	CGameObject*	m_pSubject	= nullptr;
};
END

/*
https://lifeisforu.tistory.com/327
https://engineering.linecorp.com/ko/blog/behavior-tree/
https://wergia.tistory.com/147
https://docs.unrealengine.com/4.27/ko/InteractiveExperiences/ArtificialIntelligence/BehaviorTrees/BehaviorTreeNodeReference/BehaviorTreeNodeReferenceComposites/
*/