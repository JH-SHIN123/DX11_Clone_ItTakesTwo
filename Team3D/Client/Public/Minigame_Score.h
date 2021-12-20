#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CMinigame_Score final : public CUIObject
 {
private:
	explicit CMinigame_Score(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMinigame_Score(const CUIObject& rhs);
	virtual ~CMinigame_Score() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	 _float2						m_vSaveScale;
	 _float							m_fAlpha = 0.f;
	 _uint							m_iOption = 0;
	 _uint							m_iShaderPassNum = 0;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

	CFontDraw*						m_pFontCodyWinCount = nullptr;
	CFontDraw*						m_pFontMayWinCount = nullptr;
	CFontDraw*						m_pFontCenter = nullptr;
	CFontDraw*						m_pFontCody = nullptr;
	CFontDraw*						m_pFontMay = nullptr;

private:
	HRESULT Ready_Component();
	void Render_Font();

public:
	static CMinigame_Score* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END