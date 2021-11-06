#include "..\public\Anim.h"
#include "AnimChannel.h"

CAnim::CAnim()
{
}

const _double & CAnim::Get_Duration() const
{
	return m_dDuration;
}

const _double & CAnim::Get_TicksPerSecond() const
{
	return m_dTicksPerSecond;
}

HRESULT CAnim::NativeConstruct(const char * pName, const _double & dDuration, const _double & dTicksPerSecond)
{
	strcpy_s(m_szAnimName, pName);

	m_dDuration			= dDuration;
	m_dTicksPerSecond	= dTicksPerSecond;

	return S_OK;
}

HRESULT CAnim::Bring_ChannelContainer(vector<class CAnimChannel*>& Channels)
{
	NULL_CHECK_RETURN(m_Channels.empty(), E_FAIL);

	m_Channels.swap(Channels);

	return S_OK;
}

HRESULT CAnim::Update_ChannelMatrices(_double & dCurrentTime, vector<_uint> & CurrentKeyFrames, vector<_float4x4> & Transformations, vector<_uint> & LastKeyFrames, CAnim* pPreAnim, _double dPreStopTime, _double dPreAnimDuration, _double dPreAnimTime)
{
	_uint iChannelIndex = 0;

	if (nullptr == pPreAnim || 0.0 == dPreAnimDuration || 0.0 == dPreAnimTime)
	{
		for (auto& pAnimChannel : m_Channels)
		{
			const vector<KEY_FRAME*>&	KeyFrames = pAnimChannel->Get_KeyFrames();

			KEY_FRAME*	pFirst = KeyFrames.front();
			KEY_FRAME*	pLast = KeyFrames.back();

			_vector		vScale, vRotation, vPosition;

			if (dCurrentTime < pFirst->dTime)
			{
				vScale = XMLoadFloat3(&pFirst->vScale);
				vRotation = XMLoadFloat4(&pFirst->vRotation);
				vPosition = XMLoadFloat3(&pFirst->vPosition);
				vPosition = XMVectorSetW(vPosition, 1.f);
			}
			else if (dCurrentTime >= pLast->dTime)
			{
				vScale = XMLoadFloat3(&pLast->vScale);
				vRotation = XMLoadFloat4(&pLast->vRotation);
				vPosition = XMLoadFloat3(&pLast->vPosition);
				vPosition = XMVectorSetW(vPosition, 1.f);
			}
			else
			{
				_uint& iCurrentKeyFrame = CurrentKeyFrames[iChannelIndex];

				if (dCurrentTime > KeyFrames[iCurrentKeyFrame + 1]->dTime)
					++iCurrentKeyFrame;

				_float	fRatio = _float((dCurrentTime - KeyFrames[iCurrentKeyFrame]->dTime) / (KeyFrames[iCurrentKeyFrame + 1]->dTime - KeyFrames[iCurrentKeyFrame]->dTime));

				_vector	vSrcScale, vDstScale;
				_vector	vSrcRotation, vDstRotation;
				_vector	vSrcPosition, vDstPosition;

				vSrcScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vScale);
				vSrcRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame]->vRotation);
				vSrcPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vPosition);
				vSrcPosition = XMVectorSetW(vSrcPosition, 1.f);

				vDstScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vScale);
				vDstRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame + 1]->vRotation);
				vDstPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vPosition);
				vDstPosition = XMVectorSetW(vSrcPosition, 1.f);

				vScale = XMVectorLerp(vSrcScale, vDstScale, fRatio);
				vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
				vPosition = XMVectorLerp(vSrcPosition, vDstPosition, fRatio);
				vPosition = XMVectorSetW(vPosition, 1.f);
			}
			_matrix	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

			XMStoreFloat4x4(&Transformations[iChannelIndex], TransformationMatrix);

			++iChannelIndex;
		}
	}
	else if (dPreStopTime == -1.0)
	{
		for (_uint iIndex = 0; iIndex < m_Channels.size(); ++iIndex)
		{
			CAnimChannel*	pAnimChannel = m_Channels[iIndex];

			const vector<KEY_FRAME*>&	KeyFrames = pAnimChannel->Get_KeyFrames();

			KEY_FRAME*	pFirst = KeyFrames.front();
			KEY_FRAME*	pLast = KeyFrames.back();

			_vector		vScale, vRotation, vPosition;

			if (dCurrentTime < pFirst->dTime)
			{
				vScale = XMLoadFloat3(&pFirst->vScale);
				vRotation = XMLoadFloat4(&pFirst->vRotation);
				vPosition = XMLoadFloat3(&pFirst->vPosition);
				vPosition = XMVectorSetW(vPosition, 1.f);
			}
			else if (dCurrentTime >= pLast->dTime)
			{
				vScale = XMLoadFloat3(&pLast->vScale);
				vRotation = XMLoadFloat4(&pLast->vRotation);
				vPosition = XMLoadFloat3(&pLast->vPosition);
				vPosition = XMVectorSetW(vPosition, 1.f);
			}
			else
			{
				_uint& iCurrentKeyFrame = CurrentKeyFrames[iChannelIndex];

				if (dCurrentTime > KeyFrames[iCurrentKeyFrame + 1]->dTime)
					++iCurrentKeyFrame;

				_float	fRatio = _float((dCurrentTime - KeyFrames[iCurrentKeyFrame]->dTime) / (KeyFrames[iCurrentKeyFrame + 1]->dTime - KeyFrames[iCurrentKeyFrame]->dTime));

				_vector	vSrcScale, vDstScale;
				_vector	vSrcRotation, vDstRotation;
				_vector	vSrcPosition, vDstPosition;

				vSrcScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vScale);
				vSrcRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame]->vRotation);
				vSrcPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vPosition);
				vSrcPosition = XMVectorSetW(vSrcPosition, 1.f);

				vDstScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vScale);
				vDstRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame + 1]->vRotation);
				vDstPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vPosition);
				vDstPosition = XMVectorSetW(vSrcPosition, 1.f);

				vScale = XMVectorLerp(vSrcScale, vDstScale, fRatio);
				vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
				vPosition = XMVectorLerp(vSrcPosition, vDstPosition, fRatio);
				vPosition = XMVectorSetW(vPosition, 1.f);
			}


			CAnimChannel*	pPreAnimChannel = pPreAnim->m_Channels[iIndex];
			_float			fConnectionRatio = _float(dPreAnimTime / dPreAnimDuration);

			KEY_FRAME*	pPreLast = pPreAnimChannel->Get_KeyFrames().back();

			_vector		vPreAnimScale		= XMLoadFloat3(&pPreLast->vScale);
			_vector		vPreAnimRotation	= XMLoadFloat4(&pPreLast->vRotation);

			vScale = XMVectorLerp(vScale, vPreAnimScale, fConnectionRatio);
			vRotation = XMQuaternionSlerp(vRotation, vPreAnimRotation, fConnectionRatio);

			_matrix	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

			XMStoreFloat4x4(&Transformations[iChannelIndex], TransformationMatrix);

			++iChannelIndex;
		}
	}
	else
	{
		for (_uint iIndex = 0; iIndex < m_Channels.size(); ++iIndex)
		{
			CAnimChannel*	pAnimChannel = m_Channels[iIndex];

			const vector<KEY_FRAME*>&	KeyFrames = pAnimChannel->Get_KeyFrames();

			KEY_FRAME*	pFirst	= KeyFrames.front();
			KEY_FRAME*	pLast	= KeyFrames.back();

			_vector		vScale, vRotation, vPosition;

			if (dCurrentTime < pFirst->dTime)
			{
				vScale = XMLoadFloat3(&pFirst->vScale);
				vRotation = XMLoadFloat4(&pFirst->vRotation);
				vPosition = XMLoadFloat3(&pFirst->vPosition);
				vPosition = XMVectorSetW(vPosition, 1.f);
			}
			else if (dCurrentTime >= pLast->dTime)
			{
				vScale = XMLoadFloat3(&pLast->vScale);
				vRotation = XMLoadFloat4(&pLast->vRotation);
				vPosition = XMLoadFloat3(&pLast->vPosition);
				vPosition = XMVectorSetW(vPosition, 1.f);
			}
			else
			{
				_uint& iCurrentKeyFrame = CurrentKeyFrames[iChannelIndex];

				if (dCurrentTime > KeyFrames[iCurrentKeyFrame + 1]->dTime)
					++iCurrentKeyFrame;

				_float	fRatio = _float((dCurrentTime - KeyFrames[iCurrentKeyFrame]->dTime) / (KeyFrames[iCurrentKeyFrame + 1]->dTime - KeyFrames[iCurrentKeyFrame]->dTime));

				_vector	vSrcScale, vDstScale;
				_vector	vSrcRotation, vDstRotation;
				_vector	vSrcPosition, vDstPosition;

				vSrcScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vScale);
				vSrcRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame]->vRotation);
				vSrcPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vPosition);
				vSrcPosition = XMVectorSetW(vSrcPosition, 1.f);

				vDstScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vScale);
				vDstRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame + 1]->vRotation);
				vDstPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vPosition);
				vDstPosition = XMVectorSetW(vSrcPosition, 1.f);

				vScale = XMVectorLerp(vSrcScale, vDstScale, fRatio);
				vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
				vPosition = XMVectorLerp(vSrcPosition, vDstPosition, fRatio);
				vPosition = XMVectorSetW(vPosition, 1.f);
			}


			CAnimChannel*	pPreAnimChannel = pPreAnim->m_Channels[iIndex];
			_float			fConnectionRatio = _float(dPreAnimTime / dPreAnimDuration);

			const vector<KEY_FRAME*>&	PreKeyFrames = pPreAnimChannel->Get_KeyFrames();

			KEY_FRAME*	pPreFirst	= PreKeyFrames.front();
			KEY_FRAME*	pPreLast	= PreKeyFrames.back();

			_vector		vPreAnimScale, vPreAnimRotation;

			if (dPreStopTime < pPreFirst->dTime)
			{
				vPreAnimScale = XMLoadFloat3(&pPreFirst->vScale);
				vPreAnimRotation = XMLoadFloat4(&pPreFirst->vRotation);
			}
			else if (dPreStopTime >= pPreLast->dTime)
			{
				vPreAnimScale = XMLoadFloat3(&pPreLast->vScale);
				vPreAnimRotation = XMLoadFloat4(&pPreLast->vRotation);
			}
			else
			{
				_uint& iLastKeyFrame = LastKeyFrames[iChannelIndex];

				if (dPreStopTime > PreKeyFrames[iLastKeyFrame + 1]->dTime)
					++iLastKeyFrame;

				_float	fRatio = _float((dPreStopTime - PreKeyFrames[iLastKeyFrame]->dTime) / (PreKeyFrames[iLastKeyFrame + 1]->dTime - PreKeyFrames[iLastKeyFrame]->dTime));

				_vector	vSrcScale, vDstScale;
				_vector	vSrcRotation, vDstRotation;

				vSrcScale = XMLoadFloat3(&PreKeyFrames[iLastKeyFrame]->vScale);
				vSrcRotation = XMLoadFloat4(&PreKeyFrames[iLastKeyFrame]->vRotation);

				vDstScale = XMLoadFloat3(&PreKeyFrames[iLastKeyFrame + 1]->vScale);
				vDstRotation = XMLoadFloat4(&PreKeyFrames[iLastKeyFrame + 1]->vRotation);

				vPreAnimScale = XMVectorLerp(vSrcScale, vDstScale, fRatio);
				vPreAnimRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
			}

			vScale		= XMVectorLerp(vScale, vPreAnimScale, fConnectionRatio);
			vRotation	= XMQuaternionSlerp(vRotation, vPreAnimRotation, fConnectionRatio);

			_matrix	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

			XMStoreFloat4x4(&Transformations[iChannelIndex], TransformationMatrix);

			++iChannelIndex;
		}
	}

	return S_OK;
}

CAnim * CAnim::Create(const char * pName, const _double & dDuration, const _double & dTicksPerSecond)
{
	CAnim*	pInstance = new CAnim;

	if (FAILED(pInstance->NativeConstruct(pName, dDuration, dTicksPerSecond)))
	{
		MSG_BOX("Failed to Creating Instance(CAnim).");
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