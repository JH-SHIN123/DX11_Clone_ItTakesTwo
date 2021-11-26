#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CChapterSelect final : public CUIObject
 {
private:
	explicit CChapterSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CChapterSelect(const CUIObject& rhs);
	virtual ~CChapterSelect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;
	CTextures*						m_pSubTextureCom = nullptr;

private:
	HRESULT Ready_Component();
	void SetUp_Option();

public:
	static CChapterSelect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END