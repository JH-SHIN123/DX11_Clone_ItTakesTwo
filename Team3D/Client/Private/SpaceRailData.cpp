#include "stdafx.h"
#include "SpaceRailData.h"
#include "SpaceRail_Node.h"

IMPLEMENT_SINGLETON(CSpaceRailData)

const vector<CSpaceRail_Node*>* CSpaceRailData::Get_RailNodes(const _tchar* pRailTag) const
{
	auto& iter = find_if(m_AllRailsNode.begin(), m_AllRailsNode.end(), CTagFinder(pRailTag));

	if (m_AllRailsNode.end() == iter) return nullptr;

	return &iter->second;
}

HRESULT CSpaceRailData::Load_SpaceRail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pRailTag, const _tchar* pPath)
{
	auto& iter = find_if(m_AllRailsNode.begin(), m_AllRailsNode.end(), CTagFinder(pRailTag));
	if (iter != m_AllRailsNode.end()) return E_FAIL;
	else m_AllRailsNode.emplace(pRailTag, vector<CSpaceRail_Node*>());

	HANDLE hFile = CreateFile(pPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD		dwByte;

	_uint iIndex = 0;
	_float4x4 WorldMat = MH_XMFloat4x4Identity();
	CSpaceRail_Node* pRailNode = nullptr;
	CSpaceRail_Node::SPACERAILNODE_DESC tDesc;

	while (true)
	{
		ReadFile(hFile, &WorldMat, sizeof(_float4x4), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		tDesc.iIndex = iIndex;
		lstrcpy(tDesc.szRailTag, pRailTag);
		tDesc.WorldMatrix = WorldMat;

		pRailNode = CSpaceRail_Node::Create(pDevice, pDeviceContext, &tDesc);
		m_AllRailsNode[pRailTag].push_back(pRailNode);

		++iIndex;
	}
	CloseHandle(hFile);

	return S_OK;
}

void CSpaceRailData::Free()
{
	for (auto& vecRail : m_AllRailsNode)
	{
		for (auto& pRail : vecRail.second)
		{
			Safe_Release(pRail);
		}
		vecRail.second.clear();
	}
	m_AllRailsNode.clear();
}
