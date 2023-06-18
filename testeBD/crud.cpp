#include<iostream>
#include<vector>
#include<string>
#include <mysql.h>

using namespace std;

struct Produto {
	int id{};
	string nome;
	double preco{};

	Produto() {}

	void atualizaProduto(string novoNome) {
		this->nome = novoNome;
	}

	void atualizaProduto(string novoNome, double novoPreco) {
		this->nome = novoNome;
		this->preco = novoPreco;
	}
	void atualizaProduto(double novoPreco) {
		this->preco = novoPreco;
	}
};

ostream& operator<<(ostream& os, Produto& produto) {
	os << "ID    | NOME      | PRECO" << endl;
	os << "------+-----------+---------" << endl;
	os << produto.id;
	os << string(6 - to_string(produto.id).length(), ' ');

	os << "| " << produto.nome;
	os << string(10 - produto.nome.length(), ' ');

	os << "| " << produto.preco << endl;

	return os;
}
istream& operator>>(istream& is, Produto& produto) {
	cout << "Nome do produto: ";
	is >> produto.nome;
	cout << "Preco do produto: ";
	is >> produto.preco;
	return is;
}

bool adicionarProduto(MYSQL& conn) {
	Produto newProduto;
	cin >> newProduto;
	string query = "INSERT INTO produto (nome, preco) VALUES ('" + newProduto.nome + "', " + to_string(newProduto.preco) + ")";
	if (mysql_query(&conn, query.c_str()) == 0) {
		return true; // adição bem-sucedida
	}
	else {
		cout << "Erro ao adicionar valor: " << mysql_error(&conn) << endl;
		return false; // adição falhou
	}
}

bool excluirProduto(MYSQL& conn, int idProdutoDeletado) {
	string query = "DELETE FROM produto WHERE id = '" + to_string(idProdutoDeletado) + "'";
	if (mysql_query(&conn, query.c_str()) == 0) {
		return true;
	}
	else {
		return false;
	}
}

bool atualizaProduto(MYSQL& conn, int id) {
	int escolha;
	cout << "O que deseja atualizar? (1) Nome / (2) Preco / (3) Tudo / (4) Cancelar: ";
	cin >> escolha;
	string query;
	string nome;
	double preco;
	switch (escolha) {
		case 1:
			cin >> nome;
			query = "UPDATE produto SET nome = '" + nome + "' WHERE id = '" + to_string(id) + "'";
			break;
		case 2:
			cin >> preco;
			query = "UPDATE produto SET preco = '" + to_string(preco) + "' WHERE id = '" + to_string(id) + "'";
			break;
		case 3:
			cin >> nome;
			cin >> preco;
			query = "UPDATE produto SET preco = '" + to_string(preco) + "', nome = '" + nome + "' " + "WHERE id = '" + to_string(id) + "'";
			break;
		case 4:
			break;
	}
	
	if (mysql_query(&conn, query.c_str()) == 0) {
		return true;
	}
	else {
		//cout << "Erro ao adicionar valor: " << mysql_error(&conn) << endl;
		return false;
	}

	
}

void selecionarTodosProdutos(MYSQL& conn, MYSQL_ROW& row) {

	MYSQL_RES* res;

	if (mysql_query(&conn, "SELECT * FROM produto")) {
		cout << "Erro ao executar a consulta: " << mysql_error(&conn) << endl;
	}
	else {
		// Obter o resultado da consulta
		res = mysql_use_result(&conn);

		// Iterar pelos registros

		while ((row = mysql_fetch_row(res)) != NULL) {
			// Acessar os valores de cada coluna
			// uma maneira de deixar a apresentação dos produtos mais bonito
			cout << row[0];
			cout << string(6 - strlen(row[0]), ' ');

			cout << "| " << row[1];
			cout << string(13 - strlen(row[1]), ' ');

			cout << "| " << row[2] << endl;
		}


		// Liberar memória do resultado
		mysql_free_result(res);
	}

}
/*
void selecionarProduto(MYSQL& conn, MYSQL_ROW& row, int id) {

	MYSQL_RES* res;
	string query = "SELECT * FROM produto WHERE id = '" + to_string(id) + "'";
	if (mysql_query(&conn, query.c_str()) == 0) {
		cout << "Erro ao executar a consulta: " << mysql_error(&conn) << endl;
	}
	else {
		res = mysql_use_result(&conn);
		while ((row = mysql_fetch_row(res)) != NULL) {
			cout << "ID    | NOME      | PRECO" << endl;
			cout << "------+-----------+---------" << endl;
			cout << row[0];
			cout << string(6 - strlen(row[0]), ' ');

			cout << "| " << row[1];
			cout << string(13 - strlen(row[1]), ' ');

			cout << "| " << row[2] << endl;
		}
		mysql_free_result(res);
	}

}
*/

