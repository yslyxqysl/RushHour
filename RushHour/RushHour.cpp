#pragma once

#include "RushHour.h"
#include "Random.h"
#include "Jumping.h"

BOOL IsOnFire = FALSE;
BOOL LeftKeyDown = FALSE;
INT HeroVelocity = 0;
INT ScorePerFrame = 4;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(time(0));

	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"),	_T("Mega Plane"), MB_OK);
		return 1;
	}

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create,~WS_THICKFRAME  fixed window size
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// WNDWIDTH, WNDHEIGHT: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	hWnd = CreateWindow(szWindowClass, szTitle,	WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
						CW_USEDEFAULT, CW_USEDEFAULT, WNDWIDTH, WNDHEIGHT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Mega Plane"), MB_OK);
		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
			//��ʼ��
			Init(hWnd, wParam, lParam);
			break;
		case WM_PAINT:
			//����
			Render(hWnd);
			break;
		case WM_KEYDOWN:
			//���̰����¼�
			KeyDown(hWnd, wParam, lParam);
			break;
		case WM_KEYUP:
			//�����ɿ��¼�
			KeyUp(hWnd, wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			//����ƶ��¼�
			MouseMove(hWnd, wParam, lParam);
			break;
		case WM_LBUTTONDOWN:
			//������¼�
			LeftKeyDown = TRUE;
			LButtonDown(hWnd, wParam, lParam);
			break;
		case WM_LBUTTONUP:
			LeftKeyDown = FALSE;
			LButtonUp(hWnd, wParam, lParam);
			break;
		case WM_TIMER:
			//��ʱ���¼�
			TimerUpdate(hWnd, wParam, lParam);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}
	return 0;
}

VOID Init(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//����Buildingλͼ
	m_hBuildingBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BUILDING));

	//����Ӣ��λͼ
	m_hHeroBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_HERO));
	m_hHeroUpBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_HEROUP));
	m_hHeroDownBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_HERODOWN));
	//m_hero = CreateHero(WNDWIDTH / 4, WNDHEIGHT - HERO_TO_GROUND, HERO_SIZE_X, HERO_SIZE_Y, m_hHeroBmp, 0, 0, HERO_MAX_FRAME_NUM);

	//������Ϸ״̬λͼ
	m_hGameStatusBmp =  LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_GAME_STATUS));
	//m_gameStatus = CreateGameStatus(10, 8, GAME_STATUS_SIZE_X, GAME_STATUS_SIZE_Y, m_hGameStatusBmp);

	//���ر���λͼ
	int k;
	for (k = 0; k < BACKGROUND_COLOR_NUM; k++)
	{
		m_hBackgroundBmp[k] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(m_backgroundBmpNames[k]));
	}
	for (k = 0; k < MAX_BACKGROUND_NUM; k++)
	{
		int randk = 0;
		m_background[k] = CreateBackground(BACKGROUND_SIZE_X * k, 0, BACKGROUND_SIZE_X, BACKGROUND_SIZE_Y, m_hBackgroundBmp[randk]);
	}

	//���ذ�ťλͼ
	m_hOthersBmp[0] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(m_othersBmpNames[0]));
	m_others[0] = CreateOthers(WNDWIDTH - 425, 0, 425, 460, m_hOthersBmp[0]);
	m_hOthersBmp[1] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(m_othersBmpNames[1]));
	m_others[1] = CreateOthers(250, 100, 450 * 1.2, 250 * 1.2, m_hOthersBmp[1]);
	for (k = 0; k < BUTTON_NUM * 2; k++)
	{
		m_hButtonBmp[k] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(m_buttonBmpNames[k]));
		int group = k / 2;
		if (k % 2 == 0)
			m_button[k] = CreateButton(WNDWIDTH  - 340, group * 90 + 5, 320, 90, m_hButtonBmp[k], TRUE);
		else
			m_button[k] = CreateButton(WNDWIDTH  - 340, group * 90 + 5, 320, 90, m_hButtonBmp[k], FALSE);
	}
	

	//����Blockλͼ
	//int k;
	/*for (k = 0; k < BLOCK_COLOR_NUM; k++)
	{
		m_hBlockBmp[k] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(m_blockBmpNames[k]));
	}
	//�����ݶ�λͼ
	for (k = 0; k < ROOF_COLOR_NUM; k++)
	{
		m_hRoofkBmp[k] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(m_roofBmpNames[k]));
	}*/
	
	for (int k = 0; k < OTHERS_NUM * 2; k++)
	{
		m_hOthersBmp[k] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(m_othersBmpNames[k]));
	}
	//����Ӣ�ۡ�����
	
	//m_building = CreateBuilding(0, 70, BUILDING_SIZE_X, BUILDING_SIZE_Y, m_hBuildingBmp);
	//��������
	
	//��������
	/*for (k = 0; k < MAX_TERRIAN_NUM; k++)
	{
		if (k % 4 == 0) continue;
		m_terrian[k] = CreateTerrian(k * 65, 100 + 50 * (k % 2), BLOCK_SIZE_X, 300, m_hBlockBmp[k % 4], m_hRoofkBmp[k % 2], ROOF_SIZE_Y, BLOCK_SIZE_Y);
	}*/
	//������Ϸ״̬
	
	//������ʱ��
	//SetTimer(hWnd, TIMER_ID, TIMER_ELAPSE, NULL);
	//SetTimer(hWnd, SCORE_ID, SCORE_ELAPSE, NULL);
}

