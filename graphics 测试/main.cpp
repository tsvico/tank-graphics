#include <graphics.h>
#include <conio.h>
#include <mmsystem.h>
#include <Windows.h>
#include <time.h>
#pragma comment (lib,"winmm.lib")

struct TANK
{
	int r;
	int c;
	int towards;
}; TANK tank;

#define W 1001
#define S 1002
#define A 1003
#define D 1004

bool gamewin2;
bool gamewin = true;
bool gong = true;
int Hp = 5;
int Hp2;
int g_towards = W;
CRITICAL_SECTION g_cs;
bool TankRequest = true;
IMAGE blank, woodwall, stonewall, end, playerW, playerS
, playerA, playerD,boom,fire,TankW,TankS,TankA,TankD,destory,gameover,tankfire,win;
//空地0 木墙1 石墙2 坦克3 家4 敌方坦克W S A D
int map[12][15] = {
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 },
	{ 2, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2 },
	{ 2, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0, 2 },
	{ 2, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 2 },
	{ 2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2 },
	{ 2, 2, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 2, 2 },
	{ 2, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 2 },
	{ 2, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2 },
	{ 2, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 2 },
	{ 2, 0, 0, 0, 0, 3, 1, 4, 1, 0, 0, 0, 0, 0, 2 },
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }
};


DWORD WINAPI Win(LPVOID lpParam);
DWORD WINAPI PlayGame(LPVOID lpParam);
DWORD WINAPI InitTank(LPVOID lpParam);
DWORD WINAPI FireProc(LPVOID lpParam);
DWORD WINAPI TankMove(LPVOID lpParam);
DWORD WINAPI TankFire(LPVOID lpParam);
DWORD WINAPI TankFireRequest(LPVOID lpParam);
DWORD WINAPI Lost(LPVOID lpParam);

