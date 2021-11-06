#ifndef Random_Device_h__
#define Random_Device_h__

#include "Base.h"

BEGIN(Engine)

class CRandom_Device final : public CBase
{
	DECLARE_SINGLETON(CRandom_Device)
private:
	explicit CRandom_Device();
	virtual ~CRandom_Device() = default;

public:
	HRESULT Ready_RandomDevice	();
	_bool	Random_Percentage	(_int iPercentage);
	_int	Random_Select		(_int iCount);

private:
	vector<bernoulli_distribution>											m_BernoulliDistribution;
	vector<_Binder<_Unforced, bernoulli_distribution&, mt19937&>>			m_BernoulliGenerator;
	vector<uniform_int_distribution<_int>>									m_UniformDistribution;
	vector<_Binder<_Unforced, uniform_int_distribution<_int>&, mt19937&>>	m_UniformGenerator;

public:
	virtual void	Free() override;
};

END

#endif // Random_Device_h__
