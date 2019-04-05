#include "BankSystem.h"

sqlite3* db;
sqlite3_stmt *res;
char* types[2] = { "CHECKING","SAVINGS" };

int lastUserId = 0;

int callback(void *data, int argc, char **argv, char **azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}

struct User enterUser() {
	struct User newUser;
	printf("Enter firstname \n");
	scanf("%s", newUser.firstname);
	printf("Enter lastname\n");
	scanf("%s", newUser.lastname);
	printf("Enter login\n");
	scanf("%s", newUser.login);
	printf("Enter password\n");
	scanf("%s", newUser.password);
	return newUser;
}

int printAllConfigsCallBack(void *data, int argc, char **argv, char **azColName) {
	int counter = 0;
	while (argv[counter]) {
		printf("%s : %s   |   ", azColName[counter], argv[counter]);
		counter++;
	}
	printf("\n");
	return 0;
}

void printAllConfigTypes() {
	char sql[255];
	char *zErrMsg = 0;
	int rc = 0;
	printf("Clients:\n");
	rc = sqlite3_exec(db, "SELECT * FROM BANK_CONFIG", printAllConfigsCallBack, 0, &zErrMsg);
	if (rc) {
		printf("Error message:   %s", zErrMsg);
	}
}

int printAllUsersCallBack(void *data, int argc, char **argv, char **azColName) {
	int counter = 1;
	while (argv[counter]) {
		printf("%s ", argv[counter++]);
	}
	printf("\n");
	return 0;
}

int printAllClientsCallBack(void *data, int argc, char **argv, char **azColName) {
	printf("%s ", argv[0]);
	char *zErrMsg = 0;
	char sql[255];
	sprintf(sql, "SELECT first_name, last_name FROM BANK_USERS WHERE id = %s", argv[1]);
	int rc = 0;
	rc = sqlite3_exec(db, sql, printAllUsersCallBack, 0, &zErrMsg);
	if (rc) {
		printf("Error message:   %s", zErrMsg);
	}
	return 0;
}

void printAllAccountsCallBack(void *data, int argc, char **argv, char **azColName) {
	printf("id: %s\n", argv[0]);
	printf("amount: %s\n", argv[1]);
	char *zErrMsg = 0;
	char sql[255];
	sprintf(sql, "SELECT * FROM BANK_USERS WHERE id = (SELECT user_id FROM BANK_CLIENTS WHERE id = %s)", argv[2]);
	int rc = 0;
	printf("client:   ");
	rc = sqlite3_exec(db, sql, printAllUsersCallBack, 0, &zErrMsg);
	if (rc) {
		printf("Error message:   %s", zErrMsg);
	}
	sql[0] = "\0";
	sprintf(sql, "SELECT * FROM BANK_CONFIG WHERE id = %s", argv[3]);
	printf("account:   ");
	rc = sqlite3_exec(db, sql, printAllConfigsCallBack, 0, &zErrMsg);
	if (rc) {
		printf("Error message:   %s", zErrMsg);
	}
	printf("\n");
}

void printAllAccounts() {
	char sql[255];
	char *zErrMsg = 0;
	int rc = 0;
	printf("Accounts:\n");
	rc = sqlite3_exec(db, "SELECT * FROM BANK_ACCOUNTS", printAllAccountsCallBack, 0, &zErrMsg);
	if (rc) {
		printf("Error message:   %s", zErrMsg);
	}
}

void printAllClients() {
	char sql[255];
	char *zErrMsg = 0;
	int rc = 0;
	printf("Clients:\n");
	rc = sqlite3_exec(db, "SELECT * FROM BANK_CLIENTS", printAllClientsCallBack, 0, &zErrMsg);
	if (rc) {
		printf("Error message:   %s", zErrMsg);
	}
}

void openAccount(char numberClient[], char numberAccount[]) {
	printf("%s %s\n", numberClient, numberAccount);
	char amount[15];
	printf("Choose the amount of money for the account");
	scanf("%s", amount);
	char sql[255];
	char *zErrMsg = 0;
	int rc = 0;
	sprintf(sql, "INSERT INTO BANK_ACCOUNTS (amount, client_id, account_type) VALUES (\"%s\", \"%s\", \"%s\")", amount, numberClient, numberAccount);
	rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	if (rc) {
		printf("Invalid data. Please try again\n");
	}
}

char* choose(char* buttons, char** values) {
	int length = strlen(buttons);
	char c = '0';
	char* cp;
	do {
		for (int i = 0; i < length; i++)
			printf("%c - %s\n", buttons[i], values[i]);
		while (getchar() != '\n');
		c = getchar();
	} while ((cp = strchr(buttons, c)) == NULL);
	return values[cp - buttons];
}

int biChoice(char* firstOption, char* secondOption) {
	char c;
	do {
		printf("%c - %s\n", '1', firstOption);
		printf("%c - %s\n", '2', secondOption);
		while (getchar() != '\n');
		c = getchar();
	} while (c != '1' && c != '2');
	return c == '1';
}

int addClientCallBack(void *data, int argc, char **argv, char **azColName) {
	if (strcmp(argv[0], "")) {
		lastUserId = atoi(argv[0]);
		return 0;
	}
	return -1;
}

int addClient() {
	char sql[255];
	char *zErrMsg = 0;
	int rc = 0;
	rc = sqlite3_exec(db, "SELECT * FROM BANK_USERS ORDER BY id DESC LIMIT 1;", addClientCallBack, 0, &zErrMsg);
	if (rc) {
		printf("Error message:   %s", zErrMsg);
	}
	sql[0] = '\0';
	sprintf(sql, "INSERT INTO BANK_CLIENTS (user_id) VALUES (%d)", lastUserId);
	printf("%s\n", sql);
	zErrMsg = 0;
	rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	if (rc) {
		printf("Error message:   %s", zErrMsg);
	}
	return rc;
}

