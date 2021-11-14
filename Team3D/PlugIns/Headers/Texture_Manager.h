#pragma once

#include "Textures.h"

class CTexture_Manager final : public CBase
{
	DECLARE_SINGLETON(CTexture_Manager)
private:
	explicit CTexture_Manager() = default;
	virtual ~CTexture_Manager() = default;

public:
	CTextures*	Find_SameTextures(const _tchar* pTextureFilePath);
	void		Add_Textures(const _tchar* pTextureFilePath, CTextures* pTextures);

private:
	unordered_map<const _tchar*, CTextures*> m_Textures;

public:
	virtual void Free() override;
};