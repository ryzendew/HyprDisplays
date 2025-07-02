#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>

#include "displaymanager.h"

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();

    // Application settings
    bool loadApplicationSettings();
    bool saveApplicationSettings();
    void resetApplicationSettings();
    
    // Hyprland configuration
    bool loadHyprlandConfig();
    bool saveHyprlandConfig(const QString &path);
    bool loadHyprlandMonitors(const QString &path);
    bool saveHyprlandMonitors(const QString &path);
    bool loadHyprlandWorkspaces(const QString &path);
    bool saveHyprlandWorkspaces(const QString &path);
    
    // Display configuration
    bool loadDisplayConfig();
    bool saveDisplayConfig(const QString &path);
    QJsonObject getDisplayConfig() const;
    void setDisplayConfig(const QJsonObject &config);
    
    // Workspace configuration
    bool loadWorkspaceConfig();
    bool saveWorkspaceConfig(const QString &path);
    QJsonObject getWorkspaceConfig() const;
    void setWorkspaceConfig(const QJsonObject &config);
    
    // Backup and restore
    bool createBackup(const QString &path);
    bool restoreBackup(const QString &path);
    QStringList getBackups() const;
    bool deleteBackup(const QString &path);
    
    // Validation
    bool validateConfig(const QJsonObject &config);
    bool validateMonitorsConfig(const QJsonObject &config);
    bool validateWorkspacesConfig(const QJsonObject &config);
    QStringList getValidationErrors() const;
    
    // Import/Export
    bool importConfig(const QString &path);
    bool exportConfig(const QString &path);
    bool importFromNwgDisplays(const QString &path);
    bool exportToNwgDisplays(const QString &path);

public slots:
    void onSettingsChanged();
    void onConfigChanged();
    void onDisplayConfigChanged();
    void onWorkspaceConfigChanged();

signals:
    void settingsChanged();
    void configChanged();
    void displayConfigChanged();
    void workspaceConfigChanged();
    void backupCreated(const QString &path);
    void backupRestored(const QString &path);
    void backupDeleted(const QString &path);
    void error(const QString &message);
    void success(const QString &message);

private slots:
    void onFileChanged(const QString &path);
    void onDirectoryChanged(const QString &path);

private:
    // File operations
    bool readJsonFile(const QString &path, QJsonObject &json);
    bool writeJsonFile(const QString &path, const QJsonObject &json);
    bool readTextFile(const QString &path, QString &content);
    bool writeTextFile(const QString &path, const QString &content);
    
    // Configuration parsing
    bool parseHyprlandMonitorsConfig(const QString &content);
    bool parseHyprlandWorkspacesConfig(const QString &content);
    QString generateHyprlandMonitorsConfig(const QJsonObject &config);
    QString generateHyprlandWorkspacesConfig(const QJsonObject &config);
    
    // Path management
    QString getConfigPath() const;
    QString getSettingsPath() const;
    QString getBackupPath() const;
    QString getDefaultMonitorsPath() const;
    QString getDefaultWorkspacesPath() const;
    
    // Validation helpers
    bool validateDisplayInfo(const DisplayInfo &display);
    bool validateWorkspaceAssignment(const QString &workspace, const QString &monitor);
    bool validateMonitorName(const QString &name);
    bool validateResolution(const QString &resolution);
    bool validateRefreshRate(int refreshRate);
    bool validateScale(double scale);
    bool validateTransform(const QString &transform);
    bool validatePosition(int x, int y);
    
    // Conversion helpers
    QJsonObject displayInfoToJson(const DisplayInfo &display);
    DisplayInfo jsonToDisplayInfo(const QJsonObject &json);
    QJsonObject workspaceAssignmentToJson(const QString &workspace, const QString &monitor);
    QString jsonToWorkspaceAssignment(const QJsonObject &json);
    
    // Migration helpers
    bool migrateFromOldConfig();
    bool migrateFromNwgDisplays();
    bool migrateToNewFormat();
    
    // Settings
    QSettings *m_settings;
    QJsonObject m_applicationConfig;
    QJsonObject m_displayConfig;
    QJsonObject m_workspaceConfig;
    QJsonObject m_hyprlandConfig;
    
    // Paths
    QString m_configPath;
    QString m_settingsPath;
    QString m_backupPath;
    QString m_monitorsPath;
    QString m_workspacesPath;
    
    // State
    bool m_isLoading;
    bool m_isSaving;
    bool m_autoSave;
    bool m_autoBackup;
    int m_maxBackups;
    
    // Validation
    QStringList m_validationErrors;
    bool m_strictValidation;
    
    // File watchers
    QList<QString> m_watchedFiles;
    QList<QString> m_watchedDirectories;
    
    // Backup management
    QStringList m_backupFiles;
    QRegularExpression m_backupFileRegex;
    
    // Import/Export
    bool m_enableImportExport;
    QStringList m_supportedFormats;
    QJsonObject m_importExportConfig;
};

#endif // CONFIGMANAGER_H 