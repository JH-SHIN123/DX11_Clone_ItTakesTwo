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
	, CHANNEL_LOGO
	, CHANNEL_BUTTON
	, CHANNEL_TUBE
	, CHANNEL_HANGINGPLANET
	, CHANNEL_LASERTENNIS
	, CHANNEL_LASERBUTTON
	, CHANNEL_LASERBUTTONLARGE
	, CHANNEL_LASERACTIVATION
	, CHANNEL_LASERPOWERCOORD
	, CHANNEL_PINBALL
	, CHANNEL_PINBALL_HANDLE
	, CHANNEL_PINBALL_BLOCKER
	, CHANNEL_PINBALL_DOOR
	, CHANNEL_PINBALL_HANDLEMOVE
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

	/* 사운드 이펙트 */

	/* 사운드 보간 */
	/* 사운드를 재생시킨 후 호출해야합니다.*/
	/* 첫번째 채널, 두번째 채널, 보간 스피드, 첫번째 사운드볼륨, 두번째 사운드볼륨 */
	/* 보간이 끝나면 첫번째 채널의 사운드는 자동 종료 됩니다. */
	void Lerp_Sound(CHANNEL_TYPE eFirstChannel, CHANNEL_TYPE eSecondChannel, _float fLerpSpeed, _float fFirstVolume, _float fSecondVolume);
	/* 사운드 페이드인아웃*/
	/* 채널, Type(true == FadeIn, False == FadeOut) */
	void FadeInOut(CHANNEL_TYPE eFirstChannel, _bool bType, _float fLerpSpeed, _float fVolume);

public:
	HRESULT Ready_SoundManager();
	void Update_Sound(_double dTimeDelta);

private:
	typedef unordered_map<TCHAR*, FMOD_SOUND*> SOUNDS;
private:
	FMOD_SYSTEM*	m_pSystem = nullptr;
	FMOD_CHANNEL*	m_pChannel[CHANNEL_END];
	SOUNDS			m_Sounds;

	/* 사운드 이펙트 */
	CHANNEL_TYPE	m_eFirstChannel = CHANNEL_END;
	CHANNEL_TYPE	m_eSecondChannel = CHANNEL_END;
	_float			m_fFirstVolume = 0.f;
	_float			m_fSecondVolume = 0.f;
	_float			m_fMaxVolume = 0.f;
	_float			m_fLerpSpeed = 0.f;
	_bool			m_bLerp = false;

	_bool			m_bFadeInOut = false;
	_bool			m_bType = false;

private:
	HRESULT Add_Sound(char* pFilePath);

private:
	/* Update */
	void Lerp_Sound_Update(_double dTimeDelta);
	void FadeInOut_Sound_Update(_double dTimeDelta);

public:
	virtual void Free() override;
};

END