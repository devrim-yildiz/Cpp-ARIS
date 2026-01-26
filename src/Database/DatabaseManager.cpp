//
// Created by Devrim Yildiz on 26.01.26.
//

#include "Database/DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& dbPath)
    : m_db(nullptr)
    , m_dbPath(dbPath)
{
}

DatabaseManager::~DatabaseManager() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool DatabaseManager::Initialize() {
    int result = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (result != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    std::cout << "Database opened successfully: " << m_dbPath << std::endl;
    return CreateTables();
}

bool DatabaseManager::CreateTables() {
    // Create inventory table
    std::string inventorySQL = R"(
        CREATE TABLE IF NOT EXISTS inventory (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            quantity INTEGER NOT NULL,
            location TEXT NOT NULL,
            last_updated TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";
    
    if (!ExecuteSQL(inventorySQL)) {
        return false;
    }
    
    // Create tasks table
    std::string tasksSQL = R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            robot_id INTEGER NOT NULL,
            task_type TEXT NOT NULL,
            target TEXT NOT NULL,
            status TEXT DEFAULT 'pending',
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            completed_at TIMESTAMP
        );
    )";
    
    if (!ExecuteSQL(tasksSQL)) {
        return false;
    }
    
    // Create robot activity log table
    std::string activitySQL = R"(
        CREATE TABLE IF NOT EXISTS robot_activity (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            robot_id INTEGER NOT NULL,
            action TEXT NOT NULL,
            position_x INTEGER,
            position_y INTEGER,
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";
    
    if (!ExecuteSQL(activitySQL)) {
        return false;
    }
    
    std::cout << "Database tables created successfully" << std::endl;
    return true;
}

bool DatabaseManager::ExecuteSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int result = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (result != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool DatabaseManager::AddInventoryItem(const std::string& name, int quantity, const std::string& location) {
    sqlite3_stmt* stmt;
    std::string sql = "INSERT OR REPLACE INTO inventory (name, quantity, location) VALUES (?, ?, ?);";
    
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, quantity);
    sqlite3_bind_text(stmt, 3, location.c_str(), -1, SQLITE_TRANSIENT);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool DatabaseManager::UpdateInventoryQuantity(const std::string& name, int quantity) {
    sqlite3_stmt* stmt;
    std::string sql = "UPDATE inventory SET quantity = ?, last_updated = CURRENT_TIMESTAMP WHERE name = ?;";
    
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, quantity);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool DatabaseManager::RemoveInventoryItem(const std::string& name) {
    sqlite3_stmt* stmt;
    std::string sql = "DELETE FROM inventory WHERE name = ?;";
    
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool DatabaseManager::CreateTask(int robotId, const std::string& taskType, const std::string& target) {
    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO tasks (robot_id, task_type, target) VALUES (?, ?, ?);";
    
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, robotId);
    sqlite3_bind_text(stmt, 2, taskType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, target.c_str(), -1, SQLITE_TRANSIENT);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool DatabaseManager::CompleteTask(int taskId) {
    std::string sql = "UPDATE tasks SET status = 'completed', completed_at = CURRENT_TIMESTAMP WHERE id = " +
                     std::to_string(taskId) + ";";
    return ExecuteSQL(sql);
}

int DatabaseManager::GetPendingTaskCount() {
    sqlite3_stmt* stmt;
    std::string sql = "SELECT COUNT(*) FROM tasks WHERE status = 'pending';";
    
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int DatabaseManager::GetCompletedTaskCount() {
    sqlite3_stmt* stmt;
    std::string sql = "SELECT COUNT(*) FROM tasks WHERE status = 'completed';";
    
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

bool DatabaseManager::LogRobotActivity(int robotId, const std::string& action, int x, int y) {
    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO robot_activity (robot_id, action, position_x, position_y) VALUES (?, ?, ?, ?);";
    
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, robotId);
    sqlite3_bind_text(stmt, 2, action.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, x);
    sqlite3_bind_int(stmt, 4, y);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

int DatabaseManager::GetTotalInventoryItems() {
    sqlite3_stmt* stmt;
    std::string sql = "SELECT COUNT(*) FROM inventory;";
    
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int DatabaseManager::GetTotalTasks() {
    sqlite3_stmt* stmt;
    std::string sql = "SELECT COUNT(*) FROM tasks;";
    
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}
