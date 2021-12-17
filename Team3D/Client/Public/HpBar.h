#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CHpBar final : public CUIObject
 {
private:
	explicit CHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHpBar(const CUIObject& rhs);
	virtual ~CHpBar() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_Active(_bool IsCheck);

private:
	_bool							m_IsActive = false;
	_float							m_fHp = 0.f;

private:
	class CHpBarFrame*				m_pHpBarFrame = nullptr;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;
	CTextures*						m_pSubTexturesCom = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Ready_Layer_UI();

public:
	static CHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END