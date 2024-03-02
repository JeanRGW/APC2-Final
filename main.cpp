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
json gPals;
json gAtaques;
vector<string> gTipos = {"neutral", "fire", "water", "ice", "electric", "ground", "grass", "dark", "dragon"};

// Recebe o json e nome de arquivo e escreve o json
void wJson(json data, string file) {
	ofstream outJson(file);
	outJson << setw(4) << data << endl;	 // setw deixa o json mais legível.
	outJson.close();

	cout << file << " escrito para o arquivo" << endl;
}

// Recebe um nome e lê e retorna o json
json rJson(const string& file) {
	json data = {};

	ifstream inJson(file);
	if (inJson.is_open()) {
		// Check if the file is empty
		if (inJson.peek() != ifstream::traits_type::eof()) {
			// File is not empty, read JSON data
			inJson >> data;
		} else {
			cout << ("Arquivo " + file + " esta vazio.\n");
		}
		inJson.close();
	} else {
		cout << ("Nao foi possivel abrir " + file + ". Usando json vazio.\n");
	}

	return data;
}

// Atualiza os arquivos globais
void updateFiles() {
	wJson(gPals, "pals.json");
	wJson(gAtaques, "ataques.json");
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

struct nMenuSelecionado {
	int escolha = 0;
	bool enter	= false;
	bool esc	= false;

	vector<string> opts;

	json* sourceList;
	json* actualList;

	nMenuSelecionado(json* ActualList, json* SourceList) : sourceList(SourceList), actualList(ActualList) { update(); }

	void update() {
		opts = {};
		for (auto& sourceOption : *sourceList) {
			string line;
			if (valorContido(*actualList, sourceOption["nome"])) {
				line = "\e[0;31m";
				line += sourceOption["nome"];
				line += "\e[0m";
			} else {
				line = sourceOption["nome"];
			}
			opts.push_back(line);
		}
	}

	int busca(json files, int begin, int end, string value) {
		if (begin > end) {
			return -1;
		} else {
			if (files[begin] == value) {
				return begin;
			} else {
				return busca(files, begin + 1, end, value);
			}
		}
	}

	void add_remove() {
		if (valorContido(*actualList, (*sourceList)[escolha]["nome"])) {
			int index = busca(*actualList, 0, (*actualList).size() - 1, (*sourceList)[escolha]["nome"]);
			(*actualList).erase(index);
		} else {
			(*actualList).push_back((*sourceList)[escolha]["nome"]);
		}
		update();
	}

	void interact() {
		// Montar frame
		string frame = "\033[2J\033[H";	 // Inicializar com ascii para limpar tela

		for (int i = 0; i < opts.size(); i++) {
			if (i == escolha) {
				frame += (">" + opts[i]);  // Highlight selecionado
			} else {
				frame += opts[i];
			}

			frame += "\n";
		}
		cout << frame << endl;

		// Interação
		int key = getch();
		if (key == KEY_UP && escolha > 0) {
			escolha--;
		} else if (key == KEY_DOWN && escolha < opts.size() - 1) {
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

struct MenuSelecionado {
	int id;
	int escolha = 0;
	bool enter	= false;
	bool esc	= false;
	vector<string> opts;

	MenuSelecionado(int Id) : id(Id) { update(); }

	void update() {
		opts = {};
		for (auto& ataque : gAtaques) {
			string line;
			if (valorContido(gPals[id]["ataquesPermitidos"], ataque["nome"])) {
				line = "\e[0;31m";
				line += ataque["nome"];
				line += "\e[0m";
			} else {
				line = ataque["nome"];
			}
			opts.push_back(line);
		}
	}

	int busca(json files, int begin, int end, string value) {
		if (begin > end) {
			return -1;
		} else {
			if (files[begin] == value) {
				return begin;
			} else {
				return busca(files, begin + 1, end, value);
			}
		}
	}

	void add_remove() {
		vector<string> vetorAtaquesPermitidos = gPals[id]["ataquesPermitidos"];
		if (valorContido(gPals[id]["ataquesPermitidos"], gAtaques[escolha]["nome"])) {
			int index = busca(vetorAtaquesPermitidos, 0, vetorAtaquesPermitidos.size() - 1, gAtaques[escolha]["nome"]);
			gPals[id]["ataquesPermitidos"].erase(index);
		} else {
			gPals[id]["ataquesPermitidos"].push_back(gAtaques[escolha]["nome"]);
		}
		update();
	}

	void interact() {
		// Montar frame
		string frame = "\033[2J\033[H";	 // Inicializar com ascii para limpar tela

		for (int i = 0; i < opts.size(); i++) {
			if (i == escolha) {
				frame += (">" + opts[i]);  // Highlight selecionado
			} else {
				frame += opts[i];
			}

			frame += "\n";
		}
		cout << frame << endl;

		// Interação
		int key = getch();
		if (key == KEY_UP && escolha > 0) {
			escolha--;
		} else if (key == KEY_DOWN && escolha < opts.size() - 1) {
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

struct Menu {
	// Interação
	int escolha = 0;
	bool enter	= false;
	bool esc	= false;

	// Opções
	vector<string> opt;
	string header  = "";
	bool hasHeader = false;

	// Update
	bool dynamic  = true;
	bool vertical = false;
	json* source;
	vector<string> props;
	vector<string> headers;

	// Construtor simples
	Menu(vector<string> opts) : opt(opts) {
		dynamic = false;  // Não tentar atualizar
	};

	// Construtor para json, Menu(arrayjson, {prop1, prop2, prop3}, {header1, header2, header3}, vertical?)
	Menu(json* Source, vector<string> Props, vector<string> Headers, bool Vertical)
		: source(Source), props(Props), headers(Headers), vertical(Vertical) {
		update();
	}

	// Construtor sem header (utiliza as props)
	Menu(json* Source, vector<string> Props, bool Vertical)
		: source(Source), props(Props), headers(Props), vertical(Vertical) {
		update();
	}

	// Construtor sem header, horizontal
	Menu(json* Source, vector<string> Props) : source(Source), props(Props), headers(Props) { update(); }

	// Montar header
	void constructHeader(vector<string> headers) {
		hasHeader = true;
		header	  = "";

		for (string head : headers) {
			head.resize(20, ' ');
			header += head;
		}

		for (auto& c : header) c = toupper(c);
		header += "\n";
	}

	// Montar corpo do menu
	void constructHorizontalJsonBody(json opts, vector<string> props) {
		opt.clear();

		for (auto el : opts) {
			string line = "";

			for (string prop : props) {
				string propValue = el[prop].dump();
				propValue.resize(20, ' ');
				line += propValue;
			}

			opt.push_back(line);
		}
	}

	// Montar corpo vertical
	void constructVerticalJsonBody(json obj, vector<string> props, vector<string> identifiers) {
		opt.clear();

		for (int i = 0; i < props.size(); i++) {
			string line = identifiers[i] + ": ";
			line += obj[props[i]].dump();

			opt.push_back(line);
		}
	}

	// Atualiza as informações do menu
	void update() {
		if (vertical) {
			constructVerticalJsonBody(*source, props, headers);
		} else {
			constructHeader(headers);
			constructHorizontalJsonBody(*source, props);
		}
	}

	// Exibição e interação
	void interact() {
		// Atualizar menu
		if (dynamic) {
			update();
		}

		// Montar frame
		string frame = "\033[2J\033[H";	 // Inicializar com ascii para limpar tela
		if (hasHeader) {
			frame += header;
		}

		for (int i = 0; i < opt.size(); i++) {
			if (i == escolha) {
				frame += ">";
			}

			frame += opt[i] + "\n";
		}
		cout << frame << endl;

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

	void ataqueMenu(int id) {
		Menu menu(&gAtaques[id], {"nome", "dmg", "tipo"}, {"Nome", "Dano", "Tipo"}, true);

		while (!menu.exit()) {
			menu.interact();

			if (menu.selected()) {
				string buffer = "";	 // cin direto para json causando erros
				switch (menu.escolha) {
					case 0:
						cout << "Insira um novo nome para o ataque: ";
						cin >> buffer;
						gAtaques[id]["nome"] = buffer;
					case 1:
						cout << "Insira um novo valor para o dano do ataque: ";
						cin >> buffer;
						gAtaques[id]["dmg"] = buffer;
						break;
					case 2: {  // Chaves permite declarar variável.

						cout << "Escolha um novo tipo para o ataque:";
						Menu menuEscolhaTipo(gTipos);
						while (!menuEscolhaTipo.exit()) {
							menuEscolhaTipo.interact();

							if (menuEscolhaTipo.selected()) {
								gAtaques[id]["tipo"] = gTipos[menuEscolhaTipo.escolha];
								break;
							}
						}
						break;
					}
				}
				// Atualizar arquivos, menu e limpar cin.
				updateFiles();
				cin.clear();
			}
		}
	}

	void ataquesMenu() {
		Menu menu(&gAtaques, {"nome", "dmg", "tipo"}, {"Nome", "Dano", "Tipo"}, false);
		while (!menu.exit()) {
			menu.interact();

			if (menu.selected()) {
				ataqueMenu(menu.escolha);
			}
		}
	}

	void menuAtaquesPermitidos(int id) {
		nMenuSelecionado menu(&gPals[id]["ataquesPermitidos"], &gAtaques);
		while (!menu.exit()) {
			menu.interact();

			if (menu.selected()) {
				menu.add_remove();
			}
		}

		/*
		MenuSelecionado menu(id);
		while (!menu.exit()) {
			menu.interact();

			if (menu.selected()) {
				menu.add_remove();
			}
		}
		*/
	}

	void palMenu(int id) {
		Menu palMenu(&gPals[id], {"ataquesPermitidos", "base", "especie", "tipo"},
					 {"Ataques Permitidos", "Base", "Especie", "Tipo"}, true);

		while (!palMenu.exit()) {
			palMenu.interact();

			if (palMenu.selected()) {
				string buffer = "";	 // cin direto para json causando erros
				switch (palMenu.escolha) {
					case 0:
						menuAtaquesPermitidos(id);
						break;
					case 1:
						cout << "Insira valores para as bases de atk, def e hp: ";
						cin >> gPals[id]["base"]["atk"] >> gPals[id]["base"]["def"] >> gPals[id]["base"]["hp"];
						break;
					case 2:
						cout << "Insira um novo nome para a especie: ";
						cin >> buffer;
						gPals[id]["especie"] = buffer;
						break;
					case 3:
						cout << "Insira um novo tipo para o pal: ";
						cin >> buffer;
						gPals[id]["tipo"] = buffer;
						break;
				}
				// Atualizar arquivos, menu e limpar cin.
				updateFiles();
				cin.clear();
			}
		}
	}

	void palsMenu() {
		Menu palsMenu(&gPals, {"especie", "tipo"});

		while (!palsMenu.exit()) {
			palsMenu.interact();

			if (palsMenu.selected()) {
				palMenu(palsMenu.escolha);
			}
		}
	}

	void menu() {
		Menu mainMenu({"Inventario", "Pals", "Ataques", "Sair"});

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
						ataquesMenu();
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
	gAtaques = rJson("ataques.json");
	gPals	 = rJson("pals.json");

	if (gPals.empty()) {
		json pal;
		pal["especie"] = "Placeholder";
		pal["tipo"]	   = "neutral";
		json bases;
		bases["hp"]				 = 0;
		bases["atk"]			 = 0;
		bases["def"]			 = 0;
		pal["base"]				 = bases;
		pal["ataquesPermitidos"] = json::array();
		gPals.push_back(pal);
	}

	if (gAtaques.empty()) {
		json ataque;
		ataque["nome"] = "Template";
		ataque["dmg"]  = 0;
		ataque["dmg"]  = "neutral";
		gAtaques.push_back(ataque);
	}

	Instance runtime;

	return 0;
}