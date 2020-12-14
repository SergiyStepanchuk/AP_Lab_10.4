// Lab_10_4
#include <iostream>
#include <fstream>
#include <iomanip>
#include <windows.h>
#include <string>
using namespace std;

namespace cons {

	COORD GetBufferSize() {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbInfo;
		GetConsoleScreenBufferInfo(handle, &csbInfo);
		return { csbInfo.srWindow.Right - csbInfo.srWindow.Left ,
				csbInfo.srWindow.Bottom - csbInfo.srWindow.Top };
	}

	const COORD size = GetBufferSize();

	void clear() {
		system("cls");
	}

	void gotoxy(const COORD pos) {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(handle, pos);
	}

	COORD getxy() {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(handle, &csbi))
			return { -1, -1 };
		return csbi.dwCursorPosition;
	}

	void clearto(const COORD pos) {
		COORD current_pos = getxy();
		while (current_pos.Y >= pos.Y)
		{
			if (current_pos.Y > pos.Y) {
				gotoxy({ 0, current_pos.Y });
				for (int i = 0; i < size.X; i++)
					cout << ' ';
			}
			else if (current_pos.Y == pos.Y) {
				gotoxy({ pos.X, current_pos.Y });
				for (int i = 0; i <= size.X - pos.X; i++)
					cout << ' ';
			}

			current_pos.Y--;
		}
		gotoxy(pos);
	}

	void change_cusor_visibility(const bool& rst) {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO structCursorInfo;
		GetConsoleCursorInfo(handle, &structCursorInfo);
		structCursorInfo.bVisible = rst;
		SetConsoleCursorInfo(handle, &structCursorInfo);
	}

	template <typename T>
	T input(bool (*check)(T& temp, char* err), const bool& rom, const char* text, ...) {
		COORD start[2] = { getxy() };
		char out[256] = { 0 }, err[256] = { 0 };
		T temp;

		va_list  args;
		va_start(args, text);
		vsprintf_s(out, 255, text, args);
		va_end(args);
		cout << out;
		start[1] = getxy();
		if (check == nullptr)
			check = [](T& temp, char* err) -> bool { return !cin.fail(); };
		bool last = true;
		do {
			if (!last) {
				cin.clear();
				cin.ignore();
				last = true;
			}
			if (err[0] != '\0') {
				change_cusor_visibility(false);
				clearto(start[0]);
				cout << err << endl;
				err[0] = '\0';
				cout << out;
				start[1] = getxy();
				change_cusor_visibility(true);
			}
			else clearto(start[1]);
			cin >> temp;
		} while (last = check(temp, err), !last);
		if (rom)
			clearto(start[0]);
		return temp;
	}
}

void add_number_line_to_file(ifstream& f, ofstream& h) {
	f.clear();
	f.seekg(0);
	string str;
	int id = 1;
	while (!f.eof()) {
		getline(f, str);
		h << id++ << " " << str << endl;
	}
}

void main() {
	SetConsoleCP(1251); // встановлення сторінки win-cp1251 в потік вводу
	SetConsoleOutputCP(1251); // встановлення сторінки win-cp1251 в потік виводу
	cout.setf(ios_base::left);

	char message[256] = { 0 };
	unsigned int state = 0;
	do {
		cons::clear();
		if (message[0] != '\0') {
			cout << message << endl;
			message[0] = '\0';
		}
		cout << "[1] Add number of line to file" << endl
			<< "[0] Exit" << endl;
		state = cons::input<unsigned int>(nullptr, false, "Select action: ");
		switch (state)
		{
		case 1: {
			string in_file = cons::input<string>([](string& str, char* err) -> bool {
				if (cin.fail())
					return false;
				ifstream s(str);
				if (!s.is_open()) {
					sprintf_s(err, 255, "Can't open file!");
					return false;
				}
				s.close();
				return true;
			}, false, "Input in file: ");
			string out_file = cons::input<string>([](string& str, char* err) -> bool {
				if (cin.fail())
					return false;
				ofstream s(str);
				if (!s.is_open()) {
					sprintf_s(err, 255, "Can't open file!");
					return false;
				}
				s.close();
				return true;
				}, false, "Input out file: ");\
			ifstream f(in_file);
			ofstream h(out_file);
			if (f.is_open() && h.is_open()) {
				add_number_line_to_file(f, h);
				sprintf_s(message, 255, "Succesful create file %s!", out_file.c_str());
			}
			else sprintf_s(message, 255, "Error while opening files!");
		}
			  break;
		case 0:
			break;
		default:
			sprintf_s(message, 255, "Incorrect action!");
		}
	} while (state > 0);
}