#include "NetworkHandler.h"

#define WM_SOCKET     (WM_USER + 1)
#define CLASS_NAME  L"ClientWinClass"

NetworkHandler* NetworkHandler::nhInstance = nullptr;

NetworkHandler::NetworkHandler()
{
}

NetworkHandler::~NetworkHandler()
{
}


NetworkHandler* NetworkHandler::App()
{
	if (nhInstance == nullptr)
	{
		nhInstance = new NetworkHandler();
	}
	return nhInstance;
}

void NetworkHandler::CreateTheWindow()
{
	WNDCLASS wc = {};

	wc.lpfnWndProc = NetworkHandler::WndProc;
	wc.hInstance = NULL;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"The invisile window",         // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window
		NULL,       // Menu
		NULL,  // Instance handle
		NULL        // Additional application data
	);
}

void NetworkHandler::launch()
{
	CreateTheWindow();
}

LRESULT CALLBACK NetworkHandler::WndProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKET socket;
	std::string raw;

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Analyse les s�lections de menu�:
		switch (wmId)
		{
		default:
			return DefWindowProc(_hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SOCKET:


		if (WSAGETSELECTERROR(lParam))
		{
			closesocket((SOCKET)wParam);
			break;
		}

		// Determine what event occurred on the socket
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			
			std::cout << "salut";
			//HandleMessage();

			break;
		case FD_CLOSE:
			std::cout << "disconnected\n";
			break;
		}
	default:
		return DefWindowProc(_hWnd, message, wParam, lParam);
	}
	return 0;
}


void NetworkHandler::HandleMessage()
{
	MSG msg = { 0 };

	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
	}
}

int NetworkHandler::InitSocket(std::string url, std::string port )
{
	SOCKET ConnectSocket;

	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return -1;
	}

	addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(url.c_str(), port.c_str(), &hints, &result); // TODO Give the rigth URL (is localhost rn)
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return -1;
	}

	ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}

	//___________________________________________________________________________________________________________________________
	// 
	// Connect to server.
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	WSAAsyncSelect(ConnectSocket, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return -1;
	}

	this->socketInfo = ConnectSocket;

	return 0;
}


void NetworkHandler::Send(std::string message)
{
	send(socketInfo, message.c_str(), message.size(), 0);
}