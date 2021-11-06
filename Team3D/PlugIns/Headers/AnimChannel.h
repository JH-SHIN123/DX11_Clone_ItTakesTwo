#ifndef AnimChannel_h__
#define AnimChannel_h__

#include "Base.h"

BEGIN(Engine)

class CAnimChannel final : public CBase
{
private:
	explicit CAnimChannel();
	virtual ~CAnimChannel() = default;

public: /* Getter */
	const char*					Get_Name		() const;
	const vector<KEY_FRAME*>&	Get_KeyFrames	() const;

public:
	HRESULT	NativeConstruct			(const char* pName);
	HRESULT	Bring_KeyFrameContainer	(vector<KEY_FRAME*>& KeyFrames);

private:
	typedef vector<KEY_FRAME*>	KEYFRAMES;
private:
	KEYFRAMES	m_KeyFrames;
	char		m_szChannelName[MAX_PATH]	= "";

public:
	static CAnimChannel*	Create	(const char* pName);
	virtual void			Free	() override;
};

END

#endif // AnimChannel_h__
