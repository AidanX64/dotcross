// crosshair.c
// Tiny white-dot crosshair overlay.
// Build:
//     cl /O2 crosshair.c user32.lib gdi32.lib

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DOT_SIZE 4

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

    int x = (rc.right - DOT_SIZE) / 2;
    int y = (rc.bottom - DOT_SIZE) / 2;

    HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
    HGDIOBJ oldBrush = SelectObject(hdc, brush);

    Ellipse(hdc, x, y, x + DOT_SIZE, y + DOT_SIZE);

    SelectObject(hdc, oldBrush);
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

  // Black becomes completely transparent.
  SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}
