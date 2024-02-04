#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

struct Treinador {
	string nome;
	string pegoEm;

	// Permite inicializar vazio
	Treinador() {}

	// Inicializar preenchendo cada campo
	Treinador(string Nome, string PegoEm) {
		nome   = Nome;
		pegoEm = PegoEm;
	}

	// Inicializar usando json
	Treinador(json data) {
		nome   = data["nome"];
		pegoEm = data["pegoEm"];
	}

	// Retorna a struct convertida para json
	json toJson() { return json{{"nome", nome}, {"pegoEm", pegoEm}}; }
};

// Recebe e escreve o json
void wJson(json data) {
	ofstream outJson("pokemonExemplo.json");
	outJson << setw(4) << data << endl;
	outJson.close();

	cout << "JSON escrito para o arquivo" << endl;
}

// Lê e retorna o json
json rJson() {
	json data;

	ifstream inJson("pokemonExemplo.json");
	inJson >> data;
	inJson.close();

	cout << "JSON lido do arquivo" << endl;
	return data;
}

int main() {
	// Criar json vazio
	json data;

	/*
	para atribuir usar data["Identificador"] = Valor
	para ler usar Variavel = data["Identificador"]
	*/

	// Adicionar int ou strings
	data["no"]	 = 1;
	data["name"] = "bulbasaur";

	// Adicionar array
	string powers[4] = {"Sharp leaf", "Sugada", "Petal party", "Sugar seed"};
	data["ataques"]	 = powers;

	// Adicionar struct
	Treinador treinadorVazio;
	Treinador treinador{"Jean", "04/02/2024"};

	// Convertendo usando json{{"Identificador", campo.daStruct}, {"Identificador2", campo2.daStruct}}
	data["treinador"] = json{{"nome", treinador.nome}, {"pegoEm", treinador.pegoEm}};
	// Ou método definido na struct, mais bonito e organizado.
	data["treinadorEasy"] = treinador.toJson();

	// Escrito usando função definida ^^^.
	wJson(data);

	// Função que lê e retorna o arquivo json.
	json rdata = rJson();

	// array json para array do C++
	string rpowers[4];
	for (int i = 0; i < rdata["ataques"].size(); i++) {	 // .size() é útil.
		rpowers[i] = rdata["ataques"][i];				 // Popular array
		cout << rpowers[i] << endl;
	}
	cout << endl;

	/* Tambem pode usar
	int i = 0;
	for (string power : rdata["ataques"]) {
		rpowers[i] = power;
		i++;
	}
	*/

	// setw deixa o json mais legível.
	cout << setw(4) << rdata << endl;

	return 0;
}
