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
#define KEY_BACKSPACE 8

#define KEY_N_UP 78
#define KEY_N_DOWN 110
#define KEY_S_UP 83
#define KEY_S_DOWN 115

using json = nlohmann::json;
using namespace std;

// Modelos
json palModel = {{"ataquesPermitidos", {"string", "array"}},
				 {"base", {{"atk", 0}, {"def", 0}, {"hp", 0}}},
				 {"especie", "string"},
				 {"tipo", "string"}};

json ataqueModel = {{"dmg", 0}, {"nome", "string"}, {"tipo", "string"}};

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
		// Checar se o arquivo não está vazio
		if (inJson.peek() != ifstream::traits_type::eof()) {
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

// Menu com lógica de on/off, atualmente usado apenas para permitir ataques extras especiais em um pal
struct nMenuSelecionado {
	int escolha	   = 0;
	int pressedKey = 0;
	bool esc	   = false;

	vector<string> opts;

	json* sourceList;
	json* actualList;

	nMenuSelecionado(json* ActualList, json* SourceList) : sourceList(SourceList), actualList(ActualList) { update(); }

	void update() {
		opts = {};
		for (auto& sourceElement : *sourceList) {
			string line;
			// Se o valor estiver contido nos atuais (selecionados) destaca o valor
			if (valorContido(*actualList, sourceElement["nome"])) {
				line = "\e[0;31m";
				line += sourceElement["nome"];
				line += "\e[0m";
			} else {
				line = sourceElement["nome"];
			}
			opts.push_back(line);
		}
	}

	// (Conteúdo: Recursão e busca)
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
		// Valor a ser buscado
		string searchValue = (*sourceList)[escolha]["nome"];

		// Indice encontrado, -1 para não encontrado
		int index = busca(*actualList, 0, (*actualList).size() - 1, searchValue);

		// Apagar or adicionar
		if (index != -1) {
			(*actualList).erase(index);
		} else {
			(*actualList).push_back(searchValue);
		}
		update();
	}

	void interact() {
		// Montar frame
		string frame = "Ataques permitidos\n";

		for (int i = 0; i < opts.size(); i++) {
			if (i == escolha) {
				frame += (">" + opts[i]);  // Highlight selecionado
			} else {
				frame += opts[i];
			}

			frame += "\n";
		}
		system("cls");
		cout << frame << endl;

		// Interação
		int key = getch();
		if (key == KEY_UP && escolha > 0) {
			escolha--;
		} else if (key == KEY_DOWN && escolha < opts.size() - 1) {
			escolha++;
		} else if (key == KEY_ESC) {
			esc = true;
		} else {
			pressedKey = key;
		}
	};

	int keyPress() {
		// Impedir que a tecla continue pressionada entre interações
		int buf	   = pressedKey;
		pressedKey = 0;

		return buf;
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
	int escolha	   = 0;
	int pressedKey = 0;
	bool esc	   = false;

	// Opções
	vector<string> opt;
	string header  = "";
	bool hasHeader = false;

	// Update
	bool dynamic	= true;
	bool ordenavel	= false;
	int indexOrdena = -2;
	bool vertical	= false;
	json* source;
	vector<string> props;
	vector<string> headers;

	// Construtor simples, ({opção, opção1, opção2})
	Menu(vector<string> opts) : opt(opts) {
		dynamic = false;  // Não tentar atualizar
	};

	// Construtor para json sem header, utiliza props, Menu(arrayjson, {prop1, prop2, prop3}, vertical?, ordenavel?)
	Menu(json* Source, vector<string> Props, bool Vertical = false, bool Ordenavel = false)
		: source(Source), props(Props), headers(Props), vertical(Vertical), ordenavel(Ordenavel) {
		sort();
		update();
	}

	// Construtor para json, Menu(arrayjson, {prop1, prop2, prop3}, {header1, header2, header3}, vertical?, ordenavel?)
	Menu(json* Source, vector<string> Props, vector<string> Headers, bool Vertical, bool Ordenavel = false)
		: source(Source), props(Props), headers(Headers), vertical(Vertical), ordenavel(Ordenavel) {
		sort();
		update();
	}

	// Montar header horizontal
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

	// Montar corpo do menu horizontal
	void constructHorizontalJsonBody(json jsonArray, vector<string> props) {
		opt.clear();

		for (auto el : jsonArray) {
			string line = "";

			for (string prop : props) {
				string propValue = el[prop].dump();
				propValue.resize(20, ' ');
				line += propValue;
			}

			opt.push_back(line);
		}
	}

	// Montar menu vertical
	void constructVerticalJsonBody(json jsonObject, vector<string> props, vector<string> headers) {
		opt.clear();

		for (int i = 0; i < props.size(); i++) {
			string line = headers[i] + ": ";
			line += jsonObject[props[i]].dump();

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

	template <typename T>
	void troca(T& a, T& b) {
		T temp = a;
		a	   = b;
		b	   = temp;
	}

	// (Conteúdo: ordenação)
	void sort() {
		if (ordenavel == true) {
			if (indexOrdena == -2) {
				indexOrdena = 0;
			} else if (indexOrdena < props.size() - 1) {
				indexOrdena++;
			} else {
				indexOrdena = 0;
			}

			// Propriedade para a ordenação
			string sortingProp = props[indexOrdena];

			for (int i = 0; i < (*source).size() - 1; i++) {
				for (int j = 0; j < (*source).size() - i - 1; j++) {
					auto val1 = (*source)[j][sortingProp];
					auto val2 = ((*source)[j + 1][sortingProp]);

					if (val1 > val2) {
						troca((*source)[j], (*source)[j + 1]);
					}
				}
			}
		}
	}

	// Exibição e interação
	void interact() {
		// Atualizar menu
		if (dynamic) {
			update();
		}

		// Montar frame
		string frame = "";
		if (hasHeader) {
			frame += header;
		}

		for (int i = 0; i < opt.size(); i++) {
			if (i == escolha) {
				frame += ">";
			}

			frame += opt[i] + "\n";
		}

		if (ordenavel) {
			frame += ("Ordenado por " + props[indexOrdena] + "\n");
		}

		system("cls");
		cout << frame << endl;

		// Interação
		int key = getch();
		if (key == KEY_UP && escolha > 0) {
			escolha--;
		} else if (key == KEY_DOWN && escolha < opt.size() - 1) {
			escolha++;
		} else if (key == KEY_ESC) {
			esc = true;
		} else if (key == KEY_S_UP || key == KEY_S_DOWN) {
			sort();
		} else {
			pressedKey = key;
		}
	};

	// Retorna a tecla pressionada
	int keyPress() {
		int buf	   = pressedKey;
		pressedKey = 0;
		return buf;
	}

	// Checar se deve sair do menu
	bool exit() {
		if (esc == true) {
			esc = false;
			return true;
		} else {
			return false;
		}
	}
};

class Instance {
   private:
	// Geral
	// Ler um input com tratamento de erro
	template <typename T>
	void ler(T& valor) {
		while (!(cin >> valor)) {
			cin.clear();
			cin.ignore(999, '\n');

			cerr << "Erro, insira novamente: ";
		}
	}

	json preencherObjeto(const json& model) {
		json novoItem;
		cout << "Preenchendo um novo objeto" << endl;

		// Faz entrada do usuário para cada campo do modelo
		for (const auto& item : model.items()) {
			cout << "Digite um novo valor para " << item.key() << ": ";

			if (item.key() == "tipo") {	 // Função especializada para tipos
				novoItem[item.key()] = escolhaTipo();
			} else if (item.key() == "ataquesPermitidos") {	 // Função epecializada para ataques
				novoItem["ataquesPermitidos"] = json::array();
				menuAtaquesPermitidos(novoItem);
			} else if (item.value() == "string") {	// String
				string userInput;
				cin.ignore(999, '\n');
				getline(cin, userInput);
				novoItem[item.key()] = userInput;
			} else if (item.value().is_number()) {	// Numero
				double userInput;
				ler(userInput);
				novoItem[item.key()] = userInput;
			} else if (item.value().is_object()) {	// Objeto, chamada recursiva
				novoItem[item.key()] = preencherObjeto(item.value());
			}
		}

		// Push the new data to the JSON array
		return novoItem;
	}

	// Menu interativo para escolher um tipo válido
	string escolhaTipo() {
		Menu menuEscolhaTipo(gTipos);
		while (!menuEscolhaTipo.exit()) {
			menuEscolhaTipo.interact();

			if (menuEscolhaTipo.keyPress() == KEY_ENTER) {
				return gTipos[menuEscolhaTipo.escolha];
			}
		}

		return "neutral";
	};

	// Menus do ataque selecionado
	void menuAtaqueSelecionado(int id) {
		Menu menu(&gAtaques[id], {"nome", "dmg", "tipo"}, {"Nome", "Dano", "Tipo"}, true);

		while (!menu.exit()) {
			menu.interact();

			if (menu.keyPress() == KEY_ENTER) {
				switch (menu.escolha) {
					case 0: {
						cout << "Insira um novo nome para o ataque: ";
						string buffer;
						cin.ignore(999, '\n');
						getline(cin, buffer);
						gAtaques[id]["nome"] = buffer;
					} break;

					case 1: {
						cout << "Insira um novo valor para o dano do ataque: ";
						double buffer;
						ler(buffer);

						gAtaques[id]["dmg"] = buffer;
					} break;

					case 2: {
						gAtaques[id]["tipo"] = escolhaTipo();
					} break;
				}

				updateFiles();	// Atualizar arquivos
			}
		}
	}

	// Menu de todos os ataques
	void menuAtaques() {
		Menu menu(&gAtaques, {"nome", "dmg", "tipo"}, {"Nome", "Dano", "Tipo"}, false, true);
		while (!menu.exit()) {
			menu.interact();

			switch (menu.keyPress()) {
				case KEY_ENTER: {
					menuAtaqueSelecionado(menu.escolha);
				} break;

				case KEY_N_DOWN:
				case KEY_N_UP: {
					json novoAtaque = preencherObjeto(ataqueModel);
					gAtaques.push_back(novoAtaque);
					updateFiles();
				} break;

				case KEY_BACKSPACE: {
					if (gAtaques.size() > 0) {
						gAtaques.erase(menu.escolha);
						updateFiles();

						// Reposicionar caso delete o ultimo elemento
						if (menu.escolha > gAtaques.size() - 1) {
							menu.escolha--;
						}
					}
				} break;
			}
		}
	}

	// Menus de pals
	// Menu ataques permitidos on/off
	void menuAtaquesPermitidos(json& pal) {
		nMenuSelecionado menu(&pal["ataquesPermitidos"], &gAtaques);
		while (!menu.exit()) {
			menu.interact();

			if (menu.keyPress() == KEY_ENTER) {
				menu.add_remove();
			}
		}
	}

	// Menu do pal selecionado
	void menuPalSelecionado(int id) {
		Menu menu(&gPals[id], {"ataquesPermitidos", "base", "especie", "tipo"},
				  {"Ataques Permitidos", "Base", "Especie", "Tipo"}, true);

		while (!menu.exit()) {
			menu.interact();

			if (menu.keyPress() == KEY_ENTER) {
				switch (menu.escolha) {
					case 0: {
						menuAtaquesPermitidos(gPals[id]);
					} break;
					case 1: {
						gPals[id]["base"] = preencherObjeto(palModel["base"]);
					} break;
					case 2: {
						cout << "Insira um novo nome para a especie";
						string buffer;
						cin.ignore(999, '\n');
						getline(cin, buffer);
						gPals[id]["especie"] = buffer;
					} break;
					case 3: {
						gPals[id]["tipo"] = escolhaTipo();
					} break;
				}

				updateFiles();	// Atualizar arquivos
			}
		}
	}

	// Menu de todos os pals
	void menuPals() {
		Menu menu(&gPals, {"especie", "tipo"}, {"Especie", "Tipo"}, false, true);

		while (!menu.exit()) {
			menu.interact();

			switch (menu.keyPress()) {
				case KEY_ENTER: {
					menuPalSelecionado(menu.escolha);
				} break;

				case KEY_N_DOWN:
				case KEY_N_UP: {
					json novoPal = preencherObjeto(palModel);
					gPals.push_back(novoPal);
					updateFiles();
				} break;

				case KEY_BACKSPACE: {
					if (gPals.size() > 0) {
						gPals.erase((menu.escolha));
						updateFiles();

						// Reposicionar caso delete o ultimo elemento
						if (menu.escolha > gPals.size() - 1) {
							menu.escolha--;
						}
					}
				} break;
			}
		}
	}

	// Menu inicial
	void menuInicial() {
		Menu menu({"Pals", "Ataques", "Sair"});

		while (!menu.exit()) {
			menu.interact();

			if (menu.keyPress() == KEY_ENTER) {
				switch (menu.escolha) {
					case 0:
						menuPals();
						break;
					case 1:
						menuAtaques();
						break;
					case 2:
						return;
						break;
				}
			}
		}
	}

   public:
	Instance() { menuInicial(); }
};

// MAIN
int main() {
	gAtaques = rJson("ataques.json");
	gPals	 = rJson("pals.json");

	// (Conteúdo: Alocação dinâmica)
	if (gPals.empty()) {
		json* pal = new (nothrow) json;
		if (pal == nullptr) {
			cerr << "Deu ruim" << endl;
			return 1;
		}

		(*pal)["especie"] = "Placeholder";
		(*pal)["tipo"]	  = "neutral";
		json bases;
		bases["hp"]					= 0;
		bases["atk"]				= 0;
		bases["def"]				= 0;
		(*pal)["base"]				= bases;
		(*pal)["ataquesPermitidos"] = json::array();
		gPals.push_back(*pal);

		delete pal;
		pal = nullptr;
	}

	if (gAtaques.empty()) {
		json* ataque	  = new json;
		(*ataque)["nome"] = "Template";
		(*ataque)["dmg"]  = 0;
		(*ataque)["dmg"]  = "neutral";
		gAtaques.push_back(*ataque);

		delete ataque;
		ataque = nullptr;
	}

	Instance runtime;

	return 0;
}