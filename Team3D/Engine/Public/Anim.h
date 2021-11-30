#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnim final : public CBase
{
private:
	explicit CAnim() = default;
	virtual ~CAnim() = default;

public: /* Struct */
	typedef struct tagAnimDesc
	{
		_double	dDuration;
		_double	dTicksPerSecond;
		char	szAnimName[MAX_PATH];
	}ANIM_DESC;

public: /* Getter */
	const _double	Get_Duration() const { return m_AnimDesc.dDuration; }
	const _double	Get_TicksPerSecond() const { return m_AnimDesc.dTicksPerSecond; }
	void			Get_PreAnimKeyFrames(_uint iPreAnimFrame, vector<KEY_FRAME>& PreAnimKeyFrames);
	const vector<class CAnimChannel*>& Get_Channels() const { return m_Channels; }
	const _uint		Get_ChannelCount() { return (_uint)m_Channels.size(); }
	const _double	Get_ChannelKeyFrameTimeAvg(_int iIndex); /* 모든 채널의 해당 인덱스 프레임, 평균시간 */

public:
	HRESULT	NativeConstruct(ANIM_DESC AnimDesc);
	HRESULT	Bring_ChannelContainer(vector<class CAnimChannel*>& Channels);
	HRESULT	Update_Transformations(_double& dCurrentTime, _uint& iCurAnimFrame, vector<_float4x4>& Transformations);
	HRESULT Update_Transformations_Blend(_double& dCurrentTime, _uint& iCurAnimFrame, vector<_float4x4>& Transformations, vector<KEY_FRAME>& PreAnimKeyFrames, _float fLerpRatio);

public:
	HRESULT	Update_PathTransformation(_double& dCurrentTime, _uint& iCurAnimFrame, vector<_float4x4>& Transformations);
	HRESULT	Update_RewindPathTransformation(_double& dCurrentTime, _uint& iCurAnimFrame, vector<_float4x4>& Transformations);

private: /* Typedef */
	typedef vector<class CAnimChannel*>	CHANNELS;
private:
	CHANNELS	m_Channels;
	ANIM_DESC	m_AnimDesc;

public:
	static CAnim* Create(ANIM_DESC AnimDesc);
	virtual void Free() override;
};

END