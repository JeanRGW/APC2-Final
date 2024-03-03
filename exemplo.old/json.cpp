#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace std;

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

struct pessoa {
	string nome;
	string senha;
	int idade;

	void save() {
		json currentData = rJson("banco.json");
		json pessoinha;
		pessoinha["nome"]  = nome;
		pessoinha["idade"] = idade;
		pessoinha["senha"] = senha;
		currentData.push_back(pessoinha);
		wJson(currentData, "banco.json");
	}
};
int main() {
	/*
	json pessoa;
	pessoa["nome"]	= "cleiton";
	pessoa["email"] = "cleitinreidelas@gmail.com";
	pessoa["shape"] = 0;

	json pessoa1;
	pessoa1["nome"]	 = "jorge";
	pessoa1["email"] = "jorge2005@gmail.com";
	pessoa1["shape"] = 2.1;

	banco["humanos"].push_back(pessoa);
	banco["humanos"].push_back(pessoa1);

	cout << setw(4) << banco;

	wJson(banco, "banco.json");
	*/

	pessoa jao;
	jao.nome  = "joao";
	jao.idade = 49;
	jao.senha = "12345678";

	jao.save();

	json banco = rJson("banco.json");
	for (auto &user : banco) {
		if (user["nome"] == "joao") {
			cout << "jao encontrado";
			if (user["senha"] == "12345678") {
				cout << "jao autenticado";
			}
		}
	}
	return 0;
}