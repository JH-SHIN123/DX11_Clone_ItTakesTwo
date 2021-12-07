#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)
public:
	explicit CInput_Device() = default;
	virtual ~CInput_Device() = default;

public: /* Enum */
	enum MOUSE_KEYSTATE		{ DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSE_MOVESTATE	{ DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };
	
public: /* Struct */
	typedef struct tagJoyStickCallBack
	{
		LPDIRECTINPUT8			pDInput;
		LPDIRECTINPUTDEVICE8*	ppJoyStick;
	}JS_CALLBACK;

public: /* Getter */
	_bool	Key_Up(_ubyte byKeyID);
	_bool	Key_Down(_ubyte byKeyID);
	_bool	Key_Pressing(_ubyte byKeyID);
	_bool	Mouse_Up(MOUSE_KEYSTATE eMouseKeyState);
	_bool	Mouse_Down(MOUSE_KEYSTATE eMouseKeyState);
	_bool	Mouse_Pressing(MOUSE_KEYSTATE eMouseKeyState);
	_long	Mouse_Move(MOUSE_MOVESTATE eMouseMoveState);
	_bool	Pad_Key_Up(_ubyte byPadKeyID);
	_bool	Pad_Key_Down(_ubyte byPadKeyID);
	_bool	Pad_Key_Pressing(_ubyte byPadKeyID);
	_long	Get_Pad_LStickX();
	_long	Get_Pad_LStickY();
	_long	Get_Pad_RStickX();
	_long	Get_Pad_RStickY();
	DIJOYSTATE& Get_PadState() { return m_JoyStickState; }

public:
	HRESULT	Ready_InputDevice(HINSTANCE hInst, HWND hWnd);
	_int	Tick(_bool bWndActivate);

private:
	LPDIRECTINPUT8			m_pDInput = nullptr;
	LPDIRECTINPUTDEVICE8	m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;
	LPDIRECTINPUTDEVICE8	m_pJoyStick = nullptr;
	_byte					m_byKeyState[256];
	_byte					m_byKeyState_Up[256];
	_byte					m_byKeyState_Down[256];
	_byte					m_byKeyState_Up_Buffer[256];
	_byte					m_byKeyState_Down_Buffer[256];
	DIMOUSESTATE			m_MouseState;
	DIMOUSESTATE			m_MouseState_Up;
	DIMOUSESTATE			m_MouseState_Down;
	DIMOUSESTATE			m_MouseState_Up_Buffer;
	DIMOUSESTATE			m_MouseState_Down_Buffer;
	DIJOYSTATE				m_JoyStickState;
	DIJOYSTATE				m_JoyStickState_Up;
	DIJOYSTATE				m_JoyStickState_Down;
	DIJOYSTATE				m_JoyStickState_Up_Buffer;
	DIJOYSTATE				m_JoyStickState_Down_Buffer;

public:
	virtual void Free() override;
};

END