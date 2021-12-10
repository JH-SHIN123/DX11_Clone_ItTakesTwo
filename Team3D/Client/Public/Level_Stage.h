#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

/**
* 레이어 추가할 때는 자기 이름 위치에 함수를 생성해주세요.
* cpp에 헤더를 추가할 때는 자기 이름 위치에 추가해주세요.
* __MAPLOADING_OFF 상태일 때는 Ready_Test() 함수만 호출됩니다.
* __MAPLOADING_OFF 상태에서 생성할 테스트 오브젝트들은 Ready_Test() 함수 내부에서 생성해주세요.
*/
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
	/* Read_File */
	HRESULT Clone_StaticGameObjects_ByFile(const _tchar* pFilePath, const _tchar* pLayerTag, const _tchar* pGameObjectTag, GameID::Enum eID, _float fCullRadius);
	/* Test */
	HRESULT Ready_Test();
private:
	/* Se */
	HRESULT Ready_Layer_GravityPath(const _tchar* pLayerTag);
	/* Jung */
	HRESULT Ready_Layer_WarpGate(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Wormhole(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WallLaserTrap(const _tchar* pLayerTag);
	HRESULT Ready_Layer_GravityPipe(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Env_Particles(const _tchar* pLayerTag);
	/* Hye */
	HRESULT Ready_Layer_Planet(const _tchar* pLayerTag);
	/* Taek */
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
	HRESULT Ready_Layer_MayJumpWall(const _tchar* pLayerTag);
	HRESULT Ready_Layer_PipeJumpWall(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Rope(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UFORadarSet(const _tchar* pLayerTag);
	/* Jin */
	HRESULT Ready_Layer_ControlRoomPuzzle(const _tchar* pLayerTag, const _tchar* pGameObjectTag);
	HRESULT Ready_Layer_ControlRoom_Glass(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Umbrella_Joystick(const _tchar * pLayerTag);
	/* Jun */
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);

public:
	static CLevel_Stage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END