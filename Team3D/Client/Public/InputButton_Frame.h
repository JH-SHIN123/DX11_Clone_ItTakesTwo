#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CInputButton_Frame final : public CUIObject
{
private:
	explicit CInputButton_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInputButton_Frame(const CUIObject& rhs);
	virtual ~CInputButton_Frame() = default;

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
	HRESULT Set_ConstantTable();

public:
	static CInputButton_Frame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END