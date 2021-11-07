#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimChannel final : public CBase
{
private:
	explicit CAnimChannel() = default;
	virtual ~CAnimChannel() = default;

public: /* Getter */
	const char*					Get_Name() const { return m_szChannelName; }
	const vector<KEY_FRAME*>&	Get_KeyFrames() const { return m_KeyFrames; }
	const _uint					Get_ConnectedNodeIndex() const { return m_iConnectedNodeIndex; }

public: /* Setter */
	void Set_ConnectedNodeIndex(_uint iIndex) { m_iConnectedNodeIndex = iIndex; }

public:
	HRESULT	NativeConstruct(const char* pName);
	HRESULT	Bring_KeyFrameContainer(vector<KEY_FRAME*>& KeyFrames);

private: /* Typedef */
	typedef vector<KEY_FRAME*> KEYFRAMES;
private:
	KEYFRAMES	m_KeyFrames;
	char		m_szChannelName[MAX_PATH] = "";
	_uint		m_iConnectedNodeIndex = 0;

public:
	static CAnimChannel* Create(const char* pName);
	virtual void Free() override;
};

END