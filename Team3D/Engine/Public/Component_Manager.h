#pragma once

#include "Model.h"
#include "Model_Instance.h"
#include "Renderer.h"
#include "Textures.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Color.h"
#include "VIBuffer_Sprite.h"
#include "VIBuffer_RectRHW.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_RectInstance.h"
#include "VIBuffer_PointInstance_Custom_ST.h"
#include "VIBuffer_RectInstance_Custom.h"
#include "VIBuffer_Rect_TripleUV.h"
#include "VIBuffer_PointInstance_Custom_STT.h"

#include "VIBuffer_FontInstance.h"
#include "VIBuffer_PointInstance.h"

#include "StaticActor.h"
#include "DynamicActor.h"
#include "TriggerActor.h"

#include "Path.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	explicit CComponent_Manager() = default;
	virtual ~CComponent_Manager() = default;

public:
	HRESULT		Reserve_Container(_uint iLevelCount);
	HRESULT		Add_Component_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pComponent);
	CComponent*	Add_Component_Clone(_uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	void		Clear(_uint iLevelIndex);
	void		Clear_All();

private: /* Typedef */
	typedef unordered_map<const _tchar*, CComponent*> PROTOTYPES;
private:
	PROTOTYPES*	m_pPrototypes = nullptr;
	_uint		m_iLevelCount = 0;
private:
	CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END