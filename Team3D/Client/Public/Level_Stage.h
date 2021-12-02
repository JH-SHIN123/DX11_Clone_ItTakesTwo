#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Stage final : public CLevel
{
private:
	explicit CLevel_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Stage() = default;

public:
	virtual HRESULT	NativeConstruct() override;
	virtual _int	Tick(_double dTimedelta) override;
	virtual HRESULT	Render() override;

private:
	/* Se */
	HRESULT Ready_Layer_GravityPath(const _tchar* pLayerTag);
	/* Jung */
	HRESULT Test_Layer_Object_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WarpGate(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Wormhole(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WallLaserTrap(const _tchar* pLayerTag);
	HRESULT TestLayer(const _tchar* pLayerTag);
	/* Hye */
	HRESULT Ready_Layer_PinBall(const _tchar* pLayerTag);
	/* Won */
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Sky(const _tchar* pLayerTag);
	HRESULT Ready_Layer_ToyBoxButton(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MoonBaboonCore(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MoonBaboon_MainLaser(const _tchar* pLayerTag);
	/* Yoon */
	HRESULT Ready_Layer_Cody(const _tchar* pLayerTag);
	HRESULT Ready_Layer_May(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UFO(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MoonBaboon(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Rocket(const _tchar* pLayerTag);
	HRESULT Ready_Layer_StarBuddy(const _tchar* pLayerTag);
	HRESULT Ready_Layer_NoBatterySign(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SecurityCameraHandle(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SecurityCamera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_TutorialDoor(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BigButton(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BigButtonFrame(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SpaceValve(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Hook_UFO(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BigPlanet(const _tchar* pLayerTag);
	HRESULT Ready_Layer_RobotParts(const _tchar* pLayerTag);
	HRESULT Ready_Layer_RotatedRobotParts(const _tchar* pLayerTag);
	HRESULT Ready_Layer_DummyWall(const _tchar* pLayerTag);


	/* Jin */
	/* Jun */
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	/* Read_File */
	HRESULT Clone_StaticGameObjects_ByFile(const _tchar* pFilePath, const _tchar* pLayerTag, const _tchar* pGameObjectTag, GameID::Enum eID, _float fCullRadius);

public:
	static CLevel_Stage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END