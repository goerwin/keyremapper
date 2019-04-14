#include <windows.h>
#include <string>
#include <psapi.h>
#include <chrono>
#include <fstream>

double previousTime = clock();

namespace ErwinUtils {
	double getDifferenceBetweenClocks(double clock1, double clock2) {
		return (clock1 - clock2) / CLOCKS_PER_SEC * 1000;
	}

	void writeToFile(std::string name, std::string content, bool append = true, bool newLine = true) {
		std::ofstream outfile;

		if (append) {
			outfile.open(name, std::ios_base::app);
		} else {
			outfile.open(name);
		}

		outfile << content.append(newLine ? "\n" : "");
	}

	std::string getActiveWindowProcessName(HWND hwnd) {
		if (!hwnd) {
			return NULL;
		}

		std::string processName;
		DWORD dwPID;
		GetWindowThreadProcessId(hwnd, &dwPID);
		HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);

		if (handle) {
			TCHAR path[MAX_PATH];

			if (GetModuleFileNameEx(handle, 0, path, MAX_PATH)) {
				processName = path;
				std::string::size_type idx = processName.rfind("\\");

				if (idx != std::string::npos) {
					processName = processName.substr(idx + 1);
				}
			}

			CloseHandle(handle);
		}

		return processName;
	}
}
