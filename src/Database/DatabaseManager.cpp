#include "Database/DatabaseManager.h"
#include <iostream>
#include <ctime>

namespace {
    constexpr int GRID_SAVE_ID = 1;
}

DatabaseManager::DatabaseManager(const std::string& dbPath)
    : m_db(nullptr), m_dbPath(dbPath), m_logStmt(nullptr), m_statsStmt(nullptr)
{
}

DatabaseManager::~DatabaseManager() {
    Close();
}

bool DatabaseManager::Initialize() {
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    if (!CreateTables()) {
        std::cerr << "Failed to create tables" << std::endl;
        return false;
    }
    
    const char* logSQL = "INSERT INTO simulation_logs (Timestamp, EventType, Details) VALUES (datetime('now'), ?, ?)";
    if (sqlite3_prepare_v2(m_db, logSQL, -1, &m_logStmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare log statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    const char* statsSQL = "INSERT OR REPLACE INTO robot_stats (RobotID, TotalDistance, TasksCompleted) VALUES (?, ?, ?)";
    if (sqlite3_prepare_v2(m_db, statsSQL, -1, &m_statsStmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare stats statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    std::cout << "Database initialized: " << m_dbPath << std::endl;
    return true;
}

bool DatabaseManager::CreateTables() {
    const char* createLogsTable = R"(
        CREATE TABLE IF NOT EXISTS simulation_logs (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Timestamp TEXT NOT NULL,
            EventType TEXT NOT NULL,
            Details TEXT
        );
    )";
    
    const char* createStatsTable = R"(
        CREATE TABLE IF NOT EXISTS robot_stats (
            RobotID INTEGER PRIMARY KEY,
            TotalDistance REAL DEFAULT 0,
            TasksCompleted INTEGER DEFAULT 0
        );
    )";
    
    const char* createGridTable = R"(
        CREATE TABLE IF NOT EXISTS grid_data (
            ID INTEGER PRIMARY KEY,
            Width INTEGER,
            Height INTEGER,
            CellData BLOB
        );
    )";
    
    return ExecuteSQL(createLogsTable) && 
           ExecuteSQL(createStatsTable) && 
           ExecuteSQL(createGridTable);
}

bool DatabaseManager::ExecuteSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

void DatabaseManager::LogEvent(const std::string& eventType, const std::string& details) {
    if (!m_logStmt) return;
    
    sqlite3_bind_text(m_logStmt, 1, eventType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(m_logStmt, 2, details.c_str(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(m_logStmt) != SQLITE_DONE) {
        std::cerr << "Failed to log event: " << sqlite3_errmsg(m_db) << std::endl;
    }
    
    sqlite3_reset(m_logStmt);
}

void DatabaseManager::UpdateRobotStats(int robotId, float distance, int tasksCompleted) {
    if (!m_statsStmt) return;
    
    sqlite3_bind_int(m_statsStmt, 1, robotId);
    sqlite3_bind_double(m_statsStmt, 2, distance);
    sqlite3_bind_int(m_statsStmt, 3, tasksCompleted);
    
    if (sqlite3_step(m_statsStmt) != SQLITE_DONE) {
        std::cerr << "Failed to update robot stats: " << sqlite3_errmsg(m_db) << std::endl;
    }
    
    sqlite3_reset(m_statsStmt);
}

bool DatabaseManager::SaveGrid(const Grid& grid) {
    const char* sql = "INSERT OR REPLACE INTO grid_data (ID, Width, Height, CellData) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, GRID_SAVE_ID);
    sqlite3_bind_int(stmt, 2, grid.GetWidth());
    sqlite3_bind_int(stmt, 3, grid.GetHeight());
    
    std::vector<uint8_t> cellData;
    for (int y = 0; y < grid.GetHeight(); ++y) {
        for (int x = 0; x < grid.GetWidth(); ++x) {
            cellData.push_back(static_cast<uint8_t>(grid.GetCell(x, y)));
        }
    }
    
    sqlite3_bind_blob(stmt, 3, cellData.data(), cellData.size(), SQLITE_TRANSIENT);
    
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    
    return success;
}

bool DatabaseManager::LoadGrid(Grid& grid) {
    const char* sql = "SELECT Width, Height, CellData FROM grid_data WHERE ID = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, GRID_SAVE_ID);
    
    bool success = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int width = sqlite3_column_int(stmt, 0);
        int height = sqlite3_column_int(stmt, 1);
        const void* blobData = sqlite3_column_blob(stmt, 2);
        int blobSize = sqlite3_column_bytes(stmt, 2);
        
        grid = Grid(width, height);
        const uint8_t* cellData = static_cast<const uint8_t*>(blobData);
        
        for (int i = 0; i < blobSize && i < width * height; ++i) {
            int x = i % width;
            int y = i / width;
            grid.SetCell(x, y, static_cast<CellType>(cellData[i]));
        }
        
        success = true;
    }
    
    sqlite3_finalize(stmt);
    return success;
}

void DatabaseManager::Close() {
    if (m_logStmt) {
        sqlite3_finalize(m_logStmt);
        m_logStmt = nullptr;
    }
    
    if (m_statsStmt) {
        sqlite3_finalize(m_statsStmt);
        m_statsStmt = nullptr;
    }
    
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}
