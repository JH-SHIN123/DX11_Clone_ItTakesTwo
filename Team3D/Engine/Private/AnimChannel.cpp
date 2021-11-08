#include "..\public\AnimChannel.h"

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
	CAnimChannel* pInstance = new CAnimChannel;

	if (FAILED(pInstance->NativeConstruct(pName)))
	{
		MSG_BOX("Failed to Create Instance - CAnimChannel");
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