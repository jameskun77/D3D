#include <windows.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "CommonUtil.h"
#include <time.h>

#define WIDTH 640
#define HEIGHT 480

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")

struct CUSTOMVERTEX
{
	float x, y, z, rhw;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW)

//global
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL; 

HRESULT             Direct3DInit(HWND hwnd);
HRESULT             ObjectsInit();
void                Direct3DRender(HWND hwnd);
void                Direct3DCleanUp();

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Direct3DCleanUp();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = MsgProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInst;
	wndClass.hIcon = NULL;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";

	if (!RegisterClassEx(&wndClass))
		return -1;

	HWND hWnd = CreateWindow(L"ForTheDreamOfGameDevelop", L"D3DRender",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH,
		HEIGHT, NULL, NULL, wndClass.hInstance, NULL);

	if (!FAILED(Direct3DInit(hWnd)))
	{
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Direct3DRender(hWnd);
			}
		}
	}

	UnregisterClass(L"ClassName", wndClass.hInstance);
	return 0;
}

HRESULT	Direct3DInit(HWND hwnd)
{
	LPDIRECT3D9  pD3D = NULL; 
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return E_FAIL;
	}
		 
	D3DCAPS9 caps; int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return E_FAIL;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; 

	//D3DPRESENT_PARAMETERS结构体  
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WIDTH;
	d3dpp.BackBufferHeight = HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 2;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}
	
	if (!(S_OK == ObjectsInit()))
	{
		return E_FAIL;
	}

	SAFE_RELEASE(pD3D)

	return S_OK;
}

HRESULT	ObjectsInit()
{
	//create vertex buffer
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}
	
	CUSTOMVERTEX vertices[] =
	{
		//D3DFVF_XYZRHW 表示已经映射到窗口上的值
		//window窗口的坐标系统如下
		// (0,0)
		//  ------------------------------------->x
		//  |
		//  | 
		//  |
		//  |y

		//顺时针才显示
		{    0.0f,   0.0f, 0.0f, 1.0f }, 
		{    0.0f, 150.0f, 0.0f, 1.0f },
		{  150.0f, 0.0f, 0.0f, 1.0f },
		{ 150.0f, 150.0f, 0.0f, 1.0f },
		

	};

	//填充顶点缓冲区  
	VOID* pVertices;
	if (FAILED(g_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, vertices, sizeof(vertices));
	g_pVertexBuffer->Unlock();
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);  //如果不设置，逆时针的三角形 就不会显示
	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	return S_OK;
}

void Direct3DRender(HWND hwnd)
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 100, 0), 1.0f, 0);

	g_pd3dDevice->BeginScene();  

	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	g_pd3dDevice->EndScene();                       
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL); 
}

void Direct3DCleanUp()
{
	//释放COM接口对象  
	SAFE_RELEASE(g_pVertexBuffer)
	SAFE_RELEASE(g_pd3dDevice)
}
