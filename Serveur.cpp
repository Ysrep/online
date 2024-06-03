#include <iostream>
#include <string>
#include <vector>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WS2tcpip.h>
#include <processthreadsapi.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define SOCKET_EVENT (WM_USER + 1)
#define CLASS_NAME L"ServerWinClass"

HINSTANCE hInst;
vector<SOCKET> clientSockets; // Liste des sockets clients

BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WindowProc(HWND, UINT, WPARAM, LPARAM);

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	/*MyRegisterClass();*/

	HWND hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"LA SUPER DESCRIPTION QUE TU VERRAS PAS",
		WS_OVERLAPPEDWINDOW,
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		NULL,
		NULL
	);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, SW_HIDE);

	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKET Accept;
	switch (message)
	{
	case WM_CLOSE:
		// G�rer l'�v�nement de fermeture de la fen�tre
		MessageBox(NULL, L"Fermeture de la fen�tre cach�e.", L"�v�nement", MB_ICONINFORMATION);
		DestroyWindow(hWnd);
		break;
	case SOCKET_EVENT:
	{
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_ACCEPT:
		{
			if ((Accept = accept(wParam, NULL, NULL)) == INVALID_SOCKET)
			{
				cerr << "accept() failed with error " << WSAGetLastError() << endl;
				closesocket(wParam);
				WSACleanup();
				break;
			}

			clientSockets.push_back(Accept); // Ajouter le client à la liste
			WSAAsyncSelect(Accept, hWnd, SOCKET_EVENT, FD_READ | FD_WRITE | FD_CLOSE);
		}
		break;
		case FD_READ:
		{
			// Code pour lire le message client
			char buf[4096];
			ZeroMemory(buf, sizeof(buf));
			int bytesReceived = recv((SOCKET)wParam, buf, sizeof(buf), 0);
			if (bytesReceived > 0)
			{
				cout << buf;
				// Broadcast le message reçu à tous les autres clients
				for (SOCKET clientSocket : clientSockets)
				{
					if (clientSocket != (SOCKET)wParam)
					{
						send(clientSocket, buf, bytesReceived, 0);
					}
				}
			}
		}
		break;
		case FD_CLOSE:
		{
			// Supprimer le socket du client de la liste
			auto it = find(clientSockets.begin(), clientSockets.end(), (SOCKET)wParam);
			if (it != clientSockets.end())
			{
				clientSockets.erase(it);
			}
			closesocket((SOCKET)wParam);
		}
		break;
		}
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

bool initializeGameServer(SOCKET& listeningSocket, sockaddr_in& hint, int port, HWND hWnd) {
	// Initialisation de Winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0) {
		cerr << "Can't Initialize winsock! Quitting...\n";
		return 1;
	}

	// Cr�ation du socket
	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listeningSocket == INVALID_SOCKET) {
		cerr << "Can't create a socket! Quitting...\n";
		return false;
	}

	// Configuration du socket
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	// Liaison du socket
	if (bind(listeningSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
		cerr << "Can't bind to IP/port! Quitting...\n";
		closesocket(listeningSocket);
		return false;
	}

	// Mise en �coute du socket
	if (listen(listeningSocket, SOMAXCONN) == SOCKET_ERROR) {
		cerr << "Can't listen on socket! Quitting...\n";
		closesocket(listeningSocket);
		return false;
	}

	WSAAsyncSelect(listeningSocket, hWnd, SOCKET_EVENT, FD_ACCEPT | FD_CLOSE);

	string output = "Game Server initialized and listening on port " + to_string(port) + "\n";
	std::cout << output.c_str();
	return true;
}

// Fonction pour le serveur principal
int main() {
	// Creation de la window class
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// D�finir la classe de la fen�tre
	WNDCLASS windowClass = {};
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = L"MyHiddenWindowClass";

	// Enregistrer la classe de fen�tre
	RegisterClass(&windowClass);

	// Cr�er la fen�tre cach�e
	HWND hiddenWindow = CreateWindowEx(
		0,                              // Styles �tendus
		L"MyHiddenWindowClass",        // Nom de la classe
		L"MyHiddenWindow",              // Titre de la fen�tre
		WS_OVERLAPPEDWINDOW,// Style de la fen�tre (fen�tre cach�e)
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	// V�rifier si la fen�tre a �t� cr��e avec succ�s
	if (hiddenWindow == NULL) {
		MessageBox(NULL, L"Erreur lors de la cr�ation de la fen�tre cach�e.", L"Erreur", MB_ICONERROR);
		return 1;
	}

	// Afficher la fen�tre cach�e (si n�cessaire)
	// ShowWindow(hiddenWindow, SW_SHOWNORMAL);

	cout << "Server Main thread running...\n";

	// D�claration des variables pour le serveur
	SOCKET listening;
	sockaddr_in hint;
	char buf[4096]; //message envoyer/recu
	ZeroMemory(buf, sizeof(buf));

	// Initialisation du serveur
	if (!initializeGameServer(listening, hint, 5004, hiddenWindow)) {
		return 0; // Quitter le thread en cas d'�chec de l'initialisation
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Fermeture du serveur
	closesocket(listening);
	WSACleanup();

	return 0;
}
