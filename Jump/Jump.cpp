//Jump.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Jump.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
ULONG_PTR pGdiToken;
RECT rect;

HGLOBAL hMemory = 0;
UCHAR* pDataBuffer = NULL;
DWORD nBufferSize = 0;
DWORD nDataSize = 0;
HANDLE hWrite, hRead;


INT ClientX = 540;
INT ClientY = 960;
INT BaseX = 1080;
INT BaseY = 1920;
INT Delay = 300;
FLOAT ScaleX = 2.0f;
FLOAT ScaleY = 2.0f;
FLOAT Rate = 1.4f;
bool AutoReload = false;

bool isFirst = true;

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&pGdiToken, &gdiplusStartupInput, NULL);//初始化GDI+

	parsexml(".\\Data\\config.xml");
	rect.bottom = ClientY;
	rect.top = 0;
	rect.right = ClientX;
	rect.left = 0;
														   // 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_JUMP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JUMP));

	MSG msg;

	// 主消息循环: 
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(pGdiToken);//关闭GDI+
	if (hMemory) GlobalFree(hMemory);

	return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JUMP));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_JUMP);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP | WS_CAPTION | WS_SYSMENU,
		500, 0, ClientX, ClientY, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}
	RECT w,c;
	GetWindowRect(hWnd, &w);
	GetClientRect(hWnd, &c);
	
	SetWindowPos(	hWnd, NULL, 0, 0, 
					((w.right - w.left) - (c.right - c.left)) + ClientX, 
					((w.bottom - w.top) - (c.bottom - c.top)) + ClientY, 
					SWP_NOMOVE
				);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool isGet = false;
	static int width;
	static int height;
	static int x1;
	static int y1;
	static int x2;
	static int y2;
	static bool p1 = false;
	static bool p2 = false;
	static int d = 0;
	static TCHAR s[128];

	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		x1 = LOWORD(lParam);
		y1 = HIWORD(lParam);
		p1 = true;
		InvalidateRect(hWnd, &rect, false);
		UpdateWindow(hWnd);
		if (AutoReload && p1 && p2)
		{
			PostMessage(hWnd, WM_MBUTTONDOWN, 0, 0);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		x2 = LOWORD(lParam);
		y2 = HIWORD(lParam);
		p2 = true;
		InvalidateRect(hWnd, &rect, false);
		UpdateWindow(hWnd);
		if (AutoReload && p1 && p2)
		{
			PostMessage(hWnd, WM_MBUTTONDOWN, 0, 0);
		}
	}
	break;
	case WM_MBUTTONDOWN:
	{
		if (p1 && p2)
		{
			d = sqrt(pow((x1 - x2) * ScaleX, 2) + pow((y1 - y2) * ScaleY, 2)) * Rate;
			_stprintf_s(s, _T(" shell input swipe 100 100 100 100 %d"), d);
			system_hide(_T(".\\Data\\adb.exe"), s);
			Sleep(d*1.2f+Delay);
		}
		p1 = false;
		p2 = false;
		_stprintf_s(s, _T(" shell screencap -p"));
		system_hide(_T(".\\Data\\adb.exe"), s);
		InvalidateRect(hWnd, &rect, false);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		if (hMemory)
		{
			IStream *pStream = NULL;
			process0d0d0a();
			CreateStreamOnHGlobal(hMemory, false, &pStream);

			Bitmap image(pStream);
			if (image.GetLastStatus() != Status::Ok)
			{
				pStream->Release();
				MessageBox(hWnd, _T("加载图片失败!"), _T("提示"), MB_OK);
				return -1;
			}

			ScaleX = (FLOAT)BaseX / ClientX;
			ScaleY = (FLOAT)BaseX / ClientY;
			//绘图  
			Graphics graphics(hdc);
			graphics.DrawImage(&image, 0, 0, ClientX, ClientY);

			if (p1)
			{
				Pen newPen(Color(255, 0, 0), 1);//画笔，最后一个参数，画笔大小
				graphics.DrawLine(&newPen, x1 - 10, y1, x1 + 10, y1);
				graphics.DrawLine(&newPen, x1, y1 - 10, x1, y1 + 10);
			}
			if (p2)
			{
				Pen newPen(Color(0, 255, 0), 1);//画笔，最后一个参数，画笔大小
				graphics.DrawLine(&newPen, x2 - 10, y2, x2 + 10, y2);
				graphics.DrawLine(&newPen, x2, y2 - 10, x2, y2 + 10);
			}

			TCHAR s[32];
			if (p1 && p2)
			{
				_stprintf_s(s, _T("Jump helper + +"));
			}
			else if (p1)
			{
				_stprintf_s(s, _T("Jump helper + -"));
			}
			else if (p2)
			{
				_stprintf_s(s, _T("Jump helper - +"));
			}
			else
			{
				_stprintf_s(s, _T("Jump helper - -"));
			}
			SetWindowText(hWnd, s);
			pStream->Release();
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

bool initPipe(PHANDLE hReadPipe, PHANDLE hWritePipe, DWORD nSize)
{
	SECURITY_ATTRIBUTES   sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(hReadPipe, hWritePipe, &sa, nSize))
	{
		return   FALSE;
	}
	return   TRUE;
}

BOOL system_hide(const TCHAR* _appName, TCHAR* _CommandLine, BOOL _needWait)
{
	STARTUPINFO   si;
	PROCESS_INFORMATION   pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	initPipe(&hRead, &hWrite, 0);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	//si.hStdInput = hRead;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//关键步骤，CreateProcess函数参数意义请查阅MSDN     
	if (!CreateProcess(_appName, _CommandLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		return   FALSE;
	}
	CloseHandle(hWrite);   //关键，不然readfile将阻塞

	if (!hMemory)
	{
		hMemory = GlobalAlloc(GHND, 1048576);
		nBufferSize = 1048576;
		nDataSize = 0;
	}

	DWORD nReadTotal = 0;
	while (true)
	{
		DWORD nReadSize = 0;

		if (nReadTotal + 4096 > nBufferSize)
		{
			hMemory = GlobalReAlloc(hMemory, nBufferSize += 1048576, GMEM_ZEROINIT);
		}

		pDataBuffer = (UCHAR*)GlobalLock(hMemory);
		if (ReadFile(hRead, pDataBuffer + nReadTotal, 4096, &nReadSize, NULL) == NULL)
		{
			GlobalUnlock(hMemory);
			break;
		}
		nReadTotal += nReadSize;
		GlobalUnlock(hMemory);
	}
	nDataSize = nReadTotal;

	if (_needWait)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
	}

	CloseHandle(hRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return   TRUE;
}

void parsexml(const CHAR* _file)
{
	tinyxml2::XMLDocument doc;
	int ret;
	ret = doc.LoadFile(_file);
	if (ret != 0)
		return;

	tinyxml2::XMLElement* root = doc.RootElement();
	tinyxml2::XMLElement* node = root->FirstChildElement("ClientX");
	if (node) tinyxml2::XMLUtil::ToInt(node->GetText(), &ClientX);
	node = root->FirstChildElement("ClientY");
	if (node) tinyxml2::XMLUtil::ToInt(node->GetText(), &ClientY);
	node = root->FirstChildElement("BsaeX");
	if (node) tinyxml2::XMLUtil::ToInt(node->GetText(), &BaseX);
	node = root->FirstChildElement("BaseY");
	if (node) tinyxml2::XMLUtil::ToInt(node->GetText(), &BaseY);
	node = root->FirstChildElement("Rate");
	if (node) tinyxml2::XMLUtil::ToFloat(node->GetText(), &Rate);
	node = root->FirstChildElement("Delay");
	if (node) tinyxml2::XMLUtil::ToInt(node->GetText(), &Delay);
	node = root->FirstChildElement("AutoReload");
	if (node) tinyxml2::XMLUtil::ToBool(node->GetText(), &AutoReload);
}

void process0d0d0a(void)
{
	UCHAR* temp = (UCHAR*)malloc(nDataSize);

	pDataBuffer = (UCHAR*)GlobalLock(hMemory);

	DWORD idx = 0;
	DWORD idxFirst0D = 0;
	DWORD idxFirst0A = 0;
	BOOL is0D = false;
	for (DWORD i = 0; i < nDataSize; i++)
	{
		UCHAR b = pDataBuffer[i];
		if (b == 0x0d && idxFirst0D == 0)
		{
			idxFirst0D = i;
			is0D = true;
		}
		if (b == 0x0a && idxFirst0A == 0)
		{
			idxFirst0A = i;
		}
		if (i > 2 && b == 0x0a && is0D)
		{
			idx = idx - (idxFirst0A - idxFirst0D - 1);

			temp[idx] = b;
			idx++;
		}
		else
		{
			temp[idx] = b;
			idx++;
		}
		if (b == 0x0d)
			is0D = true;
		else
			is0D = false;
	}

	memcpy(pDataBuffer, temp, idx);
	nDataSize = idx;

	GlobalUnlock(hMemory);


	free(temp);

}