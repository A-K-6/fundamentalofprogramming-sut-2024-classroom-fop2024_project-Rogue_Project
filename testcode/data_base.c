#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

/* Function to open the database and create the table if it doesn't exist */
int createDatabase(sqlite3 **db) {
    int rc = sqlite3_open("user_data.db", db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }

    // SQL command to create the users table
    const char *sqlCreateTable =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "password TEXT NOT NULL, "
        "email TEXT NOT NULL"
        ");";

    char *errMsg = NULL;
    rc = sqlite3_exec(*db, sqlCreateTable, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create table: %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(*db);
        return rc;
    }
    return SQLITE_OK;
}

/* Function to insert a user into the database */
int insertUser(sqlite3 *db, const char *username, const char *password, const char *email) {
    // Use a parameterized query to avoid SQL injection
    const char *sqlInsert = "INSERT INTO users (username, password, email) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    
    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, sqlInsert, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    
    // Bind the parameters to the statement
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, email, -1, SQLITE_STATIC);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    } else {
        printf("User inserted successfully!\n");
    }

    // Finalize the statement to release resources
    sqlite3_finalize(stmt);
    return rc;
}

int main(void) {
    sqlite3 *db;
    int rc = createDatabase(&db);
    if (rc != SQLITE_OK) {
        return rc;
    }

    // Example: Insert a user. Replace these strings with real user input as needed.
    rc = insertUser(db, "testuser", "password123", "test@example.com");
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert user.\n");
    }

    // Always close the database when done
    sqlite3_close(db);
    return 0;
}
