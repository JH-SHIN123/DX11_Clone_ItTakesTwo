#include "..\public\AnimChannel.h"

_double CAnimChannel::Get_KeyFrameTime(_int iIndex) const
{
	if (iIndex < 0 || iIndex >= m_KeyFrames.size()) return 0.0;

	return m_KeyFrames[iIndex].dTime;
}

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