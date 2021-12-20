#pragma once

#include "Client_Defines.h"
#include "Light.h"
#include "VolumeObject.h"

BEGIN(Client)
class CVolumeLight final : public CLight
{
public:
	typedef struct tagVolumeLightDesc
	{
		LIGHT_DESC					tLightDesc;
		CVolumeObject::VOLUME_DESC	tVolumeDesc;
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
	CVolumeObject*			m_pVolumeObject = nullptr;

public:
	static CVolumeLight* Create(const _tchar* pLightTag, void* pArgs);
	virtual void Free() override;
};
END