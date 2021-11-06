#pragma once

#include "Client_Defines.h"
#include "BehaviorTree.h"

BEGIN(Client)
class CBehaviorTree_Cody final : public CBehaviorTree
{
protected:
	explicit CBehaviorTree_Cody(CGameObject* pSubject);
	virtual ~CBehaviorTree_Cody() = default;

public:
	HRESULT NativeConstruct();

public:
	static CBehaviorTree_Cody* Create(CGameObject* pSubject);
	virtual void Free() override;
};
END
