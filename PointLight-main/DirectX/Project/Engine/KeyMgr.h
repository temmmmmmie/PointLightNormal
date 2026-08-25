#pragma once


enum KEY
{
	_0,
	_1,
	_2,
	_3,
	_4,
	_5,
	_6,
	_7,
	_8,
	_9,

	Q,
	W,
	E,
	R,
	T,
	Y,
	U,
	I,
	O,
	P,

	A,
	S,
	D,
	F,
	G,
	H,
	J,
	K,
	L,

	Z,
	X,
	C,
	V,
	B,
	N,
	M,

	ENTER,
	ALT,
	CTRL,
	SPACE,
	LSHIFT,
	ESC,
	END,

	LEFT,
	RIGHT,
	UP,
	DOWN,

	LBTN,
	RBTN,

	KEY_END,
};

enum KEY_STATE
{
	NONE,
	TAP,
	PRESSED,
	RELEASED,
};

struct tKeyInfo
{
	KEY_STATE State;
	bool	  PrevPressed;
};

class KeyMgr	
	: public singleton<KeyMgr>
{
	SINGLE(KeyMgr)
private:
	vector<tKeyInfo>	m_vecKeys;
	Vec2				m_MousePos;			// 이번 프레임 마우스 좌표
	Vec2				m_PrevMouse;		// 이전 프레임 마우스 좌표
	Vec2				m_MouseDragDir;		// 마우스 이동 방향벡터

public:
	KEY_STATE GetKeyState(KEY _Key) { return m_vecKeys[_Key].State; }

	Vec2 GetMousePos() { return m_MousePos; }
	Vec2 GetPrevMousePos() { return m_PrevMouse; }
	Vec2 GetMouseDragDir() { return m_MouseDragDir; }

public:
	void Tick();
};


#define KEY_CHECK(Key, State)  (KeyMgr::GetInst()->GetKeyState(Key) == State)
#define KEY_TAP(Key)			KEY_CHECK(Key, KEY_STATE::TAP)
#define KEY_PRESSED(Key)		KEY_CHECK(Key, KEY_STATE::PRESSED)
#define KEY_RELEASED(Key)		KEY_CHECK(Key, KEY_STATE::RELEASED)
