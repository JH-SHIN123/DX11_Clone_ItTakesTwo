#include "..\public\Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

_bool CInput_Device::Key_Up(_ubyte byKeyID)
{
	if (m_byKeyState[byKeyID])
	{
		m_byKeyState_Up[byKeyID] = m_byKeyState[byKeyID];
		m_byKeyState_Up_Buffer[byKeyID] = m_byKeyState[byKeyID];
		return false;
	}
	else if (m_byKeyState_Up[byKeyID])
	{
		m_byKeyState_Up_Buffer[byKeyID] = m_byKeyState[byKeyID];
		return true;
	}
	return false;
}

_bool CInput_Device::Key_Down(_ubyte byKeyID)
{
	if (m_byKeyState[byKeyID] && !(m_byKeyState_Down[byKeyID]))
	{
		m_byKeyState_Down_Buffer[byKeyID] |= m_byKeyState[byKeyID];
		return true;
	}
	else if (!(m_byKeyState[byKeyID]) && (m_byKeyState_Down[byKeyID]))
	{
		m_byKeyState_Down[byKeyID] = 0;
		m_byKeyState_Down_Buffer[byKeyID] = 0;
		return false;
	}
	return false;
}

_bool CInput_Device::Key_Pressing(_ubyte byKeyID)
{
	if (m_byKeyState[byKeyID])
		return true;
	return false;
}

_bool CInput_Device::Mouse_Up(MOUSE_KEYSTATE eMouseKeyState)
{
	if (m_MouseState.rgbButtons[eMouseKeyState])
	{
		m_MouseState_Up.rgbButtons[eMouseKeyState] = m_MouseState.rgbButtons[eMouseKeyState];
		m_MouseState_Up_Buffer.rgbButtons[eMouseKeyState] = m_MouseState.rgbButtons[eMouseKeyState];
		return false;
	}
	else if (m_MouseState_Up.rgbButtons[eMouseKeyState])
	{
		m_MouseState_Up_Buffer.rgbButtons[eMouseKeyState] = m_MouseState.rgbButtons[eMouseKeyState];
		return true;
	}
	return false;
}

_bool CInput_Device::Mouse_Down(MOUSE_KEYSTATE eMouseKeyState)
{
	if (m_MouseState.rgbButtons[eMouseKeyState] && !(m_MouseState_Down.rgbButtons[eMouseKeyState]))
	{
		m_MouseState_Down_Buffer.rgbButtons[eMouseKeyState] |= m_MouseState.rgbButtons[eMouseKeyState];
		return true;
	}
	else if (!(m_MouseState.rgbButtons[eMouseKeyState]) && (m_MouseState_Down.rgbButtons[eMouseKeyState]))
	{
		m_MouseState_Down.rgbButtons[eMouseKeyState] = m_MouseState.rgbButtons[eMouseKeyState];
		m_MouseState_Down_Buffer.rgbButtons[eMouseKeyState] = m_MouseState.rgbButtons[eMouseKeyState];
		return false;
	}
	return false;
}

_bool CInput_Device::Mouse_Pressing(MOUSE_KEYSTATE eMouseKeyState)
{
	if (m_MouseState.rgbButtons[eMouseKeyState])
		return true;
	return false;
}

_long CInput_Device::Mouse_Move(MOUSE_MOVESTATE eMouseMoveState)
{
	return *(((_long*)&m_MouseState) + eMouseMoveState);
}

BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE * Instance, void * pArg)
{
	struct JS_CALLBACK
	{
		LPDIRECTINPUT8			pDInput;
		LPDIRECTINPUTDEVICE8*	ppJoyStick;
	};

	JS_CALLBACK Js = *(JS_CALLBACK*)pArg;

	if (FAILED(Js.pDInput->CreateDevice(Instance->guidInstance, Js.ppJoyStick, nullptr)))
		return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

HRESULT CInput_Device::Ready_InputDevice(HINSTANCE hInst, HWND hWnd)
{
	FAILED_CHECK_RETURN(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, nullptr), E_FAIL);

	ZeroMemory(m_byKeyState, sizeof(_byte) * 256);
	ZeroMemory(m_byKeyState_Up, sizeof(_byte) * 256);
	ZeroMemory(m_byKeyState_Down, sizeof(_byte) * 256);
	ZeroMemory(m_byKeyState_Up_Buffer, sizeof(_byte) * 256);
	ZeroMemory(m_byKeyState_Down_Buffer, sizeof(_byte) * 256);
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
	ZeroMemory(&m_MouseState_Up, sizeof(DIMOUSESTATE));
	ZeroMemory(&m_MouseState_Down, sizeof(DIMOUSESTATE));
	ZeroMemory(&m_MouseState_Up_Buffer, sizeof(DIMOUSESTATE));
	ZeroMemory(&m_MouseState_Down_Buffer, sizeof(DIMOUSESTATE));
	ZeroMemory(&m_JoyStickState, sizeof(DIJOYSTATE));

	FAILED_CHECK_RETURN(m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr), E_FAIL);
	FAILED_CHECK_RETURN(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard), E_FAIL);
	FAILED_CHECK_RETURN(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE), E_FAIL);
	FAILED_CHECK_RETURN(m_pKeyboard->Acquire(), E_FAIL);

	FAILED_CHECK_RETURN(m_pDInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);
	FAILED_CHECK_RETURN(m_pMouse->SetDataFormat(&c_dfDIMouse), E_FAIL);
	FAILED_CHECK_RETURN(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE), E_FAIL);
	FAILED_CHECK_RETURN(m_pMouse->Acquire(), E_FAIL);

	/* JoyStick */
	//JS_CALLBACK Js;
	//Js.pDInput = m_pDInput;
	//Js.ppJoyStick = &m_pJoyStick;

	//FAILED_CHECK_RETURN(m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumCallback, &Js, DIEDFL_ATTACHEDONLY), E_FAIL);
	//FAILED_CHECK_RETURN(m_pJoyStick->SetDataFormat(&c_dfDIJoystick), E_FAIL);
	//FAILED_CHECK_RETURN(m_pJoyStick->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE), E_FAIL);
	//FAILED_CHECK_RETURN(m_pJoyStick->Acquire(), E_FAIL);

	return S_OK;
}

_int CInput_Device::Tick()
{
	FAILED_CHECK_RETURN(m_pKeyboard->GetDeviceState(256, m_byKeyState), EVENT_ERROR);
	FAILED_CHECK_RETURN(m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState), EVENT_ERROR);
	//FAILED_CHECK_RETURN(m_pJoyStick->GetDeviceState(sizeof(DIJOYSTATE), &m_JoyStickState), EVENT_ERROR);

	memcpy(m_byKeyState_Up, m_byKeyState_Up_Buffer, sizeof(_byte) * 256);
	memcpy(m_byKeyState_Down, m_byKeyState_Down_Buffer, sizeof(_byte) * 256);
	memcpy(&m_MouseState_Up, &m_MouseState_Up_Buffer, sizeof(DIMOUSESTATE));
	memcpy(&m_MouseState_Down, &m_MouseState_Down_Buffer, sizeof(DIMOUSESTATE));

	return NO_EVENT;
}

void CInput_Device::Free()
{
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pDInput);
}