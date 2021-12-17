#pragma once

#include "Base.h"
#include "./Fmod/fmod.h"

BEGIN(Engine)

enum ENGINE_DLL CHANNEL_TYPE
{
	/* Se */
	CHANNEL_BGM
	/* Yoon */
	, CHANNEL_PLAYER
	/* Jin */
	, CHANNEL_BOSS
	/* Hye */
	/* Jung */
	/* Taek */
	/* Jun */
	, CHANNEL_CUTSCENE
	, CHANNEL_END
};

class ENGINE_DLL CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)
public:
	explicit CSound_Manager() = default;
	virtual ~CSound_Manager() = default;

public: /* Getter */
	_bool Is_Playing(CHANNEL_TYPE eChannel);

public: /* Setter */
	void Play_Sound(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, _float fVolume, _bool bLoop);
	void Stop_Sound(CHANNEL_TYPE eChannel);
	void Stop_SoundAll();
	void Set_SoundVolume(CHANNEL_TYPE eChannel, _float fVolume);

public:
	HRESULT Ready_SoundManager();

private:
	typedef unordered_map<TCHAR*, FMOD_SOUND*> SOUNDS;
private:
	FMOD_SYSTEM*	m_pSystem = nullptr;
	FMOD_CHANNEL*	m_pChannel[CHANNEL_END];
	SOUNDS			m_Sounds;
private:
	HRESULT Add_Sound(char* pFilePath);

public:
	virtual void Free() override;
};

END