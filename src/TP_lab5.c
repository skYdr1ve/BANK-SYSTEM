#include "BankSystem.h"

sqlite3 *db;
sqlite3_stmt *res;
char *err_msg = 0;
struct User currentUser;

extern int biChoice(char* firstOption, char* secondOption);
extern int getClientID(sqlite3*, int);
extern char* choose(char*, char **);

int autorizationCallback(void *data, int argc, char **argv, char **azColName) {
	if (strcmp(argv[0], "")) {
		strcpy(currentUser.firstname, argv[0]);
		strcpy(currentUser.lastname, argv[1]);
		strcpy(currentUser.login, argv[2]);
		strcpy(currentUser.type, argv[3]);
		currentUser.ID = atoi(argv[4]);
		return 0;
	}
	return -1;
}

int authorization(sqlite3* db, char* login, char* password) {
	char sql[MAX_QUERY_LENGTH];
	char *zErrMsg = 0;
	sprintf(sql, "SELECT first_name, last_name, login, user_type, id from BANK_USERS where login=\"%s\" AND password=\"%s\"", login, password);
	int rc = sqlite3_exec(db, sql, autorizationCallback, 0, &zErrMsg);
	return rc;
}

int chooseAccount(sqlite3* db, char* login) {
	showClientAccounts(db, login);
	int option;
	int n = getAccountCount(db, login);
	int* accounts = getAccounts(db, login);
	do {
		printf("Choose account\n");
		for (int i = 0; i < n; i++) {
			printf("%d - id: %d\n", i + 1, accounts[i]);
		}
		scanf("%d", &option);
	} while (option<1 || option>n);
	return accounts[option - 1];
}

double comm(double sum) {
	printf("Commission is %f. Continue?\n", sum*0.05);
	if (biChoice("Yes", "No")) {
		return sum * 0.05;
	}
	return -1;
}

int enterID() {
	int id;
	printf("Enter account id\n");
	scanf("%d", &id);
	return id;
}

void operatorMenu(sqlite3* db) {
	int option;
	do {
		printf("1 - top up balance\n");
		printf("2 - withdraw money\n");
		printf("3 - transacion\n");
		printf("4 - exit\n");
		printf("===========================\n");
		scanf("%d", &option);
		double c;
		double sum;
		int acc;
		int to;
		switch (option)
		{
		case 1:
			acc = enterID();
			printf("Enter sum\n");
			scanf("%lf", &sum);
			if ((c = comm(sum)) != -1) {
				credit(db, acc, sum - c);
			}
			break;
		case 2:
			acc = enterID();
			printf("Enter sum\n");
			scanf("%lf", &sum);
			if (debit(db, acc, sum, COM) == -1)
				printf("Failed\n");
			break;
		case 3:
			acc = enterID();
			to = enterID();
			printf("Enter sum\n");
			scanf("%lf", &sum);
			if (transaction(db, acc, to, sum, OVER) != 0) {
				printf("Failed");
			}
			break;
		}
		printf("===========================\n");
	} while (option != 4);

}

void clientMenu(sqlite3* db) {
	int option;
	do {
		printf("1 - show balance\n");
		printf("2 - top up your balance\n");
		printf("3 - withdraw money\n");
		printf("4 - exit\n");
		printf("===========================\n");
		scanf("%d",&option);
		double c;
		double sum;
		int acc;
		switch (option)
		{
		case 1:
			showClientAccounts(db, currentUser.login);
			break;
		case 2:
			printf("===========================\n");
			acc = chooseAccount(db, currentUser.login);
			printf("===========================\n");
			printf("Enter sum\n");
			scanf("%lf", &sum);
			printf("%lf ", sum);
			if ((c = comm(sum)) != -1) {
				if (credit(db, acc, sum - c) == 0) {
					printf("Money was credited\n");
				}
			}
			break;
		case 3:
			acc = chooseAccount(db, currentUser.login);
			printf("===========================\n");
			printf("Enter sum\n");
			scanf("%lf", &sum);
			printf("%lf %d ", sum, acc);
			if ((c = comm(sum)) != -1) {
				if (debit(db, acc, sum + c, OVER) == 0)
					printf("Money was withdraw\n");
			}
			break;
		}
		printf("===========================\n");
	} while (option != 4);
}

void mainMenu(sqlite3* db) {
	char login[FIELD_LENGTH];
	char password[FIELD_LENGTH];
	printf("Enter login\n");
	scanf("%s", login);
	printf("Enter password\n");
	scanf("%s", password);
	authorization(db, login, password);
	CLS;
	printf("============%s===========\n", currentUser.type);
	printf("===========================\n");
	if (!strcmp(currentUser.type, "CLIENT"))
		clientMenu(db);
	if (!strcmp(currentUser.type, "OPERATOR"))
		operatorMenu(db);
	if (!strcmp(currentUser.type, "ADMINISTRATOR"))
		administratorMenu(db);
}

int main()
{
	int choice = 0;
	int rc = sqlite3_open("BANK.db", &db);
	if (rc != SQLITE_OK) {

		fprintf(stderr, "Cannot open database: %s\n",
			sqlite3_errmsg(db));
		sqlite3_close(db);

		return 1;
	}
	mainMenu(db);

	if (rc != SQLITE_OK) {
		printf("2222");
		sqlite3_close(db);
		return 1;
	}
	sqlite3_close(db);
	return 0;
}