#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTextures final : public CComponent
{
private:
	explicit CTextures(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTextures(const CTextures& rhs);
	virtual ~CTextures() = default;

public: /* Enum */
	enum TEXTURE_TYPE { TYPE_DDS, TYPE_TGA, TYPE_WIC, TYPE_END };

public: /* Getter */
	ID3D11ShaderResourceView* Get_ShaderResourceView(_uint iIndex);

public:
	virtual HRESULT	NativeConstruct_Prototype(TEXTURE_TYPE eType, const _tchar* pTextureFilePath, _uint iTextureCount);
	virtual HRESULT	NativeConstruct(void* pArg) override;

private: /* Typedef */
	typedef vector<ID3D11ShaderResourceView*> TEXTURES;
private:
	TEXTURES	m_Textures;
	_uint		m_iTextureCount = 0;

public:
	static CTextures* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TEXTURE_TYPE eType, const _tchar* pTextureFilePath, _uint iTextureCount = 1);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END