/*
	AUTHOR: Ultrae 

	TODO:
	Adapter la taille du buffer au fichier
	Ajouter un support UNICODE
	Faire le moins possible d'appel à SendInput()
	Gérer les accents
*/

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <Winuser.h>
#include <Strsafe.h>


BOOL KeysToInputs(
	CHAR* buffer,
	DWORD dwSize,
	DWORD dwDelay,
	DWORD dwLag
)
{
	UINT cbInputs = 0;
	INPUT inputTAB;
	ZeroMemory(&inputTAB, sizeof(INPUT));
	

	// Delay avant le début de l'écriture
	Sleep(dwDelay);
	for (size_t i = 0; i < dwSize; ++i)
	{

		// Convention de séparation entre les mots
		if (buffer[i] == '|')
			buffer[i] = ' ';
		
		// Latence entre chaque lettre pour éviter d'avoir 99999 WPM
		Sleep(dwLag);
		inputTAB.type = INPUT_KEYBOARD;
		inputTAB.ki.wVk = (WORD)toupper(buffer[i]);
		inputTAB.ki.dwFlags = 0;
		cbInputs = SendInput(
			1,
			&inputTAB,
			sizeof(INPUT)
		);

		// On relâche la touche
		// TODO : Faire qu'un seul appel à SendInput
		inputTAB.ki.dwFlags = KEYEVENTF_KEYUP;
		cbInputs = SendInput(
			1,
			&inputTAB,
			sizeof(INPUT)
		);

		printf("\r\n%d -> Inputs send : %ud, error : %d | %c (0x%x)\r\n", i, cbInputs, GetLastError(), buffer[i], buffer[i]);

	}

	return TRUE;
}

BOOL ReadTaping(
	CONST CHAR* pszPath,
	LPSTR lpBuffer,
	LPDWORD lpdwRead
)
{
	HFILE hFile;
	OFSTRUCT OfStruct;

	hFile = OpenFile(
		pszPath,
		&OfStruct,
		OF_READ
	);


	ReadFile(
		(HANDLE)hFile,
		lpBuffer,
		16000,
		lpdwRead,
		NULL
	);

	printf("%d %.*s", lpdwRead, lpdwRead, lpBuffer);
	

	return TRUE;
}


int main(int argc, CHAR* argv[])
{
	CHAR buffer[16000];
	DWORD dwSize = 0;
	DWORD dwDelay = 2500;
	DWORD dwLag = 69;

	if (argc < 2)
	{
		printf("FastKeyStroke.exe <chemin_vers_le_fichier_texte> <delay avant début (en ms)> <latence entre chaque input (en ms)> \r\n");
		return EXIT_FAILURE;
	}
	if (argc > 2)
		dwDelay = atoi(argv[2]);
	if (argc > 3)
		dwLag = atoi(argv[3]);
	if (argc >= 5)
		return EXIT_FAILURE;

	ReadTaping(argv[1], buffer, &dwSize);
	KeysToInputs(buffer, dwSize, dwDelay, dwLag);
	return EXIT_SUCCESS;
}

