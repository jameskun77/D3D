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



//global
LPDIRECT3DDEVICE9        g_pd3dDevice = NULL; 
LPDIRECT3DVERTEXBUFFER9  g_pVertexBuffer = NULL; 
LPDIRECT3DVERTEXBUFFER9  g_pVertexUVBufffer = NULL;
LPDIRECT3DTEXTURE9       g_pTexture = NULL;
IDirect3DVertexShader9*  g_pVertexShader = NULL;
ID3DXConstantTable*      g_pConstantTable = NULL;
LPDIRECT3DVERTEXDECLARATION9 g_VertexClear = NULL;

D3DXHANDLE               g_transformViewProjHandle;
D3DXMATRIX				 g_projMatrix;

// Mesh objects
LPD3DXMESH g_teapot = NULL;

HRESULT             Direct3DInit(HWND hwnd);
HRESULT             ObjectsInit();
void                Direct3DRender(HWND hwnd);
void                Direct3DCleanUp();
void			    RenderQuad();
void				SetTransform();

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
	wndClass.lpszClassName = "ForTheDreamOfGameDevelop";

	if (!RegisterClassEx(&wndClass))
		return -1;

	HWND hWnd = CreateWindow("ForTheDreamOfGameDevelop", "D3DRender",
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

	UnregisterClass("ClassName", wndClass.hInstance);
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

	//D3DPRESENT_PARAMETERS�ṹ��  
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
	// ��������
	D3DVERTEXELEMENT9 Decl_Element[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		//{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(g_pd3dDevice->CreateVertexDeclaration(Decl_Element, &g_VertexClear)))
	{
		return E_FAIL;
	}

	//new method
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(D3DXVECTOR3), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &g_pVertexBuffer, 0)))
	{
		return E_FAIL;
	}
	D3DXVECTOR3 *pPos = NULL;
	if (FAILED(g_pVertexBuffer->Lock(0, 0, (void**)&pPos, 0)))
	{
		return E_FAIL;
	}

	pPos[0] = D3DXVECTOR3(-1.5, 1.5, 0);
	pPos[1] = D3DXVECTOR3(-1.5, -1.5, 0);
	pPos[2] = D3DXVECTOR3(1.5, 1.5, 0);
	pPos[3] = D3DXVECTOR3(1.5, -1.5, 0);

	g_pVertexBuffer->Unlock();

	//UV
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(D3DXVECTOR2), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &g_pVertexUVBufffer, 0)))
	{
		return E_FAIL;
	}

	D3DXVECTOR2 *UV_Data = NULL;
	if (FAILED(g_pVertexUVBufffer->Lock(0, 0, (void**)&UV_Data, 0)))
	{
		return E_FAIL;
	}

	UV_Data[0] = D3DXVECTOR2(0.0f, 1.0f);
	UV_Data[1] = D3DXVECTOR2(0.0f, 0.0f);
	UV_Data[2] = D3DXVECTOR2(1.0f, 1.0f);
	UV_Data[3] = D3DXVECTOR2(1.0f, 0.0f);

	g_pVertexUVBufffer->Unlock();

	//load local texture
	if(FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "22.jpg", &g_pTexture)))
	{
		return E_FAIL;
	}

	//create teapot
	if (FAILED(D3DXCreateTeapot(g_pd3dDevice, &g_teapot, NULL)))
	{
		return E_FAIL;
	}

	//compile shader
	ID3DXBuffer* shader = 0;
	ID3DXBuffer* errorBuffer = 0;

	HRESULT hr = D3DXCompileShaderFromFile(
		"transform.txt",
		0,
		0,
		"Main",  // entry point function name
		"vs_1_1",// shader version to compile to
		D3DXSHADER_DEBUG,
		&shader,
		&errorBuffer,
		&g_pConstantTable);

	//get compile message
	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	if (errorBuffer)
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		SAFE_RELEASE(errorBuffer);
	}

	//create shader
	hr = g_pd3dDevice->CreateVertexShader(
		(DWORD*)shader->GetBufferPointer(),
		&g_pVertexShader);

	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	SAFE_RELEASE(shader);

	// Get Handles.
	g_transformViewProjHandle = g_pConstantTable->GetConstantByName(0, "ViewProjMatrix");

	// Set shader constants.
	g_pConstantTable->SetDefaults(g_pd3dDevice);

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);  //��������ã���ʱ��������� �Ͳ�����ʾ
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);  //���رչ��վͲ���ʾ��

	return S_OK;
}

void  SetTransform()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0, 0, 10)
		, &D3DXVECTOR3(0, 0, 0)
		, &D3DXVECTOR3(0, 1, 0));
	//g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 800.0f / 600.0f, 1.0f, 100.0f);
	//g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	g_projMatrix = matWorld * matView * matProj;

	//use vertex shader
	g_pConstantTable->SetMatrix(g_pd3dDevice, g_transformViewProjHandle, &g_projMatrix);
}

void Direct3DRender(HWND hwnd)
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 50, 100), 1.0f, 0);
	if(SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		SetTransform();  
		//g_pd3dDevice->SetTexture(0, g_pTexture);
		g_pd3dDevice->SetVertexShader(g_pVertexShader);
		g_pd3dDevice->SetVertexDeclaration(g_VertexClear);
		g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(D3DXVECTOR3));
		//g_pd3dDevice->SetStreamSource(1, g_pVertexUVBufffer, 0, sizeof(D3DXVECTOR2));
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		g_pd3dDevice->EndScene();
	}
	
	g_pd3dDevice->Present(0, 0, 0, 0);
	
}

void RenderQuad()
{
	// Setup texture
	//g_pd3dDevice->SetTexture(0, g_pRenderTexture);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	//Set stream source 
	//g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	//g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);  //flexible vertex declaration
	//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}


void Direct3DCleanUp()
{
	//�ͷ�COM�ӿڶ���  
	SAFE_RELEASE(g_pVertexBuffer)
	SAFE_RELEASE(g_pd3dDevice)
}
