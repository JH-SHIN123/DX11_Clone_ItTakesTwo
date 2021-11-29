#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CSpaceRail_Node;
class CSpaceRailData final : public CBase
{
	DECLARE_SINGLETON(CSpaceRailData)
private:
	explicit CSpaceRailData() = default;
	virtual ~CSpaceRailData() = default;

public:
	const vector<CSpaceRail_Node*>* Get_RailNodes(const _tchar* pRailTag) const;

public:
	HRESULT Load_SpaceRail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pRailTag, const _tchar* pPath);

private:
	unordered_map<const _tchar*, vector<CSpaceRail_Node*>> m_AllRailsNode;

public:
	virtual void Free() override;
};
END

