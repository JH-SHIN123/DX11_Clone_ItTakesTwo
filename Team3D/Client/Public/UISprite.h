#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Sprite;
END

BEGIN(Client)

class CUISprite final : public CUIObject
 {
private:
	explicit CUISprite(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUISprite(const CUIObject& rhs);
	virtual ~CUISprite() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_double								m_FrameTime = 0;
	_double								m_FrameControl = 0;
	_uint								m_iWidthMaxCount = 0;
	_uint								m_iHeightMaxCount = 0;
	_uint								m_iWidthIndex = 0;
	_uint								m_iHeightIndex = 0;
	_uint								m_iTextureWidth = 0;
	_uint								m_iTextureHeight = 0;
	_uint								m_iShaderPassNum = 0;
	_uint								m_iStickOption = 0;

private:
	CVIBuffer_Sprite*					m_pVIBuffer_SpriteCom = nullptr;

private:
	 void Update_Frame(_double TimeDelta);
	 void SetUp_SpriteInfo();

private:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective();



public:
	static CUISprite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END