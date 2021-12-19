#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CMinigame_Ready final : public CUIObject
 {
private:
	explicit CMinigame_Ready(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMinigame_Ready(const CUIObject& rhs);
	virtual ~CMinigame_Ready() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	 _float2						m_vSaveScale;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();

public:
	static CMinigame_Ready* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END