VOID Render(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc;
	//��ʼ����
	hdc = BeginPaint(hWnd, &ps);

	HDC	hdcBmp, hdcBuffer;
	HBITMAP	cptBmp;

	cptBmp = CreateCompatibleBitmap(hdc, WNDWIDTH, WNDHEIGHT);
	hdcBmp = CreateCompatibleDC(hdc);
	hdcBuffer = CreateCompatibleDC(hdc);
	SelectObject(hdcBuffer, cptBmp);

	//���Ʊ���������
	int k;
	for (k = 0; k < MAX_BACKGROUND_NUM; k++)
	{
		Background background = m_background[k];
		SelectObject(hdcBmp, background.hBmp);
		BitBlt(
		hdcBuffer, background.pos.x, background.pos.y,
		background.size.cx, background.size.cy,
		hdcBmp, 0, 0, SRCCOPY);
	}
	//���ư�ť������
	for (k = 0; k < OTHERS_NUM; k++)
	{
		Others others = m_others[k];
		SelectObject(hdcBmp, others.hBmp);
		if (k == 0)
		{
			TransparentBlt(
				hdcBuffer, others.pos.x, others.pos.y,
				others.size.cx, others.size.cy,
				hdcBmp, 0, 0, 285, 462,
				RGB(255, 255, 255)
			);
		}
		else
		{
			TransparentBlt(
				hdcBuffer, others.pos.x, others.pos.y,
				others.size.cx, others.size.cy,
				hdcBmp, 0, 0, 450, 250,
				RGB(255, 255, 255)
			);
		}
	}
	for (k = 0; k < BUTTON_NUM * 2; k++)
	{
		Button button = m_button[k];
		if (button.active)
		{
			SelectObject(hdcBmp, button.hBmp);
			TransparentBlt(
				hdcBuffer, button.pos.x, button.pos.y,
				button.size.cx, button.size.cy,
				hdcBmp, 0, 0, BUTTON_SIZE_X, BUTTON_SIZE_Y,
				RGB(255, 255, 255)
			);
		}
	}
	//���ƽ���������
	/*SelectObject(hdcBmp, m_building.hBmp);
	TransparentBlt(
		hdcBuffer, m_building.pos.x, m_building.pos.y,
		m_building.size.cx, m_building.size.cy,
		hdcBmp, 0, 0, m_building.size.cx, m_building.size.cy,
		RGB(255, 255, 255)
		);*/

	//����Hero������
	SelectObject(hdcBmp, m_hero.hBmp);
	int HeroBitLabel;
	if (m_hero.Status == 0) HeroBitLabel = m_hero.curFrameIndex % HERO_MAX_FRAME_NUM;
	if (m_hero.Status == 1) HeroBitLabel = HERO_MAX_FRAME_NUM + m_hero.curFrameIndex % HERO_MAX_FRAME_UP;
	if (m_hero.Status == 2) HeroBitLabel = HERO_MAX_FRAME_NUM + HERO_MAX_FRAME_UP + m_hero.curFrameIndex % HERO_MAX_FRAME_DOWN;
	TransparentBlt(
		hdcBuffer, m_hero.pos.x, m_hero.pos.y,
		m_hero.size.cx, m_hero.size.cy,
		hdcBmp, 0, m_hero.size.cy * HeroBitLabel, m_hero.size.cx, m_hero.size.cy,
		RGB(255, 255, 255)
		);
	//���Ƶ���
	//int k;
	/*for (k = 0; k < MAX_TERRIAN_NUM; k++)
	{
		Terrian terrian = m_terrian[k];
		SelectObject(hdcBmp, terrian.hRoofBmp);
		TransparentBlt(
			hdcBuffer, terrian.pos.x, terrian.pos.y,
			terrian.roofWidth, terrian.roofHeight,
			hdcBmp, 0, 0, terrian.roofWidth, terrian.roofHeight,
			RGB(255, 255, 255)
			);
		SelectObject(hdcBmp, terrian.hBlockBmp);
		int t;
		for (t = 0; t < terrian.blockNum; t++)
		{
			TransparentBlt(
				hdcBuffer, terrian.pos.x, terrian.pos.y + terrian.roofHeight + terrian.blockHeight * t,
				terrian.blockWidth, terrian.blockHeight,
				hdcBmp, 0, 0, terrian.blockWidth, terrian.blockHeight,
				RGB(255, 255, 255)
				);
		}
	}*/

	//������Ϸ״̬
	//��ͣ�����λͼ
	SelectObject(hdcBmp, m_gameStatus.hBmp);
	TransparentBlt(hdcBuffer, m_gameStatus.pos.x, m_gameStatus.pos.y, m_gameStatus.size.cx, m_gameStatus.size.cy,
		hdcBmp, 0, m_gameStatus.size.cy * m_gameStatus.isPaused,
		m_gameStatus.size.cx, m_gameStatus.size.cy, RGB(255, 255, 255));

	//���Ʒ���
	TCHAR szDist[13];
	SetTextColor(hdcBuffer, RGB(0, 0, 0));
	SetBkMode(hdcBuffer, TRANSPARENT);
	TextOut(hdcBuffer, 100, 15, szDist, wsprintf(szDist, _T("����:%d"), m_gameStatus.totalDist));

	//������е���Ϣ���Ƶ���Ļ��
	BitBlt(hdc, 0, 0, WNDWIDTH, WNDHEIGHT, hdcBuffer, 0, 0, SRCCOPY);

	//������Դ��ռ���ڴ�
	DeleteObject(cptBmp);
	DeleteDC(hdcBuffer);
	DeleteDC(hdcBmp);

	//��������
	EndPaint(hWnd, &ps);
}

