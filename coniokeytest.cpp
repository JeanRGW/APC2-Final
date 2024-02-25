#include <conio.h>

#include <iostream>
using namespace std;

int main() {
	while (true) {
		if (kbhit()) {
			int key = getch();
			cout << key << endl;
		}
	}
	return 0;
}