#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace std;

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

struct Ataque {
	string nome;
	int dmg;
	string tipo;

	Ataque() {}

	Ataque(string Nome, int Dmg, string Tipo) : nome(Nome), dmg(Dmg), tipo(Tipo) {}

	Ataque(string ID) {
		json ataque = rJson("ataques.json")[ID];

		nome = ataque["nome"];
		dmg	 = ataque["dmg"];
		tipo = ataque["tipo"];
	}

	void print() { cout << nome << endl << dmg << endl << tipo << endl; }
};

struct Pal {
	string nome;
	string especie;
	int lvl;
	int atk;
	int hp;
	int def;
	Ataque ataques[4];

	Pal() {}

	Pal(string Nome, string Especie, int Lvl, int Hp, int Def, int Atk)
		: nome(Nome), especie(Especie), lvl(Lvl), hp(Hp), def(Def), atk(Atk) {}

	Pal(int ID) {
		json palInfo = rJson("pals.json")[ID - 1];
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
	}
};

// MAIN
int main() {
	srand(1);

	Pal primeiroPal(01);

	primeiroPal.print();

	return 0;
}