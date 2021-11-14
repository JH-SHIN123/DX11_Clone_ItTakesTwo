#include "..\public\AnimChannel.h"

HRESULT CAnimChannel::NativeConstruct(CHANNEL_DESC ChannelDesc)
{
	memcpy(&m_ChannelDesc, &ChannelDesc, sizeof(CHANNEL_DESC));

	return S_OK;
}

HRESULT CAnimChannel::Bring_KeyFrameContainer(vector<KEY_FRAME> & KeyFrames)
{
	NULL_CHECK_RETURN(m_KeyFrames.empty(), E_FAIL);

	m_KeyFrames.swap(KeyFrames);

	return S_OK;
}

CAnimChannel * CAnimChannel::Create(CHANNEL_DESC ChannelDesc)
{
	CAnimChannel* pInstance = new CAnimChannel;

	if (FAILED(pInstance->NativeConstruct(ChannelDesc)))
	{
		MSG_BOX("Failed to Create Instance - CAnimChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimChannel::Free()
{
	m_KeyFrames.clear();
}