#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

// string const
static const TCHAR szWindowClass[] = _T("DesktopAppClass");
static const TCHAR szTitle[] = _T("Moving sprite Window");
static WCHAR pictureName[] = L"donut.png";

// brushes
const HBRUSH RECT_BRUSH = CreateSolidBrush(RGB(255, 255, 0));
const HBRUSH BACKGROUND_BRUSH = CreateSolidBrush(RGB(175, 238, 238));

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void drawBmp(HDC hdc, POINT ptCenter, HBITMAP hBitmap);
HBITMAP PngToBitmap(WCHAR* pngFilePath);

WNDCLASSEX wcex;

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow)
{
    HWND hWnd; MSG msg;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc; // function foe msg handling
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = BACKGROUND_BRUSH;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = wcex.hIcon;

    RegisterClassEx(&wcex); // register WNDCLASSEX

    hWnd = CreateWindow(szWindowClass, szTitle,  // window descriptor
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //loop to listen for the messages that Windows sends
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
 }                

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH hbrush = RECT_BRUSH;
    static HBITMAP objectBmp = NULL;

    static bool isImage = false;

    static int cxClient, cyClient;

    static POINT ptCenter; // centre of object
    static int ellRadius = 20;

    switch (message)
    {    
        case WM_CREATE: 
        {
            hbrush = RECT_BRUSH;
            objectBmp = PngToBitmap(pictureName);
            break;
        }
        case WM_SIZE:
        {
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);

            ptCenter.x = cxClient / 2;
            ptCenter.y = cyClient / 2;
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            SelectObject(hdc, hbrush);
            if (isImage) {
                drawBmp(hdc, ptCenter, objectBmp);
            }
            else {
                Ellipse(hdc,
                    ptCenter.x - ellRadius,
                    ptCenter.y + ellRadius,
                    ptCenter.x + ellRadius,
                    ptCenter.y - ellRadius);
            }

            EndPaint(hWnd, &ps);
        }
        break;
   
        case WM_MOUSEMOVE:
        {
            ptCenter.x = LOWORD(lParam);
            ptCenter.y = HIWORD(lParam);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_LEFT: 
                ptCenter.x -= 10;
                break;
            case VK_RIGHT:
                ptCenter.x += 10;
                break;
            case VK_UP:
                ptCenter.y -= 10;
                break;
            case VK_DOWN:
                ptCenter.y += 10;
                break;
            case VK_SPACE:
                isImage == false ? isImage = true : isImage = false;
                break;
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_MOUSEWHEEL: {
            int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
            {
                wheelDelta < 0 ? ptCenter.x -= 10 : ptCenter.x += 10;
            }
            else {
                wheelDelta < 0 ? ptCenter.y -= 10 : ptCenter.y += 10;
            }
            InvalidateRect(hWnd, NULL, TRUE);
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

void drawBmp(HDC hdc, POINT ptCenter, HBITMAP hBitmap) {
    HBITMAP hNewBmp ;

    HDC compDc;
    BITMAP bmp;
    int bmpWidth, bmpHeight;

    compDc = CreateCompatibleDC(hdc);

    hNewBmp = (HBITMAP)SelectObject(compDc, hBitmap);

    if (hNewBmp) {
        SetMapMode(compDc, GetMapMode(hdc));
        GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp);

        bmpWidth = bmp.bmWidth;
        bmpHeight = bmp.bmHeight;

        POINT bmpSize;
        bmpSize.x = bmpWidth;
        bmpSize.y = bmpHeight;

        DPtoLP(hdc, &bmpSize, 1);

        POINT point;
        point.x = 0;
        point.y = 0;

        DPtoLP(compDc, &point, 1);
        BitBlt(hdc, ptCenter.x - bmpWidth/2, ptCenter.y - bmpHeight/2, bmpWidth, bmpHeight, compDc, point.x, point.y, SRCCOPY);
        SelectObject(compDc, hNewBmp);
    }

    DeleteDC(compDc);
}

HBITMAP PngToBitmap(WCHAR* pngFilePath) {
    GdiplusStartupInput gdi;
    ULONG_PTR token;
    GdiplusStartup(&token, &gdi, NULL);
    Color Back = Color(Color::MakeARGB(0, 175, 238, 238));
    HBITMAP convertedBitmap = NULL;
    Bitmap* Bitmap = Bitmap::FromFile(pngFilePath, false);
    if (Bitmap) {
        Bitmap->GetHBITMAP(Back, &convertedBitmap);

        delete Bitmap;
    }
    GdiplusShutdown(token);
    return convertedBitmap;
}





