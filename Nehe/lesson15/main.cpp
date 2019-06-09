#include <Windows.h>
#include <gl\glew.h>
#include <gl\glut.h>
#include <cstdarg>
#include <cmath>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

HGLRC hRC = NULL;
HDC hDC = NULL;
HWND hwnd = NULL;
HINSTANCE hInstance;

bool keys[256];
bool active = true; // ×îÐ¡»¯£¿
bool fullscreen = false;

using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

UINT loadTexture(const char* path, UINT style = 0);

void resizeGLScene(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(
		45.0f,
		float(width) / height,
		0.1f,
		100.0f
	);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

UINT listBase;
GLYPHMETRICSFLOAT gmf[256];

UINT glass;

void buildFont()
{
	HFONT font;
	HFONT oldFont;
	listBase = glGenLists(96);

	font = CreateFont(
		-24,
		0,
		0,
		0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		SYMBOL_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		"Wingdings");

	oldFont = (HFONT)SelectObject(hDC, font);
	//wglUseFontBitmaps(hDC, 32, 96, listBase);
	wglUseFontOutlines(
		hDC,
		0,
		255,
		listBase,
		0.1f,
		0.2f,
		WGL_FONT_POLYGONS,
		gmf);

	SelectObject(hDC, oldFont);
	DeleteObject(font);
}
void deleteFont()
{
	glDeleteLists(listBase, 256);
}

void myPrint(const char* fmt, ...)
{
	char text[256];
	va_list ap;

	if (fmt == NULL)
	{
		return;
	}

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	float len = 0;
	for (UINT i = 0; text[i]; i++)
	{
		len += gmf[text[i]].gmfCellIncX;
	}
	glTranslatef(-len / 2, 0.0f, 0.0f);

	glPushAttrib(GL_LIST_BIT);
	glListBase(listBase);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

bool initGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	buildFont();
	glass = loadTexture("glass.png");
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, glass);

	return true;
}

int cnt;
float rot;

const float FRAME_PER_SECOND = 24;
float lastTime;
float deltaTime;

bool drawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float currentTime;
	do
	{
		currentTime = GetTickCount();
		deltaTime = currentTime - lastTime;
	} while (deltaTime < 1000.0f / FRAME_PER_SECOND);
	lastTime = currentTime;
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -10.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(0.0f, 0.0f);
	myPrint("ABC N");

	return true;
}

void killGLWindow()
{
	deleteFont();
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(true);
	}

	if (hRC)
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			MessageBox(NULL, "release dc & rc fail", "close error", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))
		{
			MessageBox(NULL, "release rc fail", "close error", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;
	}

	if (hDC && !ReleaseDC(hwnd, hDC))
	{
		MessageBox(NULL, "release dc fail", "close error", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;
	}

	if (hwnd && !DestroyWindow(hwnd))
	{
		MessageBox(NULL, "release window fail", "close error", MB_OK | MB_ICONINFORMATION);
		hwnd = NULL;
	}

	if (!UnregisterClass("OpenG", hInstance))
	{
		MessageBox(NULL, "release window class fail", "close error", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;
	}
}

bool createGLWindow(const char* title, int width, int height, int bits, bool fullscreenFlag)
{
	GLuint pixelFormat;
	WNDCLASS wc;
	DWORD dwExStyle;
	DWORD dwStyle;

	RECT windowRect;
	windowRect.left = 0;
	windowRect.right = width;
	windowRect.top = 0;
	windowRect.bottom = height;

	fullscreen = fullscreenFlag;

	hInstance = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "OpenG";

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "register window fail", "error", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if (fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = bits;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL, "fullscreen fail, use window mode?", "Nehe G", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = false;
			}
			else
			{
				MessageBox(NULL, "program will close", "error", MB_OK | MB_ICONSTOP);
				return false;
			}
		}
	}

	if (fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(false);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);

	if (!(hwnd = CreateWindowEx(
		dwExStyle,
		"OpenG",
		title,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
		0, 0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL)))
	{
		killGLWindow();
		MessageBox(NULL, "can't create window", "error", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_TYPE_RGBA,
		bits,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	if (!(hDC = GetDC(hwnd)))
	{
		killGLWindow();
		MessageBox(NULL, "can't create pixel format", "error", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if (!(pixelFormat = ChoosePixelFormat(hDC, &pfd)))
	{
		killGLWindow();
		MessageBox(NULL, "can't set pixel format", "error", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if (!(SetPixelFormat(hDC, pixelFormat, &pfd)))
	{
		killGLWindow();
		MessageBox(NULL, "can't set pixel format2", "error", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if (!(hRC = wglCreateContext(hDC)))
	{
		killGLWindow();
		MessageBox(NULL, "can't create openGL rc", "error", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	if (!wglMakeCurrent(hDC, hRC))
	{
		killGLWindow();
		MessageBox(NULL, "can't activate current openGL rc", "error", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	resizeGLScene(width, height);

	if (!initGL())
	{
		killGLWindow();
		MessageBox(NULL, "init fail", "error", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	return true;
}

LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		if (!HIWORD(wParam))
		{
			active = true;
		}
		else
		{
			active = false;
		}
		return 0;
		break;
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
			break;
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_KEYDOWN:
		keys[wParam] = true;
		return 0;
		break;
	case WM_KEYUP:
		keys[wParam] = false;
		return 0;
		break;
	case WM_SIZE:
		resizeGLScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;
	bool done = false;
	/*
	setFullScreen
	*/

	if (!createGLWindow("Hello World", 800, 600, 16, fullscreen))
	{
		return 0;
	}

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (active)
			{
				if (keys[VK_ESCAPE])
				{
					done = true;
				}
				else
				{
					drawGLScene();
					SwapBuffers(hDC);
				}
			}
		}
	}

	killGLWindow();
	return msg.wParam;
}

UINT loadTexture(const char* path, UINT style)
{
	UINT textureID = 0;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		//glGenerateMipmap(GL_TEXTURE_2D);

		switch (style)
		{
		case 0:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			break;
		case 1:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			break;
		case 2:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, format, GL_UNSIGNED_BYTE, data);
		default:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			break;
		}


		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}