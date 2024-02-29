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
