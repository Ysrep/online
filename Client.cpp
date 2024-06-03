#include <iostream>
#include <string>
#include "framework.h"
#include "NetworkHandler.h"
#include "Client.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WS2tcpip.h>
#include <processthreadsapi.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define SOCKET_EVENT     (WM_USER + 1)

HINSTANCE hInst;
BOOL InitInstance(HINSTANCE, int);
SOCKET sock;
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{

    NetworkHandler* networkHandler = NetworkHandler::App();

    networkHandler->launch();

    if (networkHandler->InitSocket("127.0.0.1", "5004") == -1)
    {
        cout << "test\n";
    }
    else
    {
        cout << "dinguerie\n";
    }

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	sf::RenderWindow window(sf::VideoMode(1000, 1000), "PARTIEL");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                
                NetworkHandler* networkHandler = NetworkHandler::App();
                networkHandler->Send("CLIC !!!!!");

                break;
            }
            
        }
        window.clear();
        window.display();
    }

	return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(L"REZO", L"PARTIEL", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

