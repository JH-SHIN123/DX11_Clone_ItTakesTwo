#include "..\public\Textures.h"

CTextures::CTextures(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CTextures::CTextures(const CTextures & rhs)
	: CComponent		(rhs)
	, m_Textures		(rhs.m_Textures)
	, m_iTextureCount	(rhs.m_iTextureCount)
{
	for (auto& pShaderResourceView : m_Textures)
		Safe_AddRef(pShaderResourceView);
}

ID3D11ShaderResourceView * CTextures::Get_ShaderResourceView(_uint iIndex)
{
	NULL_CHECK_RETURN(iIndex < m_iTextureCount, nullptr);

	return m_Textures[iIndex];
}

HRESULT CTextures::NativeConstruct_Prototype(TEXTURE_TYPE eType, const _tchar * pTextureFilePath, _uint iTextureCount)
{
	CComponent::NativeConstruct_Prototype();

	NULL_CHECK_RETURN(m_pDevice, E_FAIL);
	FAILED_CHECK_RETURN(CoInitializeEx(nullptr, COINIT_MULTITHREADED), E_FAIL);

	m_iTextureCount = iTextureCount;

	_tchar	szTextureFileName[MAX_PATH] = TEXT("");
	_tchar	szPreTextureFileName[MAX_PATH] = TEXT("");
	ID3D11ShaderResourceView*	pShaderResourceView = nullptr;

	for (_uint iIndex = 0; iIndex < m_iTextureCount; ++iIndex)
	{
		wsprintf(szTextureFileName, pTextureFilePath, iIndex);

		if (!lstrcmp(szPreTextureFileName, szTextureFileName))
		{
			m_Textures.emplace_back(pShaderResourceView);
			Safe_AddRef(pShaderResourceView);
			continue;
		}
		lstrcpy(szPreTextureFileName, szTextureFileName);

		ScratchImage	Image;
		HRESULT			hr = 0;

		switch (eType)
		{
		case Engine::CTextures::TYPE_DDS:
			hr = LoadFromDDSFile(szTextureFileName, DDS_FLAGS_NONE, nullptr, Image);
			break;
		case Engine::CTextures::TYPE_TGA:
			hr = LoadFromTGAFile(szTextureFileName, nullptr, Image);
			break;
		case Engine::CTextures::TYPE_WIC:
			hr = LoadFromWICFile(szTextureFileName, WIC_FLAGS_NONE, nullptr, Image);
			break;
		}

		FAILED_CHECK_RETURN(hr, E_FAIL);
		FAILED_CHECK_RETURN(CreateShaderResourceView(m_pDevice, Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(), &pShaderResourceView), E_FAIL);

		m_Textures.emplace_back(pShaderResourceView);
	}

	return S_OK;
}

HRESULT CTextures::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	return S_OK;
}

CTextures * CTextures::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TEXTURE_TYPE eType, const _tchar * pTextureFilePath, _uint iTextureCount)
{
	CTextures* pInstance = new CTextures(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType, pTextureFilePath, iTextureCount)))
	{
		MSG_BOX("Failed to Create Instance - CTextures");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTextures::Clone_Component(void * pArg)
{
	CTextures* pInstance = new CTextures(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTextures");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTextures::Free()
{
	for (auto& pShaderResourceView : m_Textures)
		Safe_Release(pShaderResourceView);
	m_Textures.clear();

	CComponent::Free();
}