void selecionarProduto(MYSQL& conn, MYSQL_ROW& row, int id) {
	MYSQL_RES* res;
	string query = "SELECT id, nome, preco FROM produto WHERE id = '" + to_string(id) + "'";
	if (mysql_query(&conn, query.c_str()) != 0) {
		cout << "Erro ao executar a consulta: " << mysql_error(&conn) << endl;
		return;
	}
	res = mysql_use_result(&conn);
	if (row = mysql_fetch_row(res)) {
		cout << "ID    | NOME         | PRECO" << endl;
		cout << "------+--------------+---------" << endl;
		cout << row[0];
		cout << string(6 - strlen(row[0]), ' ');
		cout << "| " << row[1];
		cout << string(13 - strlen(row[1]), ' ');
		cout << "| " << row[2] << endl;
	}
	else {
		cout << "id inexistente" << endl;
	}
	mysql_free_result(res);
}


void telaInicial(char& escolha) {
	cout << "-------------------------------------------" << endl;
	cout << "- Digite 1 para adicionar um produto      -" << endl;
	cout << "- Digite 2 para deletar um produto        -" << endl;
	cout << "- Digite 3 para selecionar um produto     -" << endl;
	cout << "- Digite 4 para atualizar um produto      -" << endl;
	cout << "- Digite 5 para mostrar todos os produtos -" << endl;
	cout << "- Digite . para parar                     -" << endl;
	cout << "-------------------------------------------" << endl;

	cout << "Sua escolha: ";
	cin >> escolha;
}

int main() {
	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES* res;

	conn = mysql_init(0);

	conn = mysql_real_connect(conn, "localhost", "root", "Emizin?31052005", "teste1", 3306, NULL, 0);

	char escolha;
	telaInicial(escolha);

	cout << endl;
	while (escolha != '.') {
		switch (escolha) {
		case '1': {
			cout << "--ADICIONAR PRODUTO SELECIONADA--" << endl;
			adicionarProduto(*conn);
			break;
		}
		case '2': {
			int idProdutoProcurado;
			cout << "Digite o id do produto que queira deletar: ";
			cin >> idProdutoProcurado;
			excluirProduto(*conn, idProdutoProcurado);
			break;
		}
		case '3': {
			int idProdutoProcurado;
			cout << "Digite o id do produto que queira mostrar: ";
			cin >> idProdutoProcurado;
			selecionarProduto(*conn, row, idProdutoProcurado);
			break;
		}
		case '4': {
			int idProdutoAtualizado;
			cout << "Digite o id do produto que queira atualizar: ";
			cin >> idProdutoAtualizado;
			atualizaProduto(*conn, idProdutoAtualizado);
			break;
		}
		case '5': {
			cout << "ID    | NOME         | PRECO" << endl;
			cout << "------+--------------+---------" << endl;
			selecionarTodosProdutos(*conn, row);
			break;
		}
		
		}
		cout << "-------------------------------------------" << endl;
		cout << "Sua escolha: ";
		cin >> escolha;
		cout << "-------------------------------------------" << endl;
	}

	return 0;
}