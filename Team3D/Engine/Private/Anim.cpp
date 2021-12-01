#include "..\public\Anim.h"
#include "AnimChannel.h"

void CAnim::Get_PreAnimKeyFrames(_uint iPreAnimFrame, vector<KEY_FRAME>& PreAnimKeyFrames)
{
	for (auto& pChannel : m_Channels)
		PreAnimKeyFrames[pChannel->Get_ConnectedNodeIndex()] = pChannel->Get_KeyFrames()[iPreAnimFrame];
}

const _double CAnim::Get_ChannelKeyFrameTimeAvg(_int iIndex)
{
	if (iIndex < 0) return 0.0;

	_double FrameTime = 0.0;

	for (auto& pChannel : m_Channels)
		FrameTime += pChannel->Get_KeyFrameTime(iIndex);

	FrameTime /= m_Channels.size();

	return FrameTime;
}

HRESULT CAnim::NativeConstruct(ANIM_DESC AnimDesc)
{
	memcpy(&m_AnimDesc, &AnimDesc, sizeof(ANIM_DESC));

	return S_OK;
}

HRESULT CAnim::Bring_ChannelContainer(vector<class CAnimChannel*> & Channels)
{
	NULL_CHECK_RETURN(m_Channels.empty(), E_FAIL);

	m_Channels.swap(Channels);

	return S_OK;
}

