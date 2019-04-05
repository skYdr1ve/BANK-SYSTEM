#include "BankSystem.h"

extern int callback(void *data, int argc, char **argv, char **azColName);

int amount = 0;

int changeBalance(sqlite3* db, int accountID, double value) {
	char sql[MAX_QUERY_LENGTH];
	char* mes;
	sprintf(sql, "UPDATE BANK_ACCOUNTS SET amount=amount+%f where id=%d;", value, accountID);
	sqlite3_exec(db, sql, NULL, 0, &mes);
	if (mes != NULL) 
		printf("Error, try again\n");
	return 0;
}


int isAccountExist(sqlite3* db, int accountID) {
	sqlite3_stmt *res;
	sqlite3_prepare_v2(db, "SELECT * from BANK_ACCOUNTS where id=?;", -1, &res, 0);
	sqlite3_bind_int(res, 1, accountID);
	sqlite3_step(res);
	int isExist = sqlite3_data_count(res);
	sqlite3_finalize(res);
	return isExist;
}

double getBalance(sqlite3* db, int accountID) {
	sqlite3_stmt *res;
	sqlite3_prepare_v2(db, "SELECT balance from BANK_ACCOUNTS where ID=?;", -1, &res, 0);
	sqlite3_bind_int(res, 1, accountID);
	//если аккаунт существует возвращает SQLITE3_ROW
	double balance = sqlite3_column_double(res, 0);
	sqlite3_finalize(res);
	return balance;
}

int callbackamount(void *data, int argc, char **argv, char **azColName)
{
	if (strcmp(argv[0], "")) {
		amount = atoi(argv[0]);
		return 0;
	}
	return -1;
}

int canOverdraft(sqlite3* db, int accountID) {
	char sql[MAX_QUERY_LENGTH];
	char* mes;
	sprintf(sql, "SELECT amount from BANK_ACCOUNTS where client_id=%d;", accountID);
	sqlite3_exec(db, sql, callbackamount, 0, &mes);
	if (mes != NULL)
		printf("Error, try again\n");
	if (amount <= -OVER)
		return 0;
	else
		return 1;
}

double getBalanceSum(sqlite3* db, int clientID) {
	sqlite3_stmt *res;
	sqlite3_prepare_v2(db, "SELECT SUM(amount) from BANK_ACCOUNTS inner join BANK_CLIENTS on BANK_ACCOUNTS.client_id=BANK_CLIENTS.user_id where BANK_CLIENTS.user_id=?;", -1, &res, 0);
	sqlite3_bind_int(res, 1, clientID);
	if (sqlite3_step(res) != SQLITE_ROW)
		return -1;
	double overdraft = sqlite3_column_double(res, 0);
	sqlite3_finalize(res);
	return overdraft;
}

int getClientID(sqlite3* db, int accountID) {
	sqlite3_stmt *res;
	sqlite3_prepare_v2(db, "SELECT client_id from BANK_ACCOUNTS where id=?;", -1, &res, 0);
	sqlite3_bind_int(res, 1, accountID);
	sqlite3_step(res);//если аккаунт существует возвращает SQLITE3_ROW
	int ID = sqlite3_column_int(res, 0);
	sqlite3_finalize(res);
	return ID;
}

int debit(sqlite3* db, int accountID, double value, double maxOverdrive) {
	int userID = getClientID(db, accountID);
	if (value > 0 && isAccountExist(db, accountID)) {
		double balance = getBalanceSum(db, userID);
		if (balance >= value) {
			changeBalance(db, accountID, -value);
			return 0;
		}
		else {
			int a=(getBalanceSum(db, getClientID(db, accountID)) - value) >= -maxOverdrive;
			if (canOverdraft(db, accountID) && (getBalanceSum(db, getClientID(db, accountID)) - value) >= -maxOverdrive) {
				changeBalance(db, accountID, -value);
				return 0;
			}
		}
	}
	return -1;
}

int transaction(sqlite3* db, int fromID, int toID, double value, double maxOverdrive) {
	if (value > 0 && isAccountExist(db, toID) && debit(db, fromID, value, maxOverdrive) == 0) {
		changeBalance(db, toID, value);
		return 0;
	}
	return -1;
}

int credit(sqlite3* db, int accountID, double value) {
	if (value > 0 && isAccountExist(db, accountID)) {
		changeBalance(db, accountID, value);
		return 0;
	}
	return -1;
}