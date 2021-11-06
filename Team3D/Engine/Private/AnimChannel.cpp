#include "..\public\AnimChannel.h"

CAnimChannel::CAnimChannel()
{
}

const char * CAnimChannel::Get_Name() const
{
	return m_szChannelName;
}

const vector<KEY_FRAME*> & CAnimChannel::Get_KeyFrames() const
{
	return m_KeyFrames;
}

HRESULT CAnimChannel::NativeConstruct(const char * pName)
{
	strcpy_s(m_szChannelName, pName);

	return S_OK;
}

HRESULT CAnimChannel::Bring_KeyFrameContainer(vector<KEY_FRAME*>& KeyFrames)
{
	NULL_CHECK_RETURN(m_KeyFrames.empty(), E_FAIL);

	m_KeyFrames.swap(KeyFrames);

	return S_OK;
}

CAnimChannel * CAnimChannel::Create(const char * pName)
{
	CAnimChannel*	pInstance = new CAnimChannel;

	if (FAILED(pInstance->NativeConstruct(pName)))
	{
		MSG_BOX("Failed to Creating Instance(CAnimChannel).");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimChannel::Free()
{
	for (auto& pFrame : m_KeyFrames)
		Safe_Delete(pFrame);
	m_KeyFrames.clear();
}