#pragma once
#include "GameObject.h"
#include "ScriptBackGround.h"

BEGIN(Client)
#define SCRIPT ((CScript*)(DATABASE->Get_Script()))
class CScript final : public CGameObject
{
public:
	enum SCREEN	   { FULL, HALF, SCREEN_END };

private:
	explicit CScript(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CScript(const CScript& rhs);
	virtual ~CScript() = default;

public:
	/* 스크립트 인덱스, 화면비율, 삭제속도, 백그라운드 유무 */
	HRESULT Render_Script(_uint iScriptIndex, SCREEN eScreenMode, _float fDeadTime, _bool bBackGround = true);
	/* 두줄이상출력시 사용 *Only FUllScreen */
	HRESULT Render_Script_DoubleLine(_uint iIndexFirst, _uint iIndexSecond, _float fDeadTime, _bool bBackGround = true);

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	Late_Tick(_double dTimeDelta);
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup);

private:
	CRenderer*			m_pRendererCom = nullptr;
	CFontDraw*			m_pFontCom_First = nullptr;
	CFontDraw*			m_pFontCom_Second = nullptr;

	CScriptBackGound*	m_pBackGround_Main = nullptr;
	CScriptBackGound*	m_pBackGround_Sub = nullptr;

	SCREEN				m_eScreenMode = SCREEN_END;

	_uint				m_iIndexFirst = 0;
	_uint				m_iIndexSecond = 0;

	_float				m_fDeadTime = 0.f;
	_double				m_dCoolTime = 0.0;
	_bool				m_bRender = false;
	_bool				m_isDouble = false;
	_bool				m_bBackGround = false;
	_float2				m_fOffset = {};

private:
	_tchar* Find_Script(_uint iIndex);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CScript* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END