#include "..\public\Sound_Manager.h"
#include <conio.h>
#include <io.h>

IMPLEMENT_SINGLETON(CSound_Manager)

_bool CSound_Manager::Is_Playing(CHANNEL_TYPE eChannel)
{
	FMOD_BOOL IsPlay;
	FMOD_Channel_IsPlaying(m_pChannel[eChannel], &IsPlay);

	if (IsPlay == 0)
		return false;
	else
		return true;
}

void CSound_Manager::Play_Sound(TCHAR * pSoundKey, CHANNEL_TYPE eChannel, _float fVolume, _bool bLoop)
{
	unordered_map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_Sounds.begin(), m_Sounds.end(), CTagFinder(pSoundKey));

	if (iter == m_Sounds.end())
		return;

	if (bLoop) 
		FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, 0, &(m_pChannel[eChannel]));
	FMOD_System_Update(m_pSystem);

	Set_SoundVolume(eChannel, fVolume);
}

void CSound_Manager::Stop_Sound(CHANNEL_TYPE eChannel)
{
	FMOD_Channel_Stop(m_pChannel[eChannel]);
}

void CSound_Manager::Stop_SoundAll()
{
	for (int i = 0; i < CHANNEL_END; ++i)
		FMOD_Channel_Stop(m_pChannel[i]);
}

void CSound_Manager::Set_SoundVolume(CHANNEL_TYPE eChannel, _float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume);
}

HRESULT CSound_Manager::Ready_SoundManager()
{
	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, CHANNEL_END, FMOD_INIT_NORMAL, NULL);

	FAILED_CHECK_RETURN(Add_Sound("../Bin/Resources/Sound/"), E_FAIL);

	FMOD_System_Update(m_pSystem);

	return S_OK;
}

void CSound_Manager::Update_Sound(_double dTimeDelta)
{
	Lerp_Sound_Update(dTimeDelta);
	FadeInOut_Sound_Update(dTimeDelta);
}

void CSound_Manager::Lerp_Sound(CHANNEL_TYPE eFirstChannel, CHANNEL_TYPE eSecondChannel, _float fLerpSpeed, _float fFirstVolume, _float fSecondVolume)
{
	m_eFirstChannel = eFirstChannel;
	m_eSecondChannel = eSecondChannel;

	m_fFirstVolume = fFirstVolume;
	m_fSecondVolume = 0.f;
	m_fMaxVolume = fSecondVolume;

	m_fLerpSpeed = fLerpSpeed;

	m_bLerp = true;
}

void CSound_Manager::FadeInOut(CHANNEL_TYPE eFirstChannel, _bool bType, _float fLerpSpeed, _float fVolume)
{
	if (true == m_bFadeInOut)
		return;

	m_eFirstChannel = eFirstChannel;
	m_fFirstVolume = fVolume;
	m_fLerpSpeed = fLerpSpeed;

	/* FadeIn */
	if (true == m_bType)
		m_fFirstVolume = 0.f;
	/* FadeOut */
	else
		m_fMaxVolume = fVolume;

	m_bFadeInOut = true;
	m_bType = bType;
}

HRESULT CSound_Manager::Add_Sound(char * pFilePath)
{
	_finddata_t	fd = {};
	long long handle = 0;
	_int iResult = 0;

	char szFilePath[256];
	strcpy_s(szFilePath, pFilePath);
	strcat_s(szFilePath, "*.*");

	handle = _findfirst(szFilePath, &fd);

	if (-1 == handle)
		return E_FAIL;

	while (-1 != iResult)
	{
		char szFullPath[256] = "";
		strcpy_s(szFullPath, 256, pFilePath);
		strcat_s(szFullPath, 256, fd.name);
		
		if ((0 != strcmp(fd.name, ".")) && (0 != strcmp(fd.name, "..")))
		{
			if (fd.attrib & _A_SUBDIR)
			{
				strcat_s(szFullPath, "/");
				Add_Sound(szFullPath);
			}
			else
			{
				FMOD_SOUND*	pSound = nullptr;
				FMOD_RESULT FResult = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, NULL, &pSound);

				if (FMOD_OK == FResult)
				{
					TCHAR*	pSoundKey = new TCHAR[256];
					ZeroMemory(pSoundKey, sizeof(TCHAR) * 256);
					MultiByteToWideChar(CP_ACP, 0, fd.name, (_int)strlen(fd.name) + 1, pSoundKey, 256);

					m_Sounds.emplace(pSoundKey, pSound);
				}
			}
		}

		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);

	return S_OK;
}

void CSound_Manager::Lerp_Sound_Update(_double dTimeDelta)
{
	if (false == m_bLerp)
		return;

	if (0.f >= m_fFirstVolume)
	{
		m_fFirstVolume = 0.f;
		m_fSecondVolume += (_float)dTimeDelta * m_fLerpSpeed;
	}
	else
		m_fFirstVolume -= (_float)dTimeDelta * m_fLerpSpeed;

	if (m_fSecondVolume >= m_fMaxVolume)
	{
		Stop_Sound(m_eFirstChannel);
		m_fSecondVolume = m_fMaxVolume;
		m_bLerp = false;
		return;
	}

	Set_SoundVolume(m_eFirstChannel, m_fFirstVolume);
	Set_SoundVolume(m_eSecondChannel, m_fSecondVolume);
}

void CSound_Manager::FadeInOut_Sound_Update(_double dTimeDelta)
{
	if (false == m_bFadeInOut)
		return;

	/* FadeOut */
	if (false == m_bType)
	{
		m_fFirstVolume -= (_float)dTimeDelta * m_fLerpSpeed;

		if (0 >= m_fFirstVolume)
		{
			m_fFirstVolume = 0.f;
			m_bFadeInOut = false;
			return;
		}
	}
	/* FadeIn */
	else
	{
		m_fFirstVolume += (_float)dTimeDelta * m_fLerpSpeed;

		if (m_fMaxVolume <= m_fFirstVolume)
		{
			m_fFirstVolume = m_fMaxVolume;
			m_bFadeInOut = false;
			return;
		}
	}

	Set_SoundVolume(m_eFirstChannel, m_fFirstVolume);
}

void CSound_Manager::Free()
{
	for (auto& Pair : m_Sounds)
	{
		delete[] Pair.first;
		FMOD_Sound_Release(Pair.second);
	}
	m_Sounds.clear();

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}