Hero CreateHero(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp, int curFrameIndex, int Status, int maxFrameSize)
{
	Hero hero;
	hero.hBmp = hBmp;
	hero.pos.x = posX;
	hero.pos.y = posY;
	hero.size.cx = sizeX;
	hero.size.cy = sizeY;
	hero.curFrameIndex = curFrameIndex;
	hero.Status = Status;
	hero.maxFrameSize = maxFrameSize;
	return hero;
}

Background CreateBackground(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp)
{
	Background background;
	background.hBmp = hBmp;
	background.pos.x = posX;
	background.pos.y = posY;
	background.size.cx = sizeX;
	background.size.cy = sizeY;
	return background;
}

Button CreateButton(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp, BOOL active)
{
	Button button;
	button.hBmp = hBmp;
	button.pos.x = posX;
	button.pos.y = posY;
	button.size.cx = sizeX;
	button.size.cy = sizeY;
	button.active = active;
	return button;
}

Others CreateOthers(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp)
{
	Others others;
	others.hBmp = hBmp;
	others.pos.x = posX;
	others.pos.y = posY;
	others.size.cx = sizeX;
	others.size.cy = sizeY;
	return others;
}

Building CreateBuilding(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp)
{
	Building building;
	building.hBmp = hBmp;
	building.pos.x = posX;
	building.pos.y = posY;
	building.size.cx = sizeX;
	building.size.cy = sizeY;
	return building;
}

