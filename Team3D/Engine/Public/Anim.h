#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnim final : public CBase
{
private:
	explicit CAnim() = default;
	virtual ~CAnim() = default;

public: /* Getter */
	const _double Get_Duration() const { return m_dDuration; }
	const _double Get_TicksPerSecond() const { return m_dTicksPerSecond; }
	void Get_PreAnimKeyFrames(_uint iPreAnimFrame, vector<KEY_FRAME*>& PreAnimKeyFrames);

public:
	HRESULT	NativeConstruct(const char* pName, const _double& dDuration, const _double& dTicksPerSecond);
	HRESULT	Bring_ChannelContainer(vector<class CAnimChannel*>& Channels);
	HRESULT	Update_Transformations(_double& dCurrentTime, _uint& iCurAnimFrame, vector<_float4x4>& Transformations);
	HRESULT Update_Transformations_Blend(_double& dCurrentTime, _uint& iCurAnimFrame, vector<_float4x4>& Transformations, vector<KEY_FRAME*>& PreAnimKeyFrames, _float fLerpRatio);

private: /* Typedef */
	typedef vector<class CAnimChannel*>	CHANNELS;
private:
	CHANNELS	m_Channels;
	char		m_szAnimName[MAX_PATH] = "";
	_double		m_dDuration = 0.0;
	_double		m_dTicksPerSecond = 0.0;

public:
	static CAnim* Create(const char* pName, const _double& dDuration, const _double& dTicksPerSecond);
	virtual void Free() override;
};

END