HRESULT CAnim::Update_Transformations(_double & dCurrentTime, _uint & iCurAnimFrame, vector<_float4x4> & Transformations)
{
	for (auto& pChannel : m_Channels)
	{
		_uint iNodeIndex = pChannel->Get_ConnectedNodeIndex();

		const vector<KEY_FRAME>& KeyFrames = pChannel->Get_KeyFrames();

		KEY_FRAME	pFirst = KeyFrames.front();
		KEY_FRAME	pLast = KeyFrames.back();

		_vector vScale, vRotation, vPosition;

		if (dCurrentTime < pFirst.dTime)
		{
			vScale = XMLoadFloat3(&pFirst.vScale);
			vRotation = XMLoadFloat4(&pFirst.vRotation);
			vPosition = XMLoadFloat3(&pFirst.vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else if (dCurrentTime >= pLast.dTime)
		{
			vScale = XMLoadFloat3(&pLast.vScale);
			vRotation = XMLoadFloat4(&pLast.vRotation);
			vPosition = XMLoadFloat3(&pLast.vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else
		{
			if (dCurrentTime > KeyFrames[iCurAnimFrame + 1].dTime)
				++iCurAnimFrame;

			_float fRatio = _float((dCurrentTime - KeyFrames[iCurAnimFrame].dTime) / (KeyFrames[iCurAnimFrame + 1].dTime - KeyFrames[iCurAnimFrame].dTime));

			_vector	vSrcScale, vDstScale;
			_vector	vSrcRotation, vDstRotation;
			_vector	vSrcPosition, vDstPosition;

			vSrcScale = XMLoadFloat3(&KeyFrames[iCurAnimFrame].vScale);
			vSrcRotation = XMLoadFloat4(&KeyFrames[iCurAnimFrame].vRotation);
			vSrcPosition = XMLoadFloat3(&KeyFrames[iCurAnimFrame].vPosition);
			vSrcPosition = XMVectorSetW(vSrcPosition, 1.f);

			vDstScale = XMLoadFloat3(&KeyFrames[iCurAnimFrame + 1].vScale);
			vDstRotation = XMLoadFloat4(&KeyFrames[iCurAnimFrame + 1].vRotation);
			vDstPosition = XMLoadFloat3(&KeyFrames[iCurAnimFrame + 1].vPosition);
			vDstPosition = XMVectorSetW(vSrcPosition, 1.f);

			vScale = XMVectorLerp(vSrcScale, vDstScale, fRatio);
			vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
			vPosition = XMVectorLerp(vSrcPosition, vDstPosition, fRatio);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		XMStoreFloat4x4(&Transformations[iNodeIndex], XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));
	}

	return S_OK;
}

HRESULT CAnim::Update_Transformations_Blend(_double & dCurrentTime, _uint & iCurAnimFrame, vector<_float4x4>& Transformations, vector<KEY_FRAME>& PreAnimKeyFrames, _float fLerpRatio)
{
	for (auto& pChannel : m_Channels)
	{
		_uint iNodeIndex = pChannel->Get_ConnectedNodeIndex();

		const vector<KEY_FRAME>& KeyFrames = pChannel->Get_KeyFrames();

		KEY_FRAME	pFirst = KeyFrames.front();
		KEY_FRAME	pLast = KeyFrames.back();

		_vector vScale, vRotation, vPosition;

		if (dCurrentTime < pFirst.dTime)
		{
			vScale = XMLoadFloat3(&pFirst.vScale);
			vRotation = XMLoadFloat4(&pFirst.vRotation);
			vPosition = XMLoadFloat3(&pFirst.vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else if (dCurrentTime >= pLast.dTime)
		{
			vScale = XMLoadFloat3(&pLast.vScale);
			vRotation = XMLoadFloat4(&pLast.vRotation);
			vPosition = XMLoadFloat3(&pLast.vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else
		{
			if (dCurrentTime > KeyFrames[iCurAnimFrame + 1].dTime)
				++iCurAnimFrame;

			_float fRatio = _float((dCurrentTime - KeyFrames[iCurAnimFrame].dTime) / (KeyFrames[iCurAnimFrame + 1].dTime - KeyFrames[iCurAnimFrame].dTime));

			_vector	vSrcScale, vDstScale;
			_vector	vSrcRotation, vDstRotation;
			_vector	vSrcPosition, vDstPosition;

			vSrcScale = XMLoadFloat3(&KeyFrames[iCurAnimFrame].vScale);
			vSrcRotation = XMLoadFloat4(&KeyFrames[iCurAnimFrame].vRotation);
			vSrcPosition = XMLoadFloat3(&KeyFrames[iCurAnimFrame].vPosition);
			vSrcPosition = XMVectorSetW(vSrcPosition, 1.f);

			vDstScale = XMLoadFloat3(&KeyFrames[iCurAnimFrame + 1].vScale);
			vDstRotation = XMLoadFloat4(&KeyFrames[iCurAnimFrame + 1].vRotation);
			vDstPosition = XMLoadFloat3(&KeyFrames[iCurAnimFrame + 1].vPosition);
			vDstPosition = XMVectorSetW(vSrcPosition, 1.f);

			vScale = XMVectorLerp(vSrcScale, vDstScale, fRatio);
			vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
			vPosition = XMVectorLerp(vSrcPosition, vDstPosition, fRatio);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}

		if (true == PreAnimKeyFrames[iNodeIndex].bActive)
		{
			vScale = XMVectorLerp(vScale, XMLoadFloat3(&PreAnimKeyFrames[iNodeIndex].vScale), fLerpRatio);
			vRotation = XMVectorLerp(vRotation, XMLoadFloat4(&PreAnimKeyFrames[iNodeIndex].vRotation), fLerpRatio);
			vPosition = XMVectorLerp(vPosition, XMLoadFloat3(&PreAnimKeyFrames[iNodeIndex].vPosition), fLerpRatio);
		}

		XMStoreFloat4x4(&Transformations[iNodeIndex], XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));
	}

	return S_OK;
}

HRESULT CAnim::Update_PathTransformation(_double& dCurrentTime, _uint& iCurAnimFrame, vector<_float4x4>& Transformations, _uint iFrameInteract)
{
	_uint iIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		const vector<KEY_FRAME>& KeyFrames = pChannel->Get_KeyFrames();
		if (KeyFrames.size() <= iCurAnimFrame + 1) continue;

		KEY_FRAME pFirst = KeyFrames.front();
		KEY_FRAME pLast = KeyFrames.back();

		_vector vScale, vRotation, vPosition;

		if (dCurrentTime < pFirst.dTime)
		{
			vScale = XMLoadFloat3(&pFirst.vScale);
			vRotation = XMLoadFloat4(&pFirst.vRotation);
			vPosition = XMLoadFloat3(&pFirst.vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else if (dCurrentTime >= pLast.dTime)
		{
			vScale = XMLoadFloat3(&pLast.vScale);
			vRotation = XMLoadFloat4(&pLast.vRotation);
			vPosition = XMLoadFloat3(&pLast.vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else
		{
			if (dCurrentTime > KeyFrames[iCurAnimFrame + 1].dTime)
				iCurAnimFrame += iFrameInteract;

			_float fRatio = _float((dCurrentTime - KeyFrames[iCurAnimFrame].dTime) / (KeyFrames[iCurAnimFrame + 1].dTime - KeyFrames[iCurAnimFrame].dTime));

			_vector	vSrcScale, vDstScale;
			_vector	vSrcRotation, vDstRotation;
			_vector	vSrcPosition, vDstPosition;

			vSrcScale = XMLoadFloat3(&KeyFrames[iCurAnimFrame].vScale);
			vSrcRotation = XMLoadFloat4(&KeyFrames[iCurAnimFrame].vRotation);
			vSrcPosition = XMLoadFloat3(&KeyFrames[iCurAnimFrame].vPosition);
			vSrcPosition = XMVectorSetW(vSrcPosition, 1.f);

			vDstScale = XMLoadFloat3(&KeyFrames[iCurAnimFrame + 1].vScale);
			vDstRotation = XMLoadFloat4(&KeyFrames[iCurAnimFrame + 1].vRotation);
			vDstPosition = XMLoadFloat3(&KeyFrames[iCurAnimFrame + 1].vPosition);
			vDstPosition = XMVectorSetW(vSrcPosition, 1.f);

			vScale = XMVectorLerp(vSrcScale, vDstScale, fRatio);
			vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
			vPosition = XMVectorLerp(vSrcPosition, vDstPosition, fRatio);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		XMStoreFloat4x4(&Transformations[iIndex++], XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));
	}

	return S_OK;
}

HRESULT CAnim::Update_RewindPathTransformation(_double& dCurrentTime, _uint& iCurAnimFrame, vector<_float4x4>& Transformations, _uint iFrameInteract)
{
	_uint iIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		const vector<KEY_FRAME>& KeyFrames = pChannel->Get_KeyFrames();
		_int iKeyFrameSize = (_int)pChannel->Get_KeyFrameCount() - 1;

		_int iCurFrame = iKeyFrameSize - iCurAnimFrame;
		_int ilNextFrame = iKeyFrameSize - iCurAnimFrame - 1;

		if (0 >= ilNextFrame) continue;

		KEY_FRAME pFirst = KeyFrames.front();
		KEY_FRAME pLast = KeyFrames.back();

		_vector vScale, vRotation, vPosition;

		if (dCurrentTime < pFirst.dTime)
		{
			vScale = XMLoadFloat3(&pFirst.vScale);
			vRotation = XMLoadFloat4(&pFirst.vRotation);
			vPosition = XMLoadFloat3(&pFirst.vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else if (dCurrentTime >= pLast.dTime)
		{
			vScale = XMLoadFloat3(&pLast.vScale);
			vRotation = XMLoadFloat4(&pLast.vRotation);
			vPosition = XMLoadFloat3(&pLast.vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else
		{
			if (dCurrentTime < KeyFrames[ilNextFrame].dTime)
				iCurAnimFrame += iFrameInteract;

			_float fRatio = fabsf(_float((dCurrentTime - KeyFrames[iCurFrame].dTime) / (KeyFrames[ilNextFrame].dTime - KeyFrames[iCurFrame].dTime)));

			_vector	vSrcScale, vDstScale;
			_vector	vSrcRotation, vDstRotation;
			_vector	vSrcPosition, vDstPosition;

			vSrcScale = XMLoadFloat3(&KeyFrames[iCurFrame].vScale);
			vSrcRotation = XMLoadFloat4(&KeyFrames[iCurFrame].vRotation);
			vSrcPosition = XMLoadFloat3(&KeyFrames[iCurFrame].vPosition);
			vSrcPosition = XMVectorSetW(vSrcPosition, 1.f);

			vDstScale = XMLoadFloat3(&KeyFrames[ilNextFrame].vScale);
			vDstRotation = XMLoadFloat4(&KeyFrames[ilNextFrame].vRotation);
			vDstPosition = XMLoadFloat3(&KeyFrames[ilNextFrame].vPosition);
			vDstPosition = XMVectorSetW(vSrcPosition, 1.f);

			vScale = XMVectorLerp(vSrcScale, vDstScale, fRatio);
			vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
			vPosition = XMVectorLerp(vSrcPosition, vDstPosition, fRatio);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		XMStoreFloat4x4(&Transformations[iIndex++], XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));
	}

	return S_OK;
}

CAnim * CAnim::Create(ANIM_DESC AnimDesc)
{
	CAnim* pInstance = new CAnim;

	if (FAILED(pInstance->NativeConstruct(AnimDesc)))
	{
		MSG_BOX("Failed to Create Instance - CAnim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnim::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}