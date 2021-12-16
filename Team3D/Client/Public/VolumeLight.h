#pragma once

#include "Client_Defines.h"
#include "Light.h"
#include "StaticVolume.h"
#include "DynamicVolume.h"

BEGIN(Client)
class CVolumeLight final : public CLight
{
public:
	enum TYPE { TYPE_STATIC, TYPE_DYNAMIC, TYPE_END };

	typedef struct tagVolumeLightDesc
	{
		TYPE						eType = TYPE_END;
		LIGHT_DESC					tLightDesc;
		CStaticVolume::VOLUME_DESC	tVolumeDesc_Static;
		CDynamicVolume::VOLUME_DESC tVolumeDesc_Dynamic;
	}VOLUMELIGHT_DESC;

public: /* Setter */
	// Set_WorldMatrix

private:
	explicit CVolumeLight() = default;
	virtual ~CVolumeLight() = default;

public:
	virtual HRESULT	NativeConstruct(const _tchar * pLightTag, void* pArgs) override;
	virtual _int	Tick_Light(_double dTimeDelta) override;

private:
	VOLUMELIGHT_DESC		m_tVolumeDesc;

	CStaticVolume*			m_pStaticVolume = nullptr;
	CDynamicVolume*			m_pDynamicVolume = nullptr;

public:
	static CVolumeLight* Create(const _tchar* pLightTag, void* pArgs);
	virtual void Free() override;
};
END