#include <iostream>
#include <sqlite3.h>
#include <string>
#include <algorithm>


using namespace std;

class SQLiteDB {
public:
    SQLiteDB(const std::string& dbName) {
        this->dbName = dbName;
        db = nullptr;
    }

    ~SQLiteDB() {
        if (db) {
            sqlite3_close(db);
        }
    }

    bool connectOrCreate() {
        // Проверяем, что имя файла оканчивается на ".db"
        if (dbName.size() < 3 || dbName.substr(dbName.size() - 3) != ".db") {
            std::cerr << "Имя файла должно оканчиваться на .db" << std::endl;
            return false;
        }

        int res = sqlite3_open(dbName.c_str(), &db);
        if (res != SQLITE_OK) {
            std::cerr << "Не удалось открыть " << dbName << " Ошибка: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        std::cout << "Соединение успешно!" << std::endl;
        return true;
    }

    sqlite3* getDB() {
        return db;
    }

private:
    sqlite3* db;
    std::string dbName;

    // Функция срабатывает при SELECT
    static int selectCallback(void* NotUsed, int argc, char** argv, char** azColName) {
        for (int i = 0; i < argc; i++) {
            std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
        }
        std::cout << std::endl;
        return 0;
    }

    // Позволяет функции accomplishment доступ к приватным членам класса
    friend void accomplishment(SQLiteDB& db);
};

// выполнение SQL-запросов
void accomplishment(SQLiteDB& dbInstance) {
    sqlite3* db = dbInstance.getDB();

    while (true) {
        std::string sqlStatement;
        std::cout << "Введите команду или close чтобы закрыть программу." << std::endl;
        std::getline(std::cin, sqlStatement);

        // Приведение команды к нижнему регистру
        std::transform(sqlStatement.begin(), sqlStatement.end(), sqlStatement.begin(), ::tolower);

        if (sqlStatement == "close") {
            break; // Выход из цикла, если пользователь ввел "close"
        }

        char* errorMessage = nullptr;
        int rc = sqlite3_exec(db, sqlStatement.c_str(), SQLiteDB::selectCallback, nullptr, &errorMessage);
        if (rc != SQLITE_OK && !sqlStatement.empty()) {
            std::cerr << "Ошибка выполнения SQL-команды: " << errorMessage << std::endl;
            sqlite3_free(errorMessage); // Освобождение памяти, выделенной для errorMessage
        }
        else {
            std::cout << "SQL-команда успешно выполнена" << std::endl;
        }
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    std::string dbName;
    std::cout << "Введите название Базы данных: " << std::endl;
    std::getline(std::cin, dbName);

    SQLiteDB db(dbName);
    if (!db.connectOrCreate()) {
        std::cout << "Не получилось подключиться к БД" << std::endl;
        return -1;
    }

    accomplishment(db);

    return 0;
}
