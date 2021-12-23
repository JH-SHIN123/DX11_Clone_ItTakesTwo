#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Loading.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT	NativeConstruct(Level::ID ePreLevelID, Level::ID eNextLevelID);
	virtual _int	Tick(_double dTimedelta) override;
	virtual HRESULT	Render() override;

private:
	CLoading*	m_pLoading		= nullptr;
	Level::ID	m_ePreLevelID	= Level::LEVEL_END;
	Level::ID	m_eNextLevelID	= Level::LEVEL_END;

	CFontDraw*	m_pFont = nullptr;

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Level::ID ePreLevelID, Level::ID eNextLevelID);
	virtual void Free() override;
};

END