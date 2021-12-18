#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CHpBarFrame final : public CUIObject
 {
private:
	explicit CHpBarFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHpBarFrame(const CUIObject& rhs);
	virtual ~CHpBarFrame() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	_bool							m_IsActive = false;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();

public:
	static CHpBarFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END