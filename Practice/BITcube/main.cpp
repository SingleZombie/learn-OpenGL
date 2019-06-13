#include <Windows.h>
#include <gl\glew.h>
#include <gl\glut.h>
#include <iostream>
#include "MyVertex.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

HGLRC hRC = NULL;
HDC hDC = NULL;
HWND hwnd = NULL;
HINSTANCE hInstance;

bool lastKeys[256];
bool keys[256];
bool active = true; // ×îÐ¡»¯£¿
bool fullscreen = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void processInput();

UINT loadTexture(const char* path);

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

float vertexColor[] =
{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
};

float cubePos[] =
{
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
};
float texturePos[] =
{
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f
};

// clockwise
UINT cubePosIndice[] =
{
	0, 1, 2, 3, // top - I
	4, 5, 6, 7, // bottom -I
	1, 0, 7, 6, // right -T
	3, 2, 5, 4, // left -T
	0, 3, 4, 7, // front B
	2, 1, 6, 5  // back - B
};
UINT cubeColorIndice[] =
{
	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 1, 1,
	1, 1, 1, 1,
	2, 2, 2, 2,
	2, 2, 2, 2
};
UINT cubeTexCoord[] =
{
	1, 2, 3, 0,
	1, 2, 3, 0,
	2, 3, 0, 1,
	2, 3, 0, 1,
	0, 1, 2, 3,
	0, 1, 2, 3
};


UINT listBase;
GLYPHMETRICSFLOAT gmf[256];
UINT glass;
UINT fontTexture;

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
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		"Times New Roman");

	oldFont = (HFONT)SelectObject(hDC, font);
	//wglUseFontBitmaps(hDC, 32, 96, listBase);
	wglUseFontOutlines(
		hDC,
		0,
		255,
		listBase,
		0.0f,
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

	glPushAttrib(GL_LIST_BIT);
	glListBase(listBase);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}


float xrot = 20, yrot = -45, zrot;
UINT containerTexture;

MyVertex::PropertyArray cube;
UINT textureB, textureI, textureT;

bool initGL()
{
	textureB = loadTexture("BIT_B.png");
	textureI = loadTexture("BIT_I.png");
	textureT = loadTexture("BIT_T.png");

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	cube.push_back(MyVertex::VertexProperty(cubePos, cubePosIndice, 24));
	cube.push_back(MyVertex::VertexProperty(vertexColor, cubeColorIndice, 24));
	cube.push_back(MyVertex::VertexProperty(texturePos, cubeTexCoord, 24, 2));

	buildFont();

	return true;
}

bool drawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslated(-13, 10, -30);
	myPrint("Press a, d, w, s, q, e to rotate");

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);
	
	int textureArr[] = {
		textureI,
		textureI,
		textureT,
		textureT,
		textureB,
		textureB
	};

	for (int i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textureArr[i]);
		glBegin(GL_QUADS);
		MyVertex::drawVertex(cube, MyVertex::POSITION_BIT | MyVertex::TEX_COORD2_BIT, i * 4, 4);
		glEnd();
	}
	
	return true;
}

void processInput()
{
	for (UINT i = 0; i < 256; i++)
	{
		if (keys[i] && !lastKeys[i])
		{
			lastKeys[i] = true;

			switch (i)
			{
			}

		}
		else if (!keys[i] && lastKeys[i])
		{
			lastKeys[i] = false;
			switch (i)
			{
			
			}
		}
	}
	if (keys['A'] || keys['a'])
	{
		yrot -= 0.1f;
	}
	if (keys['D'] || keys['d'])
	{
		yrot += 0.1f;
	}
	if (keys['W'] || keys['w'])
	{
		zrot -= 0.1f;
	}
	if (keys['S'] || keys['s'])
	{
		zrot += 0.1f;
	}
	if (keys['Q'] || keys['q'])
	{
		xrot -= 0.1f;
	}
	if (keys['E'] || keys['e'])
	{
		xrot += 0.1f;
	}
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
	freopen("test.out", "w", stdout);

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
					processInput();
				}
			}
		}
	}

	killGLWindow();
	return msg.wParam;
}

UINT loadTexture(const char* path)
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
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}