int addUser(struct User newClient, char* type) {
	char sql[226];
	char *zErrMsg = 0;
	int command;
	printf("Add a photo?(1/0)\n");
	scanf("%d", &command);
	printf("%s %s %s %s %s", newClient.firstname, newClient.lastname, newClient.login, newClient.password, type);
	

	while (command != 0 && command != 1) {
		printf("Select either 1 or 0\n");
		command = getchar();
	}

	if (command == 0) {
		sprintf(sql, "INSERT INTO BANK_USERS (first_name, last_name, login, password, user_type, user_photo) VALUES ( \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\");", newClient.firstname, newClient.lastname, newClient.login, newClient.password, type, "");

		int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

		if (rc != 0) 
			printf("\nError message:   %s", zErrMsg);
		return rc;
	}
	else
	{
		printf("hi");
		sprintf(sql, "INSERT INTO BANK_USERS (first_name, last_name, login, password, user_type, user_photo) VALUES ( \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", ?);", newClient.firstname, newClient.lastname, newClient.login, newClient.password, type, "");
		if (sqlite3_prepare_v2(db, sql, -1, &res, 0))
			fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
		char* filename = (char*)malloc(256);
		printf("Enter name of file(or path): ");
		scanf("%s", filename);
		FILE* f = fopen(filename, "rb");
		while (f == NULL) {
			printf("Wrong file name!\nEnter name of file(or path): ");
			scanf("%s", filename);
			f = fopen(filename, "rb");
		}

		fseek(f, 0, SEEK_END);
		int flen = ftell(f);
		fseek(f, 0, SEEK_SET);
		char* binPhoto = (char*)malloc(flen + 1);
		int size = fread(binPhoto, 1, flen, f);
		fclose(f);
		sqlite3_bind_blob(res, 1, binPhoto, size, SQLITE_STATIC);
		int k = sqlite3_step(res);
		sqlite3_finalize(res);
		free(binPhoto);
		return 0;
	}
	return 0;
}

void setAccountType(char newType[], char accNumber[]) {
	char sql[226];
	char *zErrMsg = 0;
	sprintf(sql, "UPDATE BANK_ACCOUNTS SET account_type = %s WHERE id = %s", newType, accNumber);
	int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	if (rc != 0) {
		printf("\nError message:   %s", zErrMsg);
	}
}

void closeAccount(char accountNumber[]) {
	char sql[226];
	char *zErrMsg = 0;
	sprintf(sql, "DELETE FROM BANK_ACCOUNTS WHERE id = %s", accountNumber);
	int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	if (rc != 0) {
		printf("\nError message:   %s", zErrMsg);
	}
}


void setOverdraft(int choise, char overdraftAmount[], char accountId[]) {
	char sql[226];
	char *zErrMsg = 0;
	char symbol;
	if (choise) {
		symbol = '-';
	}
	else {
		symbol = '+';
	}
	sprintf(sql, "UPDATE BANK_ACCOUNTS SET amount = (SELECT amount %c %s  FROM BANK_ACCOUNTS WHERE id = %s) WHERE id = %s", symbol, overdraftAmount, accountId, accountId);
	int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	if (rc != 0) {
		printf("\nError message:   %s", zErrMsg);
	}
}

int administratorMenu(sqlite3* ldb) {
	db = ldb;
	char option;
	do {
		printf("1 - add user\n");
		printf("2 - add client\n");
		printf("3 - add account\n");
		printf("4 - close account\n");
		printf("5 - add/remove overdraft\n");
		printf("6 - change type of account\n");
		printf("7 - exit\n");
		printf("===========================\n");
		while (getchar() != '\n');
		option = getchar();
		int acc;
		int id;
		char* position[] = { "CLIENT","OPERATOR","ADMINISTRATOR" };
		char accountNumber[10];
		switch (option)
		{
		case '1':
			addUser(enterUser(), choose("12", position));
			break;
		case '2':
			addUser(enterUser(), choose("12", position));
			addClient();
			break;
		case '3':
			printAllClients();
			printf("Choose the client by the id number\n");
			char numberClient[10];
			scanf("%s", numberClient);
			printAllConfigTypes();

			printf("Choose the account type by the id number\n");
			char numberAccount[10];
			scanf("%s", numberAccount);
			openAccount(numberClient, numberAccount);
			break;
		case '4':
			printAllAccounts();
			printf("Choose the account number by id to close it\n");
			scanf("%s", accountNumber);
			closeAccount(accountNumber);
			break;
		case '5':
			printAllAccounts();
			printf("Choose the account number by id to close it:\n");
			scanf("%s", accountNumber);
			printf("Add or Remove overdraft?");
			int choise = biChoice("Add", "Remove");
			printf("Choose the overdraft amount\n");
			char overdraftAmmount[10];
			scanf("%s", overdraftAmmount);
			setOverdraft(choise, overdraftAmmount, accountNumber);
			break;
		case '6':
			printAllAccounts();
			printf("Select account by id: \n");
			scanf("%s", accountNumber);
			char newType[2];
			printAllConfigTypes();
			printf("Select account type by id: \n");
			scanf("%s", newType);
			setAccountType(newType, accountNumber);
			break;
		}
		printf("===========================\n");
	} while (option != '7');

}
