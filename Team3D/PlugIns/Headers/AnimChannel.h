#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimChannel final : public CBase
{
private:
	explicit CAnimChannel() = default;
	virtual ~CAnimChannel() = default;

public: /* Struct */
	typedef struct tagChannelDesc
	{
		_uint	iConnectedNodeIndex;
		char	szChannelName[MAX_PATH];
	}CHANNEL_DESC;

public: /* Getter */
	const char*					Get_Name() const { return m_ChannelDesc.szChannelName; }
	const _uint					Get_ConnectedNodeIndex() const { return m_ChannelDesc.iConnectedNodeIndex; }
	const vector<KEY_FRAME>&	Get_KeyFrames() const { return m_KeyFrames; }

public:
	HRESULT	NativeConstruct(CHANNEL_DESC ChannelDesc);
	HRESULT	Bring_KeyFrameContainer(vector<KEY_FRAME>& KeyFrames);

private: /* Typedef */
	typedef vector<KEY_FRAME> KEYFRAMES;
private:
	KEYFRAMES		m_KeyFrames;
	CHANNEL_DESC	m_ChannelDesc;

public:
	static CAnimChannel* Create(CHANNEL_DESC ChannelDesc);
	virtual void Free() override;
};

END