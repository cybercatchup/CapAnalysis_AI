#!/bin/bash
set -e

# Configure database.php
if [ -f app/Config/database.php_postgres ]; then
    cp app/Config/database.php_postgres app/Config/database.php

    # Use environment variables if set, otherwise default to linked service 'db'
    DB_HOST=${DB_HOST:-db}
    DB_PORT=${DB_PORT:-5432}
    DB_USER=${DB_USER:-capana}
    DB_PASS=${DB_PASS:-securepassword}
    DB_NAME=${DB_NAME:-capanalysis}

    sed -i "s/'host' => 'localhost'/'host' => '$DB_HOST'/" app/Config/database.php
    sed -i "s/'port' => '5432'/'port' => '$DB_PORT'/" app/Config/database.php
    sed -i "s/'login' => 'capana'/'login' => '$DB_USER'/" app/Config/database.php
    sed -i "s/'password' => '123456'/'password' => '$DB_PASS'/" app/Config/database.php
    sed -i "s/'database' => 'capanalysis'/'database' => '$DB_NAME'/" app/Config/database.php
fi

# Ensure tmp directories are writable
mkdir -p app/tmp/cache/models app/tmp/cache/persistent app/tmp/cache/views app/tmp/logs app/tmp/sessions
chmod -R 777 app/tmp

exec "$@"
