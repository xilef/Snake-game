/********************************************************/
/* Main program											*/
/********************************************************/
#include "stdafx.h"
#include "snake_class.h"

/********************************************************/
/* Constants declaration								*/
/********************************************************/
#define MAP_WIDTH 200
#define MAP_HEIGHT 200
#define INIT_LEN 5
#define BODY_SIZE 10
#define UP 72
#define LEFT 75
#define DOWN 80
#define RIGHT 77

/********************************************************/
/* Global variables										*/
/********************************************************/
LPCWSTR class_name = L"SnakeClass";
Snake* sn;
int foodx, foody;
int headx, heady;
int score;
bool start;

/********************************************************/
/* Function prototypes									*/
/********************************************************/
LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wparm, LPARAM lparm);
void FlushKeyboardInputBuffer (HWND);
void create_food ();
void init ();
void key_input ();

/********************************************************/
/* Entry point											*/
/********************************************************/
int WINAPI WinMain (HINSTANCE hinst, HINSTANCE hpinst, LPSTR args, int nShowCmd) {
	HWND hwnd;
	WNDCLASSEX wc;
	MSG msg;
	RECT rc;

/********************************************************/
/* Register window. Exit program if unsuccessfull.		*/
/********************************************************/
	ZeroMemory (&wc, sizeof (WNDCLASSEX));

	wc.cbSize = sizeof (WNDCLASSEX);
	wc.hInstance = hinst;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = class_name;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx (&wc)) {
		MessageBox (NULL, L"Cannot register window! Closing.", L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}

/********************************************************/
/* Create window. Exit program if unsuccessfull			*/
/********************************************************/
	hwnd = GetDesktopWindow ();
	GetWindowRect (hwnd, &rc);
	hwnd = CreateWindowEx (NULL, class_name, L"Snake", 
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, rc.right / 3, rc.bottom / 3, MAP_WIDTH + (BODY_SIZE * 0.5), 
							MAP_HEIGHT + (BODY_SIZE * 5), NULL, NULL, hinst, NULL);

	if (hwnd == NULL) {
		MessageBox (NULL, L"Cannot create window! Closing.", L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	ShowWindow (hwnd, nShowCmd);

	while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	while (true) {
		if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}

/********************************************************/
/* Main Snake logic										*/
/* 1. Check for keyboard input.							*/
/* 2. Check if the head collided with the border		*/
/*		and or itself.									*/
/* 3. Check if the head is in the same position			*/
/*		as the food. If so, add a new body segment		*/
/*		and add the score.								*/
/* 4. Move the head towards the respective direction.	*/
/* 5. Update the screen.								*/
/********************************************************/
		if (start) {
			key_input ();
			if (headx < 0 || headx >= MAP_WIDTH || heady < 0 || heady >= MAP_HEIGHT || !sn->isCoordAvailable (headx, heady)) {
				MessageBox (NULL, L"Game over! :(", L"Game Over", MB_ICONINFORMATION | MB_OK);
				start = false;
			}

			if (headx == foodx && heady == foody) {
				create_food ();
				score += BODY_SIZE;
				sn->addHead (headx, heady);
			}
			else {
				sn->moveOnwards (headx, heady);
			}

			switch (sn->getDirection ()) {
				case UP:
					heady -= BODY_SIZE;
					break;
				case DOWN:
					heady += BODY_SIZE;
					break;
				case LEFT:
					headx -= BODY_SIZE;
					break;
				case RIGHT: 
					headx += BODY_SIZE;
					break;
			}

			InvalidateRect (hwnd, NULL, TRUE);
			UpdateWindow (hwnd);

			Sleep (100);
		}
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wparm, LPARAM lparm) {
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hbr;
	HFONT hf;
	char buffer [10];
	char *buff1 = "Press the enter key to start";
	char *buff2 = "Press the q key to stop the game";
	char *buff3 = "Press the x key to quit";
	LPCWSTR enter = L"Press the enter key to start";
	LPCWSTR stop1 = L"Press the q key to stop the game";
	LPCWSTR quit = L"Press the x key to quit";
	position *pos;

	switch (msg) {
		case WM_PAINT:
			if (start) {
				pos = sn->getPosition ();
				hdc = BeginPaint (hwnd, &ps);

/********************************************************/
/* Draw food using the foodx and foody variables		*/
/********************************************************/
				hbr = CreateSolidBrush (RGB (255, 0, 0));
				SelectObject (hdc, hbr);
				RoundRect (hdc, foodx, foody, foodx + BODY_SIZE, foody + BODY_SIZE, 20, 20);

/********************************************************/
/* Draw the snake using the coords in the queue			*/
/********************************************************/
				hbr = CreateSolidBrush (RGB (0, 255, 0));
				SelectObject (hdc, hbr);
				while (pos != NULL) {
					RoundRect (hdc, pos->xpos, pos->ypos, pos->xpos + BODY_SIZE, pos->ypos + BODY_SIZE, 20, 20);
					pos = pos->next;
				}

				MoveToEx (hdc, 0, 200, NULL);
				LineTo (hdc, MAP_WIDTH + (BODY_SIZE * 1), 200);

/********************************************************/
/* Draw the score										*/
/********************************************************/
				hf = CreateFont(15, 7, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
									OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
									DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
				SelectObject (hdc, hf);
				SetBkMode (hdc, TRANSPARENT);
				TextOut (hdc, 10, 200, L"Score: ", 7);
				sprintf_s (buffer, 10, "%d", score);
				TextOutA (hdc, 50, 200, buffer, strlen (buffer));

				DeleteObject (hf);
				DeleteObject (hbr);
				DeleteDC (hdc);
			}
			else {

/********************************************************/
/* Main menu											*/
/********************************************************/
				hdc = BeginPaint (hwnd, &ps);
				hf = CreateFont(15, 7, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
									OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
									DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
				SelectObject (hdc, hf);
				SetBkMode (hdc, TRANSPARENT);
				TextOut (hdc, 20, 90, enter, strlen (buff1));
				TextOut (hdc, 0, 105, stop1, strlen (buff2));
				TextOut (hdc, 30, 120, quit, strlen (buff3));
				DeleteObject (hf);
				EndPaint (hwnd, &ps);
			}
			return 0;
/********************************************************/
/* Keyboard input handler								*/
/* 0x0D = carriage return/enter							*/
/* 0x71 = q												*/
/* 0x51 = Q												*/
/* 0x78 = x												*/
/* 0x58 = X												*/
/********************************************************/
		case WM_CHAR:
			switch (wparm) {
				case 0x0D:
					if (!start) {
						start = true;
						init ();
						break;
					}
					break;
				case 0x71:
				case 0x51:
					if (start) {
						start = false;
						InvalidateRect (hwnd, NULL, TRUE);
						UpdateWindow (hwnd);
					}
					break;
				case 0x78:
				case 0x58:
					DestroyWindow (hwnd);
					break;
				case VK_DOWN:
					MessageBox (NULL, L"Game over!", L"Game Over", MB_ICONINFORMATION | MB_OK);
					break;
			}
			return 0;
		case WM_KEYDOWN:
			return 0;
		case WM_CLOSE:
			DestroyWindow (hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage (WM_QUIT);
			break;
		default: 
			return DefWindowProc (hwnd, msg, wparm, lparm);
	}
	return 0;
}

void init () {
	headx = 0;
	heady = 0;
	sn = new Snake (headx, heady, INIT_LEN, BODY_SIZE, RIGHT);
	headx += (INIT_LEN * BODY_SIZE);
	srand ((unsigned) time (0));
	create_food ();
	score = 0;
}

void create_food () {
	do {
		foodx = rand () % ((MAP_WIDTH / BODY_SIZE) - 1);
		foody = rand () % ((MAP_HEIGHT / BODY_SIZE) - 1);
		foodx *= BODY_SIZE;
		foody *= BODY_SIZE;
	} while (!sn->isCoordAvailable (foodx, foody));
}

/********************************************************/
/* Game input handler									*/
/********************************************************/
void key_input () {
	if (GetAsyncKeyState (VK_UP) & 0x8000) {
		if (sn->getDirection () != DOWN) {
			sn->setDirection (UP);
		}
	}
	else if (GetAsyncKeyState (VK_DOWN) & 0x8000) {
		if (sn->getDirection () != UP) {
			sn->setDirection (DOWN);
		}
	}
	else if (GetAsyncKeyState (VK_LEFT) & 0x8000) {
		if (sn->getDirection () != RIGHT) {
			sn->setDirection (LEFT);
		}
	}
	else if (GetAsyncKeyState (VK_RIGHT) & 0x8000) {
		if (sn->getDirection () != LEFT) {
			sn->setDirection (RIGHT);
		}
	}
}