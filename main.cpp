#include <conio.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ESC 27
#define KEY_ENTER 13

using json = nlohmann::json;
using namespace std;

// Definir "banco de dados global"
json rJson(string);
json gPals	  = rJson("pals.json");
json gAtaques = rJson("ataques.json");

// Atualizar arquivos
void wJson(json, string);
void updateFiles() {
	wJson(gPals, "pals.json");
	wJson(gAtaques, "ataques.json");
}

// Recebe o json e nome de arquivo e escreve o json
void wJson(json data, string file) {
	ofstream outJson(file);
	outJson << setw(4) << data << endl;	 // setw deixa o json mais legível.
	outJson.close();

	cout << file << " escrito para o arquivo" << endl;
}

// Recebe um nome e lê e retorna o json
json rJson(string file) {
	json data;

	ifstream inJson(file);
	if (inJson.is_open()) {
		inJson >> data;
		inJson.close();
	} else {
		cout << "Nao foi possivel abrir " << file << ", continuando com lista vazia" << endl;
		wJson(data, file);
	}

	return data;
}

// Checar se um valor está contido em um array json
bool valorContido(json ArrayJson, string value) {
	for (const string& elem : ArrayJson) {
		if (elem == value) {
			return true;
		}
	}
	return false;
}

struct Pal {
	string nome;
	string especie;
	int lvl;
	int atk;
	int hp;
	int def;
	json ataques[4];

	Pal() {}

	Pal(int ID) {
		json palInfo = gPals[ID - 1];
		json palBase = palInfo["base"];

		especie = palInfo["especie"];
		nome	= especie;

		// Fazer depois: Definir level de acordo com a situação
		lvl = rand() % 100 + 1;

		hp = palBase["hp"];
		hp *= (1.0 + lvl / 50.0);

		def = palBase["def"];
		def *= (1.0 + lvl / 50.0);

		atk = palBase["atk"];
		atk *= (1.0 + lvl / 50.0);

		definirAtaques(palInfo);
	}

	// Encontrar os ataques permitidos para um pal
	json ataquesPermitidos(json creatureInfo) {
		json ataquesPermitidos;

		json Extras = creatureInfo["ataquesPermitidos"];

		for (const json ataque : gAtaques) {
			if (creatureInfo["tipo"] == ataque["tipo"] || valorContido(Extras, ataque["nome"])) {
				ataquesPermitidos += ataque;
			}
		}

		return ataquesPermitidos;
	}

	// Definir ataques
	void definirAtaques(json creatureInfo) {
		json ataquesPerm = ataquesPermitidos(creatureInfo);

		int maxAtaques = ataquesPerm.size() >= 4 ? 4 : ataquesPerm.size();

		int i = 0;
		while (i < maxAtaques) {
			int random = rand() % ataquesPerm.size();
			ataques[i] = ataquesPerm[random];

			ataquesPerm.erase(random);
			i++;
		}
	}

	void print() {
		cout << nome;
		if (nome != especie) {
			cout << "(" << especie << ")";
		}

		cout << " lvl." << lvl << endl;

		cout << "ATK: " << atk << endl;
		cout << "HP: " << hp << endl;
		cout << "DEF: " << def << endl;

		cout << "Ataques:" << endl;

		for (int i = 0; i < 4; i++) {
			if (!ataques[i].empty()) {
				cout << setw(4) << ataques[i] << endl;
			}
		}
	}
};

struct Menu {
	int escolha = 0;
	vector<string> opt;
	string header  = "";
	bool hasHeader = false;

	bool enter = false;
	bool esc   = false;

	// Construtor vazio
	Menu(){};

	// Construtor simples
	Menu(vector<string> opts) : opt(opts){};

	// Construtor para json, Menu(arrayjson, {prop1, prop2, prop3})
	Menu(json opts, vector<string> props) {
		// Converter para obj para string
		for (auto el : opts) {
			string line = "";
			if (props.size() > 0) {
				for (string prop : props) {
					string propValue = el[prop];
					propValue.resize(20, ' ');
					line += propValue;
				}
			} else {
				line = el;
			}
			opt.push_back(line);
		}

		// Header
		if (props.size() > 0) {
			hasHeader = true;

			for (string prop : props) {
				prop.resize(20, ' ');
				header += prop;
			}

			for (auto& c : header) c = toupper(c);
			header += "\n";
		}
	}

