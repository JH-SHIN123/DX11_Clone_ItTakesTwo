#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPC_MouseButton final : public CUIObject
 {
private:
	explicit CPC_MouseButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPC_MouseButton(const CUIObject& rhs);
	virtual ~CPC_MouseButton() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective();
	HRESULT Set_UIVariables_Perspective1();

public:
	static CPC_MouseButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END