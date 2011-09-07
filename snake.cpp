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

/********************************************************/
/* Global variables										*/
/********************************************************/
LPCWSTR class_name = L"SnakeClass";
Snake* sn;
position head, food;
unsigned short score;
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

	ZeroMemory (&wc, sizeof (WNDCLASSEX));

	wc.cbSize = sizeof (WNDCLASSEX);
	wc.hInstance = hinst;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = class_name;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	// Register window. Exit if unsucessfull.
	if (!RegisterClassEx (&wc)) {
		MessageBox (NULL, L"Cannot register window! Closing.", L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	hwnd = GetDesktopWindow ();
	GetWindowRect (hwnd, &rc);
	hwnd = CreateWindowEx (NULL, class_name, L"Snake", 
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, rc.right / 3, rc.bottom / 3, MAP_WIDTH + (BODY_SIZE * 0.5), 
							MAP_HEIGHT + (BODY_SIZE * 5), NULL, NULL, hinst, NULL);

	// Create window. Exit if unsuccessfull.
	if (hwnd == NULL) {
		MessageBox (NULL, L"Cannot create window! Closing.", L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	ShowWindow (hwnd, nShowCmd);

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
	sn = new Snake ();

	while (true) {
		if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}

		if (start) {
			key_input ();
			if (head.xpos < 0 || head.xpos >= MAP_WIDTH || head.ypos < 0 || head.ypos >= MAP_HEIGHT || !sn->isCoordAvailable (head)) {
				MessageBox (NULL, L"Game over! :(", L"Game Over", MB_ICONINFORMATION | MB_OK);
				start = false;
			}

			if (head == food) {
				create_food ();
				score += BODY_SIZE;
				sn->addHead (head);
			}
			else {
				sn->moveOnwards (head);
			}

			switch (sn->getDirection ()) {
				case UP:
					head.ypos -= BODY_SIZE;
					break;
				case DOWN:
					head.ypos += BODY_SIZE;
					break;
				case LEFT:
					head.xpos -= BODY_SIZE;
					break;
				case RIGHT: 
					head.xpos += BODY_SIZE;
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

	switch (msg) {
		case WM_PAINT:
			if (start) {
				hdc = BeginPaint (hwnd, &ps);
				
				// Draw the food using the xpos and ypos of the food variable.
				hbr = CreateSolidBrush (RGB (255, 0, 0));
				SelectObject (hdc, hbr);
				RoundRect (hdc, food.xpos, food.ypos, food.xpos + BODY_SIZE, food.ypos + BODY_SIZE, 20, 20);

				// Draw the snake
				hbr = CreateSolidBrush (RGB (0, 255, 0));
				SelectObject (hdc, hbr);
				
				list <position> pos = sn->getPosition ();
				list <position>::iterator curr;

				for (curr = pos.begin (); curr != pos.end (); curr++)
					RoundRect (hdc, curr->xpos, curr->ypos, curr->xpos + BODY_SIZE, curr->ypos + BODY_SIZE, 20, 20);

				MoveToEx (hdc, 0, 200, NULL);
				LineTo (hdc, MAP_WIDTH + (BODY_SIZE * 1), 200);

				// Output the score
				hf = CreateFont(15, 7, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
									OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
									DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
				SelectObject (hdc, hf);
				SetBkMode (hdc, TRANSPARENT);
				TextOut (hdc, 10, 200, L"Score: ", 7);
				char buff [3];
				wchar_t buf [3];
				_itoa (score, buff, 10);
				mbstowcs (buf, buff, 3);
				TextOut (hdc, 50, 200, buf, wcslen (buf));

				DeleteObject (hf);
				DeleteObject (hbr);
				DeleteDC (hdc);
			}
			else {
				// Draw the main menu.
				hdc = BeginPaint (hwnd, &ps);
				hf = CreateFont(15, 7, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
									OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
									DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
				SelectObject (hdc, hf);
				SetBkMode (hdc, TRANSPARENT);
				LPCWSTR enter = L"Press the enter key to start";
				LPCWSTR stop1 = L"Press the q key to stop the game";
				LPCWSTR quit = L"Press the x key to quit";
				TextOut (hdc, 20, 90, enter, wcslen (enter));
				TextOut (hdc, 0, 105, stop1, wcslen (stop1));
				TextOut (hdc, 30, 120, quit, wcslen (quit));
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
	head.xpos = 0;
	head.ypos = 0;
	sn->Init (head, INIT_LEN, BODY_SIZE, RIGHT);
	head.xpos += (INIT_LEN * BODY_SIZE);
	create_food ();
	score = 0;
}

void create_food () {
	srand ((unsigned) time (0));
	do {
		food.xpos = (rand () % ((MAP_WIDTH / BODY_SIZE) - 1)) * BODY_SIZE;
		food.ypos = (rand () % ((MAP_HEIGHT / BODY_SIZE) - 1)) * BODY_SIZE;
	} while (!sn->isCoordAvailable (food));
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