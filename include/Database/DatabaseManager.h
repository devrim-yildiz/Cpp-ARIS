//
// Created by Devrim Yildiz on 26.01.26.
//

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <sqlite3.h>

// Database Manager for inventory and order tracking
class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();
    
    bool Initialize();
    bool IsOpen() const { return m_db != nullptr; }
    
    // Inventory operations
    bool AddInventoryItem(const std::string& name, int quantity, const std::string& location);
    bool UpdateInventoryQuantity(const std::string& name, int quantity);
    bool RemoveInventoryItem(const std::string& name);
    
    // Task/Order operations
    bool CreateTask(int robotId, const std::string& taskType, const std::string& target);
    bool CompleteTask(int taskId);
    int GetPendingTaskCount();
    int GetCompletedTaskCount();
    
    // Robot activity logging
    bool LogRobotActivity(int robotId, const std::string& action, int x, int y);
    
    // Statistics
    int GetTotalInventoryItems();
    int GetTotalTasks();

private:
    sqlite3* m_db;
    std::string m_dbPath;
    
    bool CreateTables();
    bool ExecuteSQL(const std::string& sql);
};
