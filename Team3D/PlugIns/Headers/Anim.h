#ifndef Anim_h__
#define Anim_h__

#include "Base.h"

BEGIN(Engine)

class CAnim final : public CBase
{
private:
	explicit CAnim();
	virtual ~CAnim() = default;

public: /* Getter */
	const _double&	Get_Duration		() const;
	const _double&	Get_TicksPerSecond	() const;

public:
	HRESULT		NativeConstruct			(const char* pName, const _double& dDuration, const _double& dTicksPerSecond);
	HRESULT		Bring_ChannelContainer	(vector<class CAnimChannel*>& Channels);
	HRESULT		Update_ChannelMatrices	(_double& dCurrentTime, vector<_uint>& CurrentKeyFrames, vector<_float4x4>& Transformations, vector<_uint>& LastKeyFrames, CAnim* pPreAnim = nullptr, _double dPreStopTime = 0.0, _double dPreAnimDuration = 0.0,_double dPreAnimTime = 0.0);

private:
	typedef vector<class CAnimChannel*>	CHANNELS;
private:
	CHANNELS	m_Channels;
	char		m_szAnimName[MAX_PATH]	= "";
	_double		m_dDuration				= 0.0;
	_double		m_dTicksPerSecond		= 0.0;

public:
	static CAnim*	Create	(const char* pName, const _double& dDuration, const _double& dTicksPerSecond);
	virtual void	Free	() override;
};

END

#endif // Anim_h__
