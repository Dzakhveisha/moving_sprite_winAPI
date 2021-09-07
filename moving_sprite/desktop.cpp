#include <windows.h>
#include <string.h>
#include <tchar.h>

// The main window class name
static TCHAR szWindowClass[] = _T("DesktopAppClass");
// The string that appears in the application's title bar
static TCHAR szTitle[] = _T("Moving sprite Window");

// brushes
const HBRUSH RECT_BRUSH = CreateSolidBrush(RGB(255, 255, 0));
const HBRUSH BACKGROUND_BRUSH = CreateSolidBrush(RGB(175, 238, 238));

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow)
{
    WNDCLASSEX wcex; HWND hWnd; MSG msg;

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

    static int cxClient, cyClient;

    static POINT ptCenter; // centre of object
    static int ellRadius = 20;

    switch (message)
    {    
        case WM_CREATE: 
        {
            //myBmp = PngToBitmap(picture);
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
            Ellipse(hdc,
                ptCenter.x - ellRadius,
                ptCenter.y + ellRadius,
                ptCenter.x + ellRadius,
                ptCenter.y - ellRadius);
            
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



