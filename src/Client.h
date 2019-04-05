#pragma once
#include "sqlite3.h"

int* getAccounts(sqlite3*, char*);
int getAccountCount(sqlite3* db, char* login);
int showClientAccounts(sqlite3* db, char* login);