void GameOver();
void DrawMap();
void InitGame();
void GameMove();
int main()
{
	::InitializeCriticalSection(&g_cs);
	InitGame();
	::CreateThread(0, 0, InitTank, 0, 0, 0);
	::CreateThread(0, 0, PlayGame, 0, 0, 0);
	while (gamewin)
	{
		DrawMap();
		GameMove();
	}
	Sleep(2000);
	if (gamewin2 == false)
	{
		::CreateThread(0, 0, Lost, 0, 0, 0);
		MessageBox(NULL, L"You lost", L"message", MB_OK);
	}
	else
	{
		::CreateThread(0, 0, Win, 0, 0, 0);
		MessageBox(NULL, L"You win", L"message", MB_OK);
	}
	
	getch();
	::DeleteCriticalSection(&g_cs);
	return 0;
}
void InitGame()
{
	Hp2=3;
	initgraph(780, 600);
	mciSendString(L"open ./music/bgm1.mp3 alias bgm", 0, 0, 0);
	mciSendString(L"play bgm repeat", 0, 0, 0);
	//blank, woodwall, stonewall, end, playerW, playerS, playerA, playerD
	loadimage(&blank, L"./image/blank.gif", 60, 60);
	loadimage(&woodwall, L"./image/woodwall.gif", 60, 60);
	loadimage(&stonewall, L"./image/stonewall.gif", 60, 60);
	loadimage(&end, L"./image/end.gif", 60, 60);
	loadimage(&playerW, L"./image/playerW.gif", 60, 60);
	loadimage(&playerS, L"./image/playerS.gif", 60, 60);
	loadimage(&playerA, L"./image/playerA.gif", 60, 60);
	loadimage(&playerD, L"./image/playerD.gif", 60, 60);
	loadimage(&boom, L"./image/boom.gif", 60, 60);
	loadimage(&fire, L"./image/fire.gif", 60, 60);
	loadimage(&TankW, L"./image/TankW.gif", 60, 60);
	loadimage(&TankS, L"./image/TankS.gif", 60, 60);
	loadimage(&TankA, L"./image/TankA.gif", 60, 60);
	loadimage(&TankD, L"./image/TankD.gif", 60, 60);
	loadimage(&destory, L"./image/destory.gif", 60, 60);
	loadimage(&gameover, L"./image/gameover.gif", 780, 600);
	loadimage(&tankfire, L"./image/fire.gif", 60, 60);
	loadimage(&win, L"./image/gamewin.gif", 780, 600);
}
void DrawMap()
{
	for (int i = 1; i < 14; i++)
	{
		for (int j = 1; j < 11; j++)
		{
			switch (map[j][i])
			{
			case -1:
				putimage((i - 1) * 60, (j - 1) * 60, &destory);
				break;
			case W:
				putimage((i - 1) * 60, (j - 1) * 60, &TankW);
				break;
			case S:
				putimage((i - 1) * 60, (j - 1) * 60, &TankS);
				break;
			case A:
				putimage((i - 1) * 60, (j - 1) * 60, &TankA);
				break;
			case D:
				putimage((i - 1) * 60, (j - 1) * 60, &TankD);
				break;
			case 0:
				putimage((i-1) * 60, (j-1) * 60, &blank);
				break;
			case 1:
				putimage((i-1) * 60, (j-1) * 60, &woodwall);
				break;
			case 2:
				putimage((i-1) * 60, (j-1) * 60, &stonewall);
				break;
			case 4:
				putimage((i-1) * 60, (j-1) * 60, &end);
				break;
			case 3:
				switch (g_towards)
				{
				case W:
					putimage((i-1) * 60, (j-1) * 60, &playerW);
					break;
				case S:
					putimage((i-1) * 60, (j-1) * 60, &playerS);
					break;
				case A:
					putimage((i-1) * 60, (j-1) * 60, &playerA);
					break;
				case D:
					putimage((i-1) * 60, (j-1) * 60, &playerD);
					break;
				}
			}
		}
	}
}
void GameMove()
{
	int r, c;
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			if (map[j][i] == 3)
			{
				r = j;
				c = i;
			}
		}
	}
	char ch;
	HANDLE hThread;
	ch = getch();
	switch (ch)
	{
	case 'J':
	case 'j':
		hThread = CreateThread(0, 0, FireProc, 0, 0, 0);
		break;
	case 'W':
	case 'w':
		g_towards = W;
		if (map[r - 1][c] == 0)
		{
			
			map[r][c] = 0;
			map[r - 1][c] = 3;
		}
		break;
	case 'S':
	case 's':
		g_towards = S;
		if (map[r + 1][c] == 0)
		{
			
			map[r][c] = 0;
			map[r + 1][c] = 3;
		}
		break;
	case 'A':
	case 'a':
		g_towards = A;
		if (map[r][c-1] == 0)
		{
			
			map[r][c] = 0;
			map[r][c-1] = 3;
		}
		break;
	case 'D':
	case 'd':
		g_towards = D;
		if (map[r][c + 1] == 0)
		{
			
			map[r][c] = 0;
			map[r][c + 1] = 3;
		}
		break;
	}
	
}
//线程参考
/*
#include <Windows.h>
#include <iostream>
using namespace std;
DWORD WINAPI fun(void* g)
{
	while (1) // 线程调用fun函数，内部一直打印
	{
		Sleep(1000);
		cout << (char*)g << endl;
	}
	return 0;
}
int main()
{
	char* a = "abcd";
	::CreateThread(0, 0, fun, a, 0, 0); // 建立线程，线程调用fun函数
	Sleep(300);
	while (1) // 主函数一直打印
	{
		Sleep(1000);
		cout << "print main" << endl;
	}
	return 0;
}*/
DWORD WINAPI FireProc(LPVOID lpParam)
{

	if (!gong)
	{
		return 0;
	}
	
	EnterCriticalSection(&g_cs);
	gong = false;
	int towards = g_towards;
	int r, c;
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			if (map[j][i] == 3)
			{
				r = j;
				c = i;
			}
		}
	}
	int m = 0, n = 0;
	switch (towards)
	{
	case W:
		n = -1;
		break;
	case S:
		n = 1;
		break;
	case A:
		m = -1;
		break;
	case D:
		m = 1;
		break;
	}
	mciSendString(L"open ./music/bgm2.mp3 alias bgm2", 0, 0, 0);
	mciSendString(L"play bgm2", 0, 0, 0);
	while (map[r][c] != 2)
	{
		if (map[r][c] == 1)
		{
			
			map[r][c] = 0;
			putimage((c - 1) * 60, (r - 1) * 60, &boom);
			Sleep(500);
			DrawMap();
			break;
		}
		if (map[r][c] == W || map[r][c] == S || map[r][c] == A || map[r][c] == D )
		{
			Hp--;
			break;
		}
		if (map[r][c] == 2)
		{
			break;
		}
		if (map[r][c] == 4)
		{
			putimage(6 * 60, 9 * 60, &blank);
			putimage(6 * 60, 9 * 60, &destory);
			map[10][7] = -1;
			GameOver();
			return 0;
		}
		switch (towards)
		{
		case W:
			putimage((c - 1) * 60, (r - 2) * 60, &fire);
			break;
		case S:
			putimage((c - 1) * 60, (r + 2) * 60, &fire);
			break;
		case A:
			putimage((c - 2) * 60, (r - 1) * 60, &fire);
			break;
		case D:
			putimage((c + 2) * 60, (r - 1) * 60, &fire);
			break;
		}
			
		Sleep(100);
		DrawMap();
		r = r + n;
		c = c + m;
		
	}
	gong = true;
	LeaveCriticalSection(&g_cs);
	
	return 0;
}
DWORD WINAPI InitTank(LPVOID lpParam)
{
	srand((unsigned int)time(NULL));
		if (TankRequest)
		{
			Hp = 5;
			tank.r = 1;
			tank.c = rand() % 13 + 1;
			while (map[tank.r][tank.c] == 3)
			{
				tank.c = rand() % 13 + 1;
			}
			
			tank.towards = S;
			map[tank.r][tank.c] = S;
			Sleep(3000);
			::CreateThread(0, 0, TankMove, 0, 0, 0);
			::CreateThread(0, 0, TankFireRequest, 0, 0, 0);
			TankRequest = false;
		}
	return 0;
}
DWORD WINAPI TankMove(LPVOID lpParam)
{
	srand((unsigned int)time(NULL));
	int T;
	//::CreateThread(0, 0, TankFire, 0, 0, 0);
	while (1)
	{
		T = rand() % 2;
		if (map[1][7] == W || map[1][7] == S || map[1][7] == A || map[1][7] == D)
		{
			switch (T)
			{
			case 0:
				tank.towards = A;
			case 1:
				tank.towards = D;
			}
		}
		else
		{
			if (map[tank.r + 1][tank.c] == 0)
			{
				tank.towards = S;
			}
			else if (map[tank.r][tank.c - 1] == 0 && map[tank.r][tank.c + 1] == 0)
			{
 				switch (T)
				{
				case 0:
					tank.towards = A;
				case 1:
					tank.towards = D;
				}
			}
			else if (map[tank.r][tank.c - 1] == 0)
			{
				tank.towards = A;
			}
			else if (map[tank.r][tank.c + 1] == 0)
			{
				tank.towards = D;
			}
			else
			{
				tank.towards = W;
			}
		}
		switch (tank.towards)
		{
		case W:
			map[tank.r][tank.c] = W;
			if (map[tank.r - 1][tank.c] == 0)
			{

				map[tank.r][tank.c] = 0;
				map[tank.r - 1][tank.c] = W;
				tank.r = tank.r - 1;
			}
			break;
		case S:
			map[tank.r][tank.c] = S;
			if (map[tank.r + 1][tank.c] == 0)
			{

				map[tank.r][tank.c] = 0;
				map[tank.r + 1][tank.c] = S;
				tank.r = tank.r + 1;
			}
			break;
		case A:
			map[tank.r][tank.c] = A;
			if (map[tank.r][tank.c - 1] == 0)
			{
				map[tank.r][tank.c] = 0;
				map[tank.r][tank.c - 1] = A;
				tank.c = tank.c - 1;
			}
			break;
		case D:
			map[tank.r][tank.c] = D;
			if (map[tank.r][tank.c + 1] == 0)
			{

				map[tank.r][tank.c] = 0;
				map[tank.r][tank.c + 1] = D;
				tank.c = tank.c + 1;
			}
			break;
		}
		if (Hp <= 0)
		{
			map[tank.r][tank.c] = 0;
			TankRequest = true;
			putimage((tank.c - 1) * 60, (tank.r - 1) * 60, &boom);
			::CreateThread(0, 0, InitTank, 0, 0, 0);
			Sleep(200);
			DrawMap();
			return 0;
		}
		DrawMap();
		Sleep(1000);
	}
	return 0;
}
DWORD WINAPI TankFire(LPVOID lpParam)
{
	int m = 0, n = 0;
	int towards = tank.towards;
	switch (towards)
	{
	case W:
		n = -1;
		break;
	case S:
		n = 1;
		break;
	case A:
		m = -1;
		break;
	case D:
		m = 1;
		break;
	}
	int r = tank.r + m;
	int c = tank.c + n;
	switch (towards)
	{
	case W:
		c++;
		break;
	case S:
		c--;
		break;
	case A:
		r++;
		break;
	case D:
		r--;
		break;
	}
	while (map[r][c] != 2)
	{
		if (map[r][c] == 1)
		{

			map[r][c] = 0;
			putimage((c - 1) * 60, (r - 1) * 60, &boom);
			Sleep(500);
			DrawMap();
			break;
		}
		if (map[r][c] == 4)
		{
			putimage(6 * 60, 9 * 60, &blank);
			putimage(6 * 60, 9 * 60, &destory);
			map[10][7] = -1;
			GameOver();
			return 0;
		}
		if (map[r][c] == 3)
		{
			Hp2--;
			if (Hp2 <= 0)
			{
				GameOver();
				return 0;
			}
			int r2, c2;
			for (int i = 0; i < 15; i++)
			{
				for (int j = 0; j < 12; j++)
				{
					if (map[j][i] == 3)
					{
						r2 = j;
						c2 = i;
					}
				}
			}
			map[r2][c2] = 0;
			map[10][5] = 3;
			DrawMap();
			break;
		}
		if (map[r][c] == 2 || map[r][c] == 4)
		{
			break;
		}
		switch (towards)
		{
		case W:
			putimage((c - 1) * 60, (r - 2) * 60, &tankfire);
			break;
		case S:
			putimage((c - 1) * 60, (r + 2) * 60, &tankfire);
			break;
		case A:
			putimage((c - 2) * 60, (r - 1) * 60, &tankfire);
			break;
		case D:
			putimage((c + 2) * 60, (r - 1) * 60, &tankfire);
			break;
		}

		Sleep(150);
		DrawMap();
		r = r + n;
		c = c + m;

	}
}
DWORD WINAPI TankFireRequest(LPVOID lpParam)
{
	while (!TankRequest)
	{
		Sleep(3000);
		::CreateThread(0, 0, TankFire, 0, 0, 0);
	}
	return 0;
}
void GameOver()
{
	gamewin2 = false;
	gamewin = false;
}
DWORD WINAPI Lost(LPVOID lpParam)
{
	while (1)
	{
		putimage(0, 0, &gameover);
	}
	return 0;
}
DWORD WINAPI PlayGame(LPVOID lpParam)
{
	Sleep(180000);
	gamewin2 = true;
	gamewin = false;
	return 0;
}
DWORD WINAPI Win(LPVOID lpParam)
{
	while (1)
	{
		putimage(0, 0, &win);
	}
	return 0;
}