#include "pch.h"
#include "KeyMgr.h"

#include "Engine.h"

int KeyData[KEY_END] =
{
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',

	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',
	'A',
	'S',
	'D',
	'F',
	'G',
	'H',
	'J',
	'K',
	'L',
	'Z',
	'X',
	'C',
	'V',
	'B',
	'N',
	'M',

	VK_RETURN,
	VK_MENU,
	VK_CONTROL,
	VK_SPACE,
	VK_LSHIFT,
	VK_ESCAPE,
	VK_END,

	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,

	VK_LBUTTON,
	VK_RBUTTON,
};


KeyMgr::KeyMgr()
{
	m_vecKeys.resize(KEY_END);
}

KeyMgr::~KeyMgr()
{
}

void KeyMgr::Tick()
{
	for (size_t i = 0; i < m_vecKeys.size(); ++i)
	{
		// 지금 눌려있는지 체크
		if (GetAsyncKeyState(KeyData[i]))
		{
			// 이전 프레임에 눌렸었다.
			if (m_vecKeys[i].PrevPressed)
			{
				m_vecKeys[i].State = PRESSED;
			}

			// 이전 프레임에는 안눌려있었다.
			else
			{
				m_vecKeys[i].State = TAP;
			}

			m_vecKeys[i].PrevPressed = true;
		}

		// 지금 눌려있지 않다.
		else
		{
			// 이전 프레임에는 눌려있었다.
			if (m_vecKeys[i].PrevPressed)
			{
				m_vecKeys[i].State = RELEASED;
			}

			// 이전 프레임에도 안눌려있었다.
			else
			{
				m_vecKeys[i].State = NONE;
			}

			m_vecKeys[i].PrevPressed = false;
		}
	}

	// 마우스 좌표 계산
	POINT ptMouse = {};

	// Screen 좌표계	
	GetCursorPos(&ptMouse); 

	// Screen 좌표계 -> Client 기준 좌표계
	ScreenToClient(Engine::GetInst()->GetMainWnd(), &ptMouse); 

	m_PrevMouse = m_MousePos;

	m_MousePos = Vec2((float)ptMouse.x, (float)ptMouse.y);

	m_MouseDragDir = m_MousePos - m_PrevMouse;
}