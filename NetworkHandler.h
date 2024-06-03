#include <iostream>
#include <string>
#include "framework.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WS2tcpip.h>
#include <processthreadsapi.h>
#pragma comment (lib, "ws2_32.lib")

class NetworkHandler
{
public :

	NetworkHandler();
	~NetworkHandler();
	
	static NetworkHandler* App();
	
	void CreateTheWindow();
	
	void launch();
	
	static LRESULT WndProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	void HandleMessage();
	
	int InitSocket(std::string url, std::string port);

	void Send(std::string message);

	static NetworkHandler* nhInstance;
	SOCKET socketInfo;
	HWND hwnd;

};

