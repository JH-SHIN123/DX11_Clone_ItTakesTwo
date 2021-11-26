#pragma once

namespace Engine
{
	struct GameID
	{
		enum Enum
		{
			eCODY,
			eMAY,
			eCAMERA,
			eMAINCAMERA,
			eSUBCAMERA,
			eUFO,
			eMOONBABOON,
			eSTARBUDDY,
			eROCKET,
			eROBOT,
			eROBOTHEAD,
			eROBOTBATTERY,
			eROBOTLEVER,
			eVALVE,
			eUMBRELLA,
			eVERTICALDOOR,
			eBIGBUTTON,
			eROPEBASE,
			eGRINDRAIL,
			ePLANET,
			eNOBATTERYSIGN,
			eSPACEVALVE,
			eGRAVITYPIPE,
			eHOOKUFO,
			eENVIRONMENT
		};
	};

	typedef struct tagUserData
	{
		GameID::Enum		eID;
		class CGameObject*	pGameObject;

		tagUserData() {}
		tagUserData(GameID::Enum _eID, class CGameObject* _pGameObject) : eID(_eID), pGameObject(_pGameObject) {}
	}USERDATA;

	struct TriggerStatus
	{
		enum Enum { eFOUND, eLOST };
	};
}