	void fromJson(json obj) {
		opt.clear();
		for (auto [key, value] : obj.items()) {
			string line = key + ": ";
			if (key == "base") {
				for (auto [bKey, bValue] : value.items()) {
					string pair = bKey + "/" + bValue.dump() + " ";
					line += pair;
				}
			} else {
				line += value.dump();
			}
			opt.push_back(line);
		}
	}

	void interact() {
		// Montar frame
		string frame = "\033[2J\033[H";	 // Inicializar com ascii para limpar tela
		if (hasHeader) {
			frame += header;
		}

		for (int i = 0; i < opt.size(); i++) {
			if (i == escolha) {
				frame += ("\e[1;97m" + opt[i] + "\e[0m");  // Highlight selecionado
			} else {
				frame += opt[i];
			}

			frame += "\n";
		}
		cout << frame;

		// Interação
		int key = getch();
		if (key == KEY_UP && escolha > 0) {
			escolha--;
		} else if (key == KEY_DOWN && escolha < opt.size() - 1) {
			escolha++;
		} else if (key == KEY_ENTER) {
			enter = true;
		} else if (key == KEY_ESC) {
			esc = true;
		}
	};

	bool selected() {
		if (enter == true) {
			enter = false;
			return true;
		} else {
			return false;
		}
	}

	bool exit() {
		if (esc == true) {
			esc = false;
			return true;
		} else {
			return false;
		}
	}
};

struct Instance {
	Instance() { menu(); }

	void palMenu(int id) {
		Menu palMenu;
		palMenu.fromJson(gPals[id]);

		while (!palMenu.exit()) {
			palMenu.interact();

			if (palMenu.selected()) {
				string buffer = "";	 // cin direto para json causando erros
				switch (palMenu.escolha) {
					case 0:
						cout << "Mó trampo implementar agr";
						break;
					case 1:
						cout << "Insira valores para as bases de atk, def e hp";
						cin >> gPals[id]["base"]["atk"] >> gPals[id]["base"]["def"] >> gPals[id]["base"]["hp"];
						break;
					case 2:
						cout << "Insira um novo nome para a especie";
						cin >> buffer;
						gPals[id]["especie"] = buffer;
						break;
					case 3:
						cout << "Insira um novo tipo para o pal";
						cin >> buffer;
						gPals[id]["tipo"] = buffer;
						break;
				}
				// Atualizar arquivos, menu e limpar cin.
				updateFiles();
				palMenu.fromJson(gPals[id]);
				cin.clear();
			}
		}
	}

	void palsMenu() {
		Menu palsMenu(gPals, {"especie", "tipo"});

		while (!palsMenu.exit()) {
			palsMenu.interact();

			if (palsMenu.selected()) {
				palMenu(palsMenu.escolha);
			}
		}
	}

	void menu() {
		// vector<string> opt = {"1.Inventário", "2.Pals", "3.Ataques", "0.Sair"};
		Menu mainMenu({"Inventário", "Pals", "Ataques", "Sair"});

		while (!mainMenu.exit()) {
			mainMenu.interact();

			if (mainMenu.selected()) {
				switch (mainMenu.escolha) {
					case 0:
						// inv
						break;
					case 1:
						palsMenu();
						break;
					case 2:
						// ataques
						break;
					case 3:
						return;
						break;
				}
			}
		}
	}
};

// MAIN
int main() {
	srand(1);

	/*
		string a, b;
		cin >> a;
		cin >> b;

		a.resize(24, ' ');
		string final = a + b;
		cout << final;
		cin >> final;
	*/

	Instance ins;

	/*
	Pal primeiroPal(01);
	Pal segundoPal(02);

	primeiroPal.print();
	segundoPal.print();
	*/

	return 0;
}