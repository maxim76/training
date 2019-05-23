#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifdef _WIN32
#pragma comment(lib,"sqlite3.lib")
#endif

extern "C" {
#include "third_party/sqlite/sqlite3.h"
}


static int callback(void *data, int argc, char **argv, char **azColName) {
	int i;
	fprintf(stderr, "%s: ", (const char*)data);

	for(i = 0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int main(int argc, char* argv[]) {
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	std::string sql;

	// OPEN DATABASE
	printf( "OPEN DATABASE\n" );
	rc = sqlite3_open("test.db", &db);

	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	} else {
		fprintf(stdout, "Opened database successfully\n");
	}

	// CREATE TABLE
	printf( "CREATE TABLE\n" );
	sql = "CREATE TABLE COMPANY("  \
		"ID INT PRIMARY KEY     NOT NULL," \
		"NAME           TEXT    NOT NULL," \
		"AGE            INT     NOT NULL," \
		"ADDRESS        CHAR(50)," \
		"SALARY         REAL );";

	rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Table created successfully\n");
	}

	// INSERT RECORDS
	printf( "INSERT RECORDS\n" );
	sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
		"VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
		"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
		"VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
		"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
		"VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
		"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
		"VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

	rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Records created successfully\n");
	}

	// SELECT RECORDS
	printf( "SELECT RECORDS\n" );
	sql = "SELECT * from COMPANY";
	const char* data = "Callback function called";

	rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);

	if( rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Operation done successfully\n");
	}

	// UPDATE RECORDS
	printf( "UPDATE RECORDS\n" );
	sql = "UPDATE COMPANY set SALARY = 25000.00 where ID=1; " \
		"SELECT * from COMPANY";

	rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);

	if( rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Operation done successfully\n");
	}

	// DELETE RECORDS
	printf( "DELETE RECORDS\n" );
	sql = "DELETE from COMPANY where ID=2; " \
		"SELECT * from COMPANY";

	rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);

	if( rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Operation done successfully\n");
	}

	sqlite3_close(db);
	return 0;
}
