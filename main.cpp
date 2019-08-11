#include <iostream> //XXXXXXXXXXX

#include <fcntl.h> //for console
#include <windows.h>
#include <winsock2.h>
#include <gl/gl.h>
#include "kaufen.h"
#include "resource.h"

#define WM_WSAASYNC (WM_USER +5)

bool g_keys[256],g_mouse_but[4];
int  g_mouse_pos[2];
int  g_window_width=800;
int  g_window_height=640;

kaufen g_Kaufen;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_KAU));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "Wallstreeting";
    wcex.hIconSm = LoadIcon(NULL, NULL);


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "WallStreeting",
                          "WallStreeting",
                          (WS_OVERLAPPEDWINDOW | WS_SYSMENU) - (WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME),
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          g_window_width,
                          g_window_height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    //if debug mode start console
    string command_line=lpCmdLine;
    if(command_line=="debug")
    {
        //Open a console window
        AllocConsole();
        //Connect console output
        HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
        int hCrt          = _open_osfhandle((long) handle_out, _O_TEXT);
        FILE* hf_out      = _fdopen(hCrt, "w");
        setvbuf(hf_out, NULL, _IONBF, 1);
        *stdout = *hf_out;
        //Connect console input
        HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
        hCrt             = _open_osfhandle((long) handle_in, _O_TEXT);
        FILE* hf_in      = _fdopen(hCrt, "r");
        setvbuf(hf_in, NULL, _IONBF, 128);
        *stdin = *hf_in;
        //Set console title
        SetConsoleTitle("Debug Console");

        cout<<"Software started\n";
        g_Kaufen.set_confirmation_mode(true);
    }

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    //Init game
    g_Kaufen.init(g_keys,g_mouse_but,g_mouse_pos,g_window_width,g_window_height);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            //clear screen buffer
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glLoadIdentity();

            int ret_val=g_Kaufen.cycle();//game calc and drawing
            switch(ret_val)//return value for init menu
            {
                case 1://host
                {
                    cout<<"Started hosting\n";
                    g_Kaufen.init_network("server");
                    WSAAsyncSelect( g_Kaufen.get_socket() , hwnd, WM_WSAASYNC, FD_READ | FD_WRITE | FD_ACCEPT | FD_CONNECT | FD_CLOSE);
                    g_Kaufen.start_hosting();
                    //start to listen

                }break;

                case 2://join
                {
                    cout<<"Try to join the server\n";
                    g_Kaufen.init_network("client");
                    WSAAsyncSelect( g_Kaufen.get_socket() , hwnd, WM_WSAASYNC, FD_READ | FD_WRITE | FD_ACCEPT | FD_CONNECT | FD_CLOSE);
                    g_Kaufen.connect_to_server();
                }break;

                case 3://help
                {

                }break;

                case 4://exit
                {
                    cout<<"Shutting down software\n";
                    bQuit=TRUE;
                }break;
            }

            SwapBuffers(hDC);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        }
        break;

        case WM_DESTROY:
        {
            return 0;
        }
        break;

        case WM_MOUSEMOVE:
        {
             g_mouse_pos[0]=LOWORD(lParam);
             g_mouse_pos[1]=HIWORD(lParam)+28;//-22 pixel shift to get same coord as drawing
        }
        break;

        case WM_LBUTTONDOWN:
        {
             g_mouse_but[0]=true;
        }
        break;

        case WM_LBUTTONUP:
        {
             g_mouse_but[0]=false;
        }
        break;

        case WM_RBUTTONDOWN:
        {
             g_mouse_but[1]=true;
             //cout<<"x: "<<g_mouse_pos[0]<<", y: "<<g_mouse_pos[1]<<endl; //temp
        }
        break;

        case WM_RBUTTONUP:
        {
             g_mouse_but[1]=false;
        }
        break;

        case WM_MOUSEWHEEL:
        {
            if(HIWORD(wParam)>5000) {g_mouse_but[2]=true;}
            if(HIWORD(wParam)>100&&HIWORD(wParam)<5000) {g_mouse_but[3]=true;}
        }
        break;

		case WM_KEYDOWN:
		{
			g_keys[wParam]=true;
		}
		break;

		case WM_KEYUP:
		{
			g_keys[wParam]=false;
		}
		break;

		case WM_WSAASYNC://Network message
        {
            switch(WSAGETSELECTEVENT(lParam))//get message
            {
                case FD_READ: //incomming data from SOCKET wParam
                {
                    //cout<<"FD_READ\n";
                    g_Kaufen.recv_data(wParam);
                } break;

                case FD_WRITE: //only used if sending large files
                {
                    //cout<<"FD_WRITE\n";
                } break;

                case FD_ACCEPT://client wants to join
                {
                    cout<<"Client wants to join\n";

                    if( g_Kaufen.connect_to_client(wParam) )
                    {
                        cout<<"Client connected\n";
                    }
                    else cout<<"ERROR: Client could not be connected\n";
                } break;

                case FD_CONNECT://you have joined a server
                {
                    cout<<"Server accepted you to join\n";
                    if(g_Kaufen.test_connection() )
                    {
                        cout<<"Connection to server OK\n";
                    }
                    else cout<<"ERROR: Bad connection to server\n";

                } break;

                case FD_CLOSE: //lost client
                {
                    cout<<"Lost connection\n";
                    g_Kaufen.remove_player(wParam);

                } break;

            }
        }


        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);

    //set 2D mode

    glClearColor(0.0,0.0,0.0,0.0);  //Set the cleared screen colour to black
    glViewport(0,0,g_window_width,g_window_height);   //This sets up the viewport so that the coordinates (0, 0) are at the top left of the window

    //Set up the orthographic projection so that coordinates (0, 0) are in the top left
    //and the minimum and maximum depth is -10 and 10. To enable depth just put in
    //glEnable(GL_DEPTH_TEST)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,g_window_width,g_window_height,0,-1,1);

    //Back to the modelview so we can draw stuff
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Enable antialiasing
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

