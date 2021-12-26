#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CWhiteOut : public CGameObject
{
protected:
	explicit CWhiteOut(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWhiteOut(const CWhiteOut& rhs);
	virtual ~CWhiteOut() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBuffer_RectCom = nullptr;

	_float				m_fAlpha = 0.f;

private:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective();

public:
	static CWhiteOut* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END