GameStatus CreateGameStatus(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp)
{
	GameStatus gameStatus;
	gameStatus.pos.x = posX;
	gameStatus.pos.y = posY;
	gameStatus.size.cx = sizeX;
	gameStatus.size.cy = sizeY;
	gameStatus.hBmp = hBmp;
	gameStatus.totalDist = 0;
	gameStatus.isPaused = false;
	return gameStatus;
}

Terrian CreateTerrian(LONG posX, LONG posY, LONG sizeX, LONG sizeY, 
					  HBITMAP hBlockBmp, HBITMAP hRoofBmp, int roofHeight, int blockHeight)
{
	Terrian terrian;
	terrian.pos.x = posX;
	terrian.pos.y = posY;
	terrian.size.cx = sizeX;
	terrian.size.cy = sizeY;
	terrian.hBlockBmp = hBlockBmp;
	terrian.hRoofBmp = hRoofBmp;
	terrian.roofWidth = sizeX;
	terrian.roofHeight = roofHeight;
	terrian.blockWidth = sizeX;
	terrian.blockHeight = blockHeight;
	terrian.blockNum = (int)ceil((sizeY - roofHeight) * 1.0 / blockHeight); 
	return terrian;
}

VOID TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (wParam == TIMER_ID)
	{
		HeroUpdate();
		BackgroundUpdate();
		TerrianUpdate();
		GameStatusUpdate();
	}
	if (wParam == SCORE_ID)
	{
		//GameStatusUpdate();
		ScorePerFrame++;
	}
	InvalidateRect(hWnd, NULL, FALSE);
}

VOID HeroUpdate()
{
	//TODO
	//����λ��
	//m_hero.pos.x += 1;
	//m_hero.pos.x = m_hero.pos.x >= WNDWIDTH ? 0 : m_hero.pos.x;
	//���¶���
	int HeroAcceleration;
	if (IsOnFire)
	{
		HeroAcceleration = -Gravity;
		if (HeroVelocity > 0) HeroAcceleration = -1.5 * Gravity;
		m_hero.Status = 1;
	}
	else
	{
		HeroAcceleration = Gravity;
		if (HeroVelocity < 0) HeroAcceleration = 1.5 * Gravity;
		m_hero.Status = 2;
	}
	int HeroDisplacement = HeroVelocity + 0.5 * HeroAcceleration;
	HeroVelocity += HeroAcceleration;
	int HeroPosY = m_hero.pos.y + HeroDisplacement;
	if (HeroPosY < 0)
	{
		HeroPosY = 0;
		HeroVelocity = 0;
	}
	else if (HeroPosY > WNDHEIGHT - HERO_TO_GROUND)
	{
		HeroPosY = WNDHEIGHT - HERO_TO_GROUND;
		HeroVelocity = 0;
		m_hero.Status = 0;
	}
	m_hero.pos.y = HeroPosY;

	++m_hero.curFrameIndex;
	m_hero.curFrameIndex = m_hero.curFrameIndex >= HERO_MAX_FRAME ? 0 : m_hero.curFrameIndex;
}

VOID BackgroundUpdate()
{
	int k;
	for(k = 0; k < MAX_BACKGROUND_NUM; k++)
	{
		m_background[k].pos.x -= ScorePerFrame * 4;
		if (m_background[k].pos.x + m_background[k].size.cx < 0)
		{
			m_background[k].pos.x += MAX_BACKGROUND_NUM * BACKGROUND_SIZE_X;
			int randk = GetRandomInt(0, BACKGROUND_COLOR_NUM - 1);
			m_background[k].hBmp = m_hBackgroundBmp[randk];
		}
	}
}

VOID TerrianUpdate()
{
	//TODO
	int k;
	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		m_terrian[k].pos.x -= 2;
		if (m_terrian[k].pos.x + m_terrian[k].size.cx < 0)
		{
			m_terrian[k].pos.x += MAX_TERRIAN_NUM * m_terrian[k].size.cx;
		}
	}
}

VOID GameStatusUpdate()
{
	//TODO
	m_gameStatus.totalDist += ScorePerFrame;
}

BOOL Paused(POINT ptMouse)
{
	RECT rPause;

	rPause.left = m_gameStatus.pos.x;
	rPause.top = m_gameStatus.pos.y;
	rPause.right = m_gameStatus.size.cx;
	rPause.bottom = m_gameStatus.size.cy;

	return PtInRect(&rPause, ptMouse);
}

