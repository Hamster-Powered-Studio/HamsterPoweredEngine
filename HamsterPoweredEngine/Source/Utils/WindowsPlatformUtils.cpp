#include "PlatformUtils.h"
#include <Windows.h>
#include "SFML/Window.hpp"
#include "SFML/Main.hpp"
#include "SFML/System.hpp"
#include <PathCch.h>


//#pragma comment(lib, "Pathcch.lib")


std::string FileDialogs::OpenFile(sf::WindowHandle handle, const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = handle;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}
	return std::string();
}

std::string FileDialogs::SaveFile(sf::WindowHandle handle, const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = handle;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}
	return std::string();
}

TCHAR* FileDialogs::GetThisPath(TCHAR* dest, size_t destSize)
{
	{
		if (!dest) return NULL;

		DWORD length = GetModuleFileName( NULL, dest, destSize );
#if (NTDDI_VERSION >= NTDDI_WIN8)
		PathCchRemoveFileSpec(dest, destSize);
#else
		if (MAX_PATH > destSize) return NULL;
		PathRemoveFileSpec(dest);
#endif
		return dest;
	}
}