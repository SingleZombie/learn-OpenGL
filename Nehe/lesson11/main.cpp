#include <Windows.h>
#include <gl\glew.h>
#include <gl\glut.h>
#include <iostream>
#include <ctime>
#include "MyVertex.h"
#include "Sector.h"
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

HGLRC hRC = NULL;
HDC hDC = NULL;
HWND hwnd = NULL;
HINSTANCE hInstance;

bool lastKeys[256];
bool keys[256];
bool active = true; // ��С����
bool fullscreen = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

UINT loadTexture(const char* path, UINT style = 0);
void processInput();

template<typename T>
T clamp(const T& x, const T& lob, const T& upb)
{
	return max(min(x, upb), lob);
}

template<typename T>
T loopClamp(T x, const T& lob, const T& upb)
{
	assert(upb + 1e-6 > lob);
	
	T len = upb - lob;
	while (x > upb + 1e-6)
	{
		x -= len;
	}
	while (x + 1e-6 < lob)
	{
		x += len;
	}
	return x;
}

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
const int MAXN = 45;
glm::vec3 points[MAXN][MAXN];


float xrot, yrot, zrot;
glm::vec3 position;
glm::vec3 head;
float speed;

Sector sector;
UINT glassTexture;
UINT containerTexture;

bool initGL()
{
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	/*glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);*/

	srand(time(0));

	glassTexture = loadTexture("glass.png");
	containerTexture = loadTexture("container2.png");
	sector = Sector("vertexData.txt");
	position = { 0, 0, 10 };
	speed = 0.1f;
	head = { 0, 0, -1 };

	for (int i = 0; i < MAXN; i++)
	{
		for (int j = 0; j < MAXN; j++)
		{
			float x = i / 5.0f - 4.5f;
			float y = j / 5.0f - 4.5f;
			points[i][j].x = x;
			points[i][j].y = y;
			points[i][j].z = sin(glm::radians(i * 8.0f));
		}
	}

	return true;
}

const float FRAME_PER_SECOND = 60;
float lastTime;
float deltaTime;

bool drawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	float currentTime;
	do
	{
		currentTime = GetTickCount();
		deltaTime = currentTime - lastTime;
	} while (deltaTime < 1000.0f / FRAME_PER_SECOND);
	lastTime = currentTime;

	glRotatef(yrot, 0, 1.0f, 0);

	glTranslatef(-position.x, -position.y, -position.z);
	glBindTexture(GL_TEXTURE_2D, glassTexture);
	//sector.draw();

	glBindTexture(GL_TEXTURE_2D, containerTexture);

	glBegin(GL_QUADS);
	for (int i = 0; i < MAXN - 1; i++)
	{
		for (int j = 0; j < MAXN - 1; j++)
		{
			float x1 = i / 44.0f, x2 = (i + 1) / 44.0f;
			float y1 = j / 44.0f, y2 = (j + 1) / 44.0f;

			glTexCoord2f(x1, y1);
			glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);

			glTexCoord2f(x1, y2);
			glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);

			glTexCoord2f(x2, y2);
			glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);

			glTexCoord2f(x2, y1);
			glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
		}
	}
	glEnd();

	for (int j = 0; j < MAXN; j++)
	{
		float tmp = points[0][j].z;
		for (int i = 0; i < MAXN - 1; i++)
		{
			points[i][j].z = points[i + 1][j].z;
		}
		points[MAXN - 1][j].z = tmp;
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
	if (keys['D'] || keys['d'])
	{
		yrot += 1.5f;
	}
	if (keys['A'] || keys['A'])
	{
		yrot -= 1.5f;
	}
	yrot = loopClamp(yrot, 0.0f, 360.0f);
	float ra = glm::radians(yrot - 180);
	head = { -sin(ra), 0, cos(ra)};
	if (keys['W'] || keys['w'])
	{
		position += speed * head;
	}
	if (keys['S'] || keys['s'])
	{
		position -= speed * head;
	}
}

void killGLWindow()
{
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
		default :
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