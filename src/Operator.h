#pragma once
#include "sqlite3.h"


int changeBalance(sqlite3* db, int accountID, double value);
int transaction(sqlite3* db, int fromID, int toID, double sum, double maxOverdrive);
int debit(sqlite3*, int, double, double);
int credit(sqlite3* db, int accountID, double value);