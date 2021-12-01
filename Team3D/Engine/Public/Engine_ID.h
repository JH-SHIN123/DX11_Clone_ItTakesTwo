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
			eSAVEPOINT,
			eDEADLINE,
			eROCKET,
			eROBOT,
			eROBOTHEAD,
			eROBOTBATTERY,
			eROBOTLEVER,
			eVALVE,
			eUMBRELLA,
			eVERTICALDOOR,
			eBIGBUTTON,
			eBIGBUTTONFRAME,
			eROPEBASE,
			eGRINDRAIL,
			ePLANET,
			eNOBATTERYSIGN,
			eSPACEVALVE,
			eGRAVITYPIPE,
			eHOOKUFO,
			eGRAVITYPATH,
			eENVIRONMENT,
			ePINBALL,
			eBLOCKED,
			ePINBALLHANDLE,
			eGRAVITYPATH_CENTER,
			eGRAVITYPATH_SIDE,
			ePINBALLDOOR,
			eWARPGATE,
			eWORMHOLE,
			eFIREDOOR,
			eBOSSMISSILE_COMBAT,
			eBOSSMISSILE_PLAYABLE,
			eDUMMYWALL,
			eHANGINGPLANET
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

	struct ContactStatus
	{
		enum Enum { eFOUND, eLOST };
	};
}