BOOL MouseInButton(POINT ptMouse, Button button)
{
	RECT rButton;

	rButton.left = button.pos.x;
	rButton.top = button.pos.y;
	rButton.right = button.pos.x + 320;
	rButton.bottom = button.pos.y + 90;

	return PtInRect(&rButton, ptMouse);
}

VOID KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//TODO
	switch (wParam)
	{
	case VK_UP:
		//m_hero.pos.y -= 50;
		IsOnFire = true;
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_DOWN:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	default:
		break;
	}
}

VOID KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//TODO
	switch (wParam)
	{
	case VK_UP:
		//m_hero.pos.y += 50;
		IsOnFire = false;
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_DOWN:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	default:
		break;
	}
}

VOID MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);

	//320 * 90
	for (int i = 0; i < BUTTON_NUM * 2; i += 2)
	{
		int group = i / 2;
		if(MouseInButton(ptMouse, m_button[i]))
		{
			m_button[i].active = FALSE;
			m_button[i + 1].active = TRUE;
			if (LeftKeyDown)
			{
				m_button[i].pos.x = m_button[i + 1].pos.x = WNDWIDTH - 340 + 16;
				m_button[i].pos.y = m_button[i + 1].pos.y = group * 90 + 5 + 4.5;
				m_button[i].size.cx = m_button[i + 1].size.cx = 320 - 16 * 2;
				m_button[i].size.cy = m_button[i + 1].size.cy = 90 - 4.5 * 2;
			}
		}
		else
		{
			m_button[i].active = TRUE;
			m_button[i + 1].active = FALSE;
			if (LeftKeyDown)
			{
				m_button[i].pos.x = m_button[i + 1].pos.x = WNDWIDTH - 340;
				m_button[i].pos.y = m_button[i + 1].pos.y = group * 90 + 5;
				m_button[i].size.cx = m_button[i + 1].size.cx = 320;
				m_button[i].size.cy = m_button[i + 1].size.cy = 90;
			}
		}
	}
	InvalidateRect(hWnd, NULL, FALSE);
}

VOID LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < BUTTON_NUM * 2; i += 2)
	{
		int group = i / 2;
		m_button[i].pos.x = m_button[i + 1].pos.x = WNDWIDTH - 340;
		m_button[i].pos.y = m_button[i + 1].pos.y = group * 90 + 5;
		m_button[i].size.cx = m_button[i + 1].size.cx = 320;
		m_button[i].size.cy = m_button[i + 1].size.cy = 90;
	}
	InvalidateRect(hWnd, NULL, FALSE);
}

VOID LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);

	//�������˼�������ͣͼ��
	
	if (Paused(ptMouse))
	{
		if (!m_gameStatus.isPaused)
		{
			KillTimer(hWnd, TIMER_ID);
			KillTimer(hWnd, SCORE_ID);
			m_gameStatus.isPaused = TRUE;
		}
		else
		{
			SetTimer(hWnd, TIMER_ID, TIMER_ELAPSE, NULL);
			SetTimer(hWnd, SCORE_ID, SCORE_ELAPSE, NULL);
			m_gameStatus.isPaused = FALSE;
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
	else
	{
		//m_button[k] = CreateButton(WNDWIDTH  - 340, group * 90 + 5, 320, 90, m_hButtonBmp[k], TRUE);
		for (int i = 0; i < BUTTON_NUM * 2; i += 2)
		{
			int group = i / 2;
			if(MouseInButton(ptMouse, m_button[i]))
			{
				m_button[i].pos.x = m_button[i + 1].pos.x = WNDWIDTH - 340 + 16;
				m_button[i].pos.y = m_button[i + 1].pos.y = group * 90 + 5 + 4.5;
				m_button[i].size.cx = m_button[i + 1].size.cx = 320 - 16 * 2;
				m_button[i].size.cy = m_button[i + 1].size.cy = 90 - 4.5 * 2;
			}
			else
			{
				m_button[i].pos.x = m_button[i + 1].pos.x = WNDWIDTH - 340;
				m_button[i].pos.y = m_button[i + 1].pos.y = group * 90 + 5;
				m_button[i].size.cx = m_button[i + 1].size.cx = 320;
				m_button[i].size.cy = m_button[i + 1].size.cy = 90;
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
}