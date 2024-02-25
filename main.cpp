#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

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

struct Instance {
	Instance() { menu(); }

	void palsMenu() {
		int dump;

		cout << "--ID"
			 << "------------ESPECIE"
			 << "--------TIPO" << endl;
		for (auto& pal : gPals) {
			cout << setw(4) << pal["id"].get<string>() << setw(19) << pal["especie"].get<string>() << setw(12)
				 << pal["tipo"].get<string>() << endl;
		}

		cout << "use enter para voltar";
		cin >> dump;
		cin.clear();
	}

	void menu() {
		string opt[] = {"1.Inventário", "2.Pals", "3.Ataques", "0.Sair"};

		int escolha;
		while (true) {
			for (string opt : opt) {
				cout << opt << endl;
			}
			cin >> escolha;
			cin.clear();

			switch (escolha) {
				case 0:
					return;
					break;
				case 1:
					cout << "salve" << endl;
					break;
				case 2:
					palsMenu();
					break;
			}
		}
	}
};

// MAIN
int main() {
	srand(1);

	Instance ins;

	/*
	Pal primeiroPal(01);
	Pal segundoPal(02);

	primeiroPal.print();
	segundoPal.print();
	*/

	return 0;
}