#include <Windows.h>
#include <lmcons.h>
#include <iostream>

void cmd(const char* fmt, ...) {
	char buffer[MAX_PATH];
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	system(buffer);
}

int main(int argc, char *argv[])
{
	bool pause = false;
	for (int i = 0; i != argc; i++) {
		// Help
		if (!argv[1] || strstr(argv[i], "/?")) {
			std::cout << "Syntax" << std::endl;
			std::cout << "   ForceDelete [options]" << std::endl;
			std::cout << "Key" << std::endl;
			std::cout << "   options:" << std::endl;
			std::cout << "      [path]  File to delete." << std::endl;
			std::cout << "      /A  Add entry to context menu using current path." << std::endl;
			std::cout << "      /R  Remove entry from context menu." << std::endl;
			std::cout << "      /P  Pause after deletion." << std::endl;
			cmd("pause");
			return EXIT_SUCCESS;
		}

		// Add registry keys
		if (strstr(argv[i], "/A")) {
			// reg add (path) /t REG_SZ /d (data)
			cmd("reg add HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\*\\shell\\forcedel /t REG_SZ /d \"Force Delete\"");
			cmd("reg add HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\*\\shell\\forcedel\\command /t REG_SZ /d \"%s \\\"%%1\\\"\"", argv[0]);
			cmd("reg add HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\Folder\\shell\\forcedel /t REG_SZ /d \"Force Delete\"");
			cmd("reg add HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\Folder\\shell\\forcedel\\command /t REG_SZ /d \"%s \\\"%%1\\\"\"", argv[0]);
			cmd("pause");
			return EXIT_SUCCESS;
		}

		// Delete registry keys
		if (strstr(argv[i], "/R")) {
			// reg delete (path) /f
			cmd("reg delete HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\*\\shell\\forcedel /f");
			cmd("reg delete HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\Folder\\shell\\forcedel /f");
			cmd("pause");
			return EXIT_SUCCESS;
		}

		if (strstr(argv[i], "/P")) {
			pause = true;
		}
	}

	DWORD attr = GetFileAttributesA(argv[1]);
	char user[UNLEN];
	DWORD userlen = UNLEN;
	GetUserNameA(user, &userlen);

	// Take ownership
	// takeown [/r] /f (path)
	cmd(attr == FILE_ATTRIBUTE_DIRECTORY ? "takeown /r /f \"%s\"" : "takeown /f \"%s\"", argv[1]);

	// Allow full control
	// icacls (path) /t /grant (username):F
	cmd("icacls \"%s\" /t /grant %s:F", argv[1], user);

	// Delete
	// del /F /Q (path)
	// rmdir /Q (path)
	cmd("del /F /Q \"%s\"", argv[1]);
	if (attr == FILE_ATTRIBUTE_DIRECTORY)
		cmd("rmdir /Q \"%s\"", argv[1]);

	if (pause) cmd("pause");
	return EXIT_SUCCESS;
}