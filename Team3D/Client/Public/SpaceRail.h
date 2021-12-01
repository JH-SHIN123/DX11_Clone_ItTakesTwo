#pragma once

#include "Client_Defines.h"
#include "Dynamic_Env.h"

BEGIN(Engine)
class CStaticActor;
class CPath;
END

BEGIN(Client)
class CSpaceRail_Node;
class CSpaceRail : public CDynamic_Env
{
private:
	explicit CSpaceRail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpaceRail(const CSpaceRail& rhs);
	virtual ~CSpaceRail() = default;

public:
	HRESULT Start_Path(CPath::STATE eState, _uint iAnimFrame);
	_bool	Take_Path(_double dTimeDelta, _matrix& WorldMatrix);

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CPath*			m_pPathCom = nullptr;

private:
	vector<CSpaceRail_Node*>	m_vecSpaceRailNodes;
	_tchar						m_szRailTag[MAX_PATH] = L"";
	_uint						m_iPerNodesInteract = 1;

public:
	static CSpaceRail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END
