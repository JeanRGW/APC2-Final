// Encontrar os ataques permitidos para um pal
json ataquesPermitidos(json creatureInfo) {
	json ataquesPermitidos;

	json ataques = rJson("ataques.json");
	json Extras	 = creatureInfo["ataquesPermitidos"];

	for (const json ataque : ataques) {
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

int main() {
	json ataquesPermitidos;

	json ataques = rJson("ataques.json");
	json Extras	 = creatureInfo["ataquesPermitidos"];

	for (const json ataque : ataques) {
		if (creatureInfo["tipo"] == ataque["tipo"] || valorContido(Extras, ataque["nome"])) {
			ataquesPermitidos += ataque;
		}
	}

	int maxAtaques = ataquesPermitidos.size() >= 4 ? 4 : ataquesPerm.size();

	int i = 0;
	while (i < maxAtaques) {
		int random = rand() % ataquesPermitidos.size();
		ataques[i] = ataquesPermitidos[random];

		ataquesPermitidos.erase(random);
		i++;
	}
}