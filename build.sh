#!/bin/bash

DB_NAME="taskm.db"
SQL_FILE="bootup.sql"

# Check if SQLite CLI is installed
if ! command -v sqlite3 &> /dev/null
then
    echo "SQLite3 is not installed. Please install it first."
    exit 1
fi

# Create or update the database
if [ -f "$SQL_FILE" ]; then
    sqlite3 "$DB_NAME" < "$SQL_FILE"
    echo "Database initialized successfully!"
else
    echo "SQL file $SQL_FILE not found!"
    exit 1
fi

# Optional: Output tables for verification
sqlite3 "$DB_NAME" .tables