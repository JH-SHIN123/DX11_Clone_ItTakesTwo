#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLayer final : public CBase
{
public:
	explicit CLayer() = default;
	virtual ~CLayer() = default;

public: /* Getter */
	void Bring_GameObjectList(list<CGameObject*>& GameObjectList);
	list<CGameObject*>& Get_ObjectList() { return m_Objects; }

public:
	HRESULT	NativeConstruct();
	HRESULT	Add_GameObject(class CGameObject* pGameObject);
	_int	Tick(_double TimeDelta);
	_int	Late_Tick(_double TimeDelta);
	void	Remove_DeadObject();

private:
	list<CGameObject*>		m_Objects;
	vector<CGameObject*>	m_DeadObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END