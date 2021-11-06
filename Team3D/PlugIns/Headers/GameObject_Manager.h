#pragma once

#include "Layer.h"

BEGIN(Engine)

class CGameObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CGameObject_Manager)
private:
	explicit CGameObject_Manager() = default;
	virtual ~CGameObject_Manager() = default;

public: /* Getter */
	void Bring_LayerGameObjects(_uint iLayerLevelIndex, const _tchar* pLayerTag, list<CGameObject*>& GameObjectList);

public:
	HRESULT	Reserve_Container(_uint iLevelCount);
	HRESULT	Add_GameObject_Prototype(_uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT	Add_GameObject_Clone(_uint iLayerLevelIndex, const _tchar* pLayerTag, _uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, void* pArg, CGameObject** ppOut);
	_int	Tick(_double dTimeDelta);
	_int	Late_Tick(_double dTimeDelta);
	void	Clear(_uint iLevelIndex);
	void	Clear_All();
	void	Clear_Layer(_uint iLayerLevelIndex, const _tchar* pLayerTag);

private: /* Typedef */
	typedef unordered_map<const _tchar*, CGameObject*>	PROTOTYPES;
	typedef unordered_map<const _tchar*, CLayer*>		LAYERS;
private:
	_uint			m_iLevelCount = 0;
	PROTOTYPES*		m_pPrototypes = nullptr;
	LAYERS*			m_pLayers = nullptr;
private:
	CGameObject*	Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);
	CLayer*			Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END

/*
	Clone 시 클론 오브젝트의 주소를 내보낼 수 있는 기능 추가.
	> 레퍼런스 카운트 증가됨.
*/