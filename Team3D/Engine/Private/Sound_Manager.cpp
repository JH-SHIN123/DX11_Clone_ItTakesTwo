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

void CSound_Manager::Play_Sound(TCHAR * pSoundKey, CHANNEL_TYPE eChannel, _float fVolume)
{
	unordered_map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_Sounds.begin(), m_Sounds.end(), CTagFinder(pSoundKey));

	if (iter == m_Sounds.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, 0, &(m_pChannel[eChannel]));
	FMOD_System_Update(m_pSystem);

	Set_SoundVolume(eChannel, fVolume);
}

void CSound_Manager::Play_BGM(TCHAR * pSoundKey, CHANNEL_TYPE eChannel)
{
	unordered_map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_Sounds.begin(), m_Sounds.end(), CTagFinder(pSoundKey));

	if (iter == m_Sounds.end())
		return;

	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);
	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, 0, &(m_pChannel[eChannel]));
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

	_finddata_t	fd = {};
	long long handle = 0;
	int iResult = 0;

	handle = _findfirst("../Bin/Resources/Sound/*.*", &fd);

	if (-1 == handle)
	{
		MSG_BOX("Can't Read SoundFile");
		return E_FAIL;
	}

	while (-1 != iResult)
	{
		char szFullPath[256] = "";
		strcpy_s(szFullPath, 256, "../Bin/Resources/Sound/");
		strcat_s(szFullPath, 256, fd.name);

		FMOD_SOUND*	pSound = nullptr;

		FMOD_RESULT FResult = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, NULL, &pSound);

		if (FMOD_OK == FResult)
		{
			TCHAR*	pSoundKey = new TCHAR[256];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * 256);
			MultiByteToWideChar(CP_ACP, 0, fd.name, (_int)strlen(fd.name) + 1, pSoundKey, 256);

			m_Sounds.emplace(pSoundKey, pSound);
		}

		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);

	FMOD_System_Update(m_pSystem);

	return S_OK;
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
