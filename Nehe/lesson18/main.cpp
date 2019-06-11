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
bool active = true; // 最小化？
bool fullscreen = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

UINT loadTexture(const char* path, UINT style = 0);
void processInput();

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

float vertexArr[] =
{
	0.0f, 1.0f, 0.0f,		// top
	-1.0f, -1.0f, 1.0f,		// left-down
	-1.0f, -1.0f, -1.0f,	// left-up
	1.0f, -1.0f, 1.0f,		// right-down
	1.0f, -1.0f, -1.0f		// right-up
};
UINT vertexIndice[] =
{
	0, 1, 3,
	0, 3, 4,
	0, 4, 2,
	0, 2, 1
};

float vertexColor[] =
{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
};

UINT pyramidColorIndice[]
{
	0, 1, 2,
	0, 2, 1,
	0, 1, 2,
	0, 2, 1,
};

MyVertex::PropertyArray pyramid;

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
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

// clockwise
UINT cubePosIndice[] =
{
	0, 1, 2, 3, // top
	4, 5, 6, 7, // bottom
	1, 0, 7, 6, // right
	3, 2, 5, 4, // left
	0, 3, 4, 7, // front
	2, 1, 6, 5  // back
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
	0, 1, 2, 3,
	0, 1, 2, 3,
	0, 1, 2, 3,
	0, 1, 2, 3,
	0, 1, 2, 3,
	0, 1, 2, 3
};

float cubeNormal[] =
{
	0.0f, 1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, -1.0f
};
UINT cubeNormalIndice[] =
{
	0, 1, 2, 3, 4, 5
};

float xrot, yrot, zrot;
float xSpdR, ySpdR, zSpdR;
float z = -5.0f;
UINT containerTexture;

MyVertex::PropertyArray cube;


float lightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };

UINT fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };
UINT fogFilter;
float fogColor[] = { 0.5f, 0.5f ,0.5f, 1.0f };


UINT texture[3];

GLUquadric* quadratic;
UINT mode;
float p1, p2 = 0.5f , part1, part2;

bool initGL()
{
	for (int i = 0; i < 3; i++)
	{
		texture[i] = loadTexture("container2.png", i);
		if (!texture[i])
		{
			return false;
		}
	}

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glFogi(GL_FOG_MODE, fogMode[fogFilter]); // 设置雾气的模式
	glFogfv(GL_FOG_COLOR, fogColor); // 设置雾的颜色
	glFogf(GL_FOG_DENSITY, 0.35f); // 设置雾的密度
	glHint(GL_FOG_HINT, GL_DONT_CARE); // 设置系统如何计算雾气
	glFogf(GL_FOG_START, 1.0f); // 雾气的开始位置
	glFogf(GL_FOG_END, 5.0f); // 雾气的结束位置
	glEnable(GL_FOG); // 使用雾气

	pyramid.push_back(MyVertex::VertexProperty(vertexArr, vertexIndice, 12));
	pyramid.push_back(MyVertex::VertexProperty(vertexColor, pyramidColorIndice, 12));
	cube.push_back(MyVertex::VertexProperty(cubePos, cubePosIndice, 24));
	cube.push_back(MyVertex::VertexProperty(vertexColor, cubeColorIndice, 24));
	cube.push_back(MyVertex::VertexProperty(texturePos, cubeTexCoord, 24, 2));
	cube.push_back(MyVertex::VertexProperty(cubeNormal, cubeNormalIndice, 6));

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT1);

	quadratic = gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	gluQuadricTexture(quadratic, GL_TRUE);

	return true;
}

bool drawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, z);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	switch (mode)
	{
	case 0:
		glBegin(GL_QUADS);
		for (int i = 0; i < 6; i++)
		{
			MyVertex::drawVertex(cube, MyVertex::NORMAL_BIT, i, 1);
			MyVertex::drawVertex(cube, MyVertex::POSITION_BIT | MyVertex::TEX_COORD2_BIT, i * 4, 4);
		}

		glEnd();
		break;
	case 1:
		glTranslatef(0.0f, 0.0f, -1.5f);
		gluCylinder(quadratic, 1.0f, 1.0f, 3.0f, 32, 32);
		break;
	case 2:
		gluDisk(quadratic, 0.5f, 1.5f, 32, 32);
		break;
	case 3:
		gluSphere(quadratic, 1.3f, 32, 32);
		break;
	case 4:
		glTranslatef(0.0f, 0.0f, -1.5f);
		gluCylinder(quadratic, 1.0f, 0.0f, 3.0f, 32, 32);
		break;
	case 5:
		part1 += p1;
		part2 += p2;
		if (part1>359)
		{
			p1 = 0;
			part1 = 0;
			p2 = 0.1f;
			part2 = 0;
		}
		if (part2>359)
		{
			p1 = 0.1f;
			p2 = 0;
		}
		gluPartialDisk(quadratic, 0.5f, 1.5f, 32, 32, part1, part2 - part1);
		break;
	}
	

	xrot += 0.03f;
	yrot += 0.02f;
	zrot += 0.04f;

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
			case 'G':
			case 'g':
				fogFilter = (fogFilter + 1) % 3;
				glFogi(GL_FOG_MODE, fogMode[fogFilter]);
				break;
			case ' ':
				mode = (mode + 1) % 6;
			}
		}
	}
}

void killGLWindow()
{
	gluDeleteQuadric(quadratic);

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