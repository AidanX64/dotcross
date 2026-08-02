// redcalebdot.c
// Tiny red crosshair overlay with a black outline.
// Build:
//     cl /O2 redcalebdot.c user32.lib gdi32.lib

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define CROSS_SIZE 5
#define OUTLINE_THICKNESS 1

static const COLORREF TRANSPARENT_COLOR = RGB(255, 0, 255);

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
  switch (msg) {
  case WM_ERASEBKGND:
    return 1;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc;
    GetClientRect(hwnd, &rc);

    const int centerX = rc.right / 2;
    const int centerY = rc.bottom / 2;
    const int arm = CROSS_SIZE / 2;

    HBRUSH brush = CreateSolidBrush(TRANSPARENT_COLOR);
    FillRect(hdc, &rc, brush);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(0, 0, 0));
    RECT horizontal = {centerX - arm - OUTLINE_THICKNESS,
                       centerY - OUTLINE_THICKNESS,
                       centerX + arm + OUTLINE_THICKNESS + 1,
                       centerY + OUTLINE_THICKNESS + 1};
    RECT vertical = {centerX - OUTLINE_THICKNESS,
                     centerY - arm - OUTLINE_THICKNESS,
                     centerX + OUTLINE_THICKNESS + 1,
                     centerY + arm + OUTLINE_THICKNESS + 1};
    FillRect(hdc, &horizontal, brush);
    FillRect(hdc, &vertical, brush);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(0, 255, 255));
    SetRect(&horizontal, centerX - arm, centerY, centerX + arm + 1,
            centerY + 1);
    SetRect(&vertical, centerX, centerY - arm, centerX + 1,
            centerY + arm + 1);
    FillRect(hdc, &horizontal, brush);
    FillRect(hdc, &vertical, brush);
    DeleteObject(brush);

    EndPaint(hwnd, &ps);
    return 0;
  }

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }

  return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine,
                   int nShowCmd) {
  (void)hPrev;
  (void)lpCmdLine;
  (void)nShowCmd;

  const char CLASS_NAME[] = "CrosshairOverlay";

  WNDCLASS wc = {0};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInst;
  wc.lpszClassName = CLASS_NAME;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);

  RegisterClass(&wc);

  int screenW = GetSystemMetrics(SM_CXSCREEN);
  int screenH = GetSystemMetrics(SM_CYSCREEN);

  HWND hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
                             CLASS_NAME, "", WS_POPUP, 0, 0, screenW, screenH,
                             NULL, NULL, hInst, NULL);

  if (!hwnd)
    return 1;

  // Magenta becomes completely transparent, leaving black available for the outline.
  SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0, LWA_COLORKEY);

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}
