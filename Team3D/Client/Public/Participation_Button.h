#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CParticipation_Button : public CGameObject
{
protected:
	explicit CParticipation_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CParticipation_Button(const CParticipation_Button& rhs);
	virtual ~CParticipation_Button() = default;

public:
	_bool Get_ReadyCody() { return m_bReadyCody; }
	_bool Get_ReadyMay() { return m_bReadyMay; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

	CTextures*			m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBuffer_RectCom = nullptr;

	_uint				m_iFrame = 0;
	_uint				m_iOption = 0;

	_bool				m_bReadyCody = false;
	_bool				m_bReadyMay = false;

	_double				m_dDelayTime = 0.0;

private:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective();

public:
	static CParticipation_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END