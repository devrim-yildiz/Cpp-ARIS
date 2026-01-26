#pragma once
#include <string>
#include <sqlite3.h>
#include "Simulation/Grid.h"

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath = "aris_simulation.db");
    ~DatabaseManager();
    
    bool Initialize();
    
    void LogEvent(const std::string& eventType, const std::string& details);
    void UpdateRobotStats(int robotId, float distance, int tasksCompleted);
    
    bool SaveGrid(const Grid& grid);
    bool LoadGrid(Grid& grid);
    
    void Close();

private:
    bool CreateTables();
    bool ExecuteSQL(const std::string& sql);

private:
    sqlite3* m_db;
    std::string m_dbPath;
    sqlite3_stmt* m_logStmt;
    sqlite3_stmt* m_statsStmt;
};
