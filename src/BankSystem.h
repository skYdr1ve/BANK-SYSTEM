#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "User.h"
#include "sqlite3.h"
#include "Operator.h"
#include "Client.h"
#include "Administrator.h"

#define MAX_QUERY_LENGTH 1000
#define FIELD_LENGTH 20
#define COM 0.05
#define CLS system("cls") 
#define EXIT exit(0)
#define OVER 150

extern char* types[2];