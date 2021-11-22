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

HRESULT CTextures::NativeConstruct_Prototype(TEXTURE_TYPE eType, const _tchar * pTextureFilePath, _uint iTextureCount, _uint iTextureType)
{
	CComponent::NativeConstruct_Prototype();

	NULL_CHECK_RETURN(m_pDevice, E_FAIL);
	FAILED_CHECK_RETURN(CoInitializeEx(nullptr, COINIT_MULTITHREADED), E_FAIL);

	m_iTextureCount = iTextureCount;

	_tchar	szTextureFileName[MAX_PATH] = TEXT("");
	_tchar	szProcessedTextureFileName[MAX_PATH] = TEXT("");
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

		/* Find_ProcessedImage */
		lstrcpy(szProcessedTextureFileName, szTextureFileName);
		lstrcat(szProcessedTextureFileName, TEXT("p"));

		HANDLE hFile = CreateFile(szProcessedTextureFileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile) /* No_ProcessedImage, Make_NewImage */
		{
			ScratchImage SrcImage;

			switch (eType)
			{
			case Engine::CTextures::TYPE_DDS:
				FAILED_CHECK_RETURN(LoadFromDDSFile(szTextureFileName, DDS_FLAGS_NONE, nullptr, SrcImage), E_FAIL);
				break;
			case Engine::CTextures::TYPE_TGA:
				FAILED_CHECK_RETURN(LoadFromTGAFile(szTextureFileName, nullptr, SrcImage), E_FAIL);
				break;
			case Engine::CTextures::TYPE_WIC:
				FAILED_CHECK_RETURN(LoadFromWICFile(szTextureFileName, WIC_FLAGS_NONE, nullptr, SrcImage), E_FAIL);
				break;
			}

			if (iTextureType == aiTextureType_DIFFUSE)
			{
				ScratchImage DstImage;
				FAILED_CHECK_RETURN(Resize(SrcImage.GetImages(), SrcImage.GetImageCount(), SrcImage.GetMetadata(), SrcImage.GetMetadata().width / 4, SrcImage.GetMetadata().height / 4, TEX_FILTER_DEFAULT, DstImage), E_FAIL);

				Save_ProcessedImage(&DstImage, szProcessedTextureFileName);
				FAILED_CHECK_RETURN(CreateShaderResourceView(m_pDevice, DstImage.GetImages(), DstImage.GetImageCount(), DstImage.GetMetadata(), &pShaderResourceView), E_FAIL);
			}
			else
			{
				Save_ProcessedImage(&SrcImage, szProcessedTextureFileName);
				FAILED_CHECK_RETURN(CreateShaderResourceView(m_pDevice, SrcImage.GetImages(), SrcImage.GetImageCount(), SrcImage.GetMetadata(), &pShaderResourceView), E_FAIL);
			}

			m_Textures.emplace_back(pShaderResourceView);
		}
		else /* Load_ProcessedImage */
		{
			Image		ProcessedImage;
			_ulong		dwByte = 0;
			size_t		iSize;
			TexMetadata	Metadata;

			ReadFile(hFile, &iSize, sizeof(size_t), &dwByte, nullptr);
			ReadFile(hFile, &Metadata, sizeof(TexMetadata), &dwByte, nullptr);
			ReadFile(hFile, &ProcessedImage, sizeof(Image), &dwByte, nullptr);
			ProcessedImage.pixels = new uint8_t[iSize];
			ReadFile(hFile, ProcessedImage.pixels, sizeof(uint8_t) * (_uint)iSize, &dwByte, nullptr);

			FAILED_CHECK_RETURN(CreateShaderResourceView(m_pDevice, &ProcessedImage, 1, Metadata, &pShaderResourceView), E_FAIL);
			m_Textures.emplace_back(pShaderResourceView);
			Safe_Delete_Array(ProcessedImage.pixels);
		}

		CloseHandle(hFile);
	}

	return S_OK;
}

HRESULT CTextures::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CTextures::Save_ProcessedImage(ScratchImage * pSrcImage, const _tchar * pTextureFilePath)
{
	HANDLE hFile = CreateFile(pTextureFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile) return E_FAIL;

	Image		BaseImage = *pSrcImage->GetImage(0, 0, 0);
	_ulong		dwByte = 0;
	size_t		iSize = pSrcImage->GetPixelsSize();
	TexMetadata Metadata = pSrcImage->GetMetadata();

	WriteFile(hFile, &iSize, sizeof(size_t), &dwByte, nullptr);
	WriteFile(hFile, &Metadata, sizeof(TexMetadata), &dwByte, nullptr);
	WriteFile(hFile, &BaseImage, sizeof(Image), &dwByte, nullptr);
	WriteFile(hFile, BaseImage.pixels, sizeof(uint8_t) * (_uint)iSize, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

CTextures * CTextures::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TEXTURE_TYPE eType, const _tchar * pTextureFilePath, _uint iTextureCount, _uint iTextureType)
{
	CTextures* pInstance = new CTextures(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType, pTextureFilePath, iTextureCount, iTextureType)))
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
