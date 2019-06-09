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
bool active = true; // ×îÐ¡»¯£¿
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
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

// clockwise
UINT cubePosIndice[] =
{
	0, 1, 2, 3,
	4, 5, 6, 7,
	1, 0, 7, 6,
	3, 2, 5, 4,
	0, 3, 4, 7,
	2, 1, 6, 5
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

float boxColor[] = {
	1.0f, 0.0f, 0.0f,
	1.0f, 0.5f, 0.0f,
	1.0f, 1.0f, 0.0f, 
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f
};
float lightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };


float xrot, yrot, zrot;
MyVertex::PropertyArray cube;

glm::vec3 position;
glm::vec3 head;
float speed;

Sector sector;
UINT glassTexture;
UINT containerTexture;

UINT cubeList;

void buildLists()
{
	cubeList = glGenLists(1);
	glNewList(cubeList, GL_COMPILE);

	cube.push_back(MyVertex::VertexProperty(cubePos, cubePosIndice, 24));
	cube.push_back(MyVertex::VertexProperty(vertexColor, cubeColorIndice, 24));
	cube.push_back(MyVertex::VertexProperty(texturePos, cubeTexCoord, 24, 2));

	glBindTexture(GL_TEXTURE_2D, containerTexture);
	glBegin(GL_QUADS);
	MyVertex::drawVertex(cube, MyVertex::POSITION_BIT | MyVertex::TEX_COORD2_BIT);
	glEnd();

	glEndList();
}

bool initGL()
{
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);

	/*glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);*/

	/*glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);*/

	srand(time(0));

	glassTexture = loadTexture("glass.png");
	containerTexture = loadTexture("container2.png");
	sector = Sector("vertexData.txt");
	buildLists();
	position = { 0, 0, 30 };
	speed = 0.1f;
	head = { 0, 0, -1 };

	cube.push_back(MyVertex::VertexProperty(cubePos, cubePosIndice, 24));
	cube.push_back(MyVertex::VertexProperty(vertexColor, cubeColorIndice, 24));
	cube.push_back(MyVertex::VertexProperty(texturePos, cubeTexCoord, 24, 2));

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


	for (int y = 1; y <= 5; y++)
	{
		for (int x = 0; x < y; x++)
		{
			glLoadIdentity();
			glRotatef(yrot, 0, 1.0f, 0);
			glTranslatef(-position.x, -position.y, -position.z);
			glTranslatef(x * 2, y * 2, 0);
			glColor3f(boxColor[(y - 1) * 3], boxColor[(y - 1) * 3 + 1], boxColor[(y - 1) * 3 + 2]);
			glCallList(cubeList);
		}
	}

	

	glBindTexture(GL_TEXTURE_2D, containerTexture);

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