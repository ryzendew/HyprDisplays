#include "configmanager.h"
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
    , m_isLoading(false)
    , m_isSaving(false)
    , m_autoSave(true)
    , m_autoBackup(true)
    , m_maxBackups(5)
    , m_strictValidation(true)
    , m_enableImportExport(true)
{
    // Initialize paths
    m_configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/HyprDisplays";
    m_settingsPath = m_configPath + "/settings.json";
    m_backupPath = m_configPath + "/backups";
    m_monitorsPath = QDir::homePath() + "/.config/hypr/monitors.conf";
    m_workspacesPath = QDir::homePath() + "/.config/hypr/workspaces.conf";
    
    // Create directories if they don't exist
    QDir().mkpath(m_configPath);
    QDir().mkpath(m_backupPath);
    
    // Initialize settings
    m_settings = new QSettings("HyprDisplays", "HyprDisplays", this);
    
    // Setup backup file regex
    m_backupFileRegex = QRegularExpression(R"(hyprdisplays_backup_\d{8}_\d{6}\.json)");
    
    // Setup supported formats
    m_supportedFormats = {"json", "conf", "txt"};
    
    // Load initial settings
    loadApplicationSettings();
}

ConfigManager::~ConfigManager()
{
    if (m_isSaving) {
        saveApplicationSettings();
    }
}

bool ConfigManager::loadApplicationSettings()
{
    if (m_isLoading) {
        return false;
    }
    
    m_isLoading = true;
    
    QFile file(m_settingsPath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        
        if (!doc.isNull()) {
            m_applicationConfig = doc.object();
            
            // Load settings from JSON
            m_autoSave = m_applicationConfig["autoSave"].toBool(true);
            m_autoBackup = m_applicationConfig["autoBackup"].toBool(true);
            m_maxBackups = m_applicationConfig["maxBackups"].toInt(5);
            m_strictValidation = m_applicationConfig["strictValidation"].toBool(true);
            m_enableImportExport = m_applicationConfig["enableImportExport"].toBool(true);
        }
    }
    
    m_isLoading = false;
    emit settingsChanged();
    return true;
}

bool ConfigManager::saveApplicationSettings()
{
    if (m_isSaving) {
        return false;
    }
    
    m_isSaving = true;
    
    // Update JSON config
    m_applicationConfig["autoSave"] = m_autoSave;
    m_applicationConfig["autoBackup"] = m_autoBackup;
    m_applicationConfig["maxBackups"] = m_maxBackups;
    m_applicationConfig["strictValidation"] = m_strictValidation;
    m_applicationConfig["enableImportExport"] = m_enableImportExport;
    
    QJsonDocument doc(m_applicationConfig);
    QFile file(m_settingsPath);
    
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        m_isSaving = false;
        emit settingsChanged();
        return true;
    }
    
    m_isSaving = false;
    emit error("Failed to save application settings");
    return false;
}

void ConfigManager::resetApplicationSettings()
{
    m_applicationConfig = QJsonObject();
    m_displayConfig = QJsonObject();
    m_workspaceConfig = QJsonObject();
    m_hyprlandConfig = QJsonObject();
    
    // Reset to defaults
    m_autoSave = true;
    m_autoBackup = true;
    m_maxBackups = 5;
    m_strictValidation = true;
    m_enableImportExport = true;
    
    saveApplicationSettings();
    emit settingsChanged();
}

bool ConfigManager::loadHyprlandConfig()
{
    QFile file(m_configPath + "/hyprland.conf");
    if (!file.open(QIODevice::ReadOnly)) {
        emit error("Failed to open Hyprland config file");
        return false;
    }
    
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    
    if (!parseHyprlandMonitorsConfig(content)) {
        emit error("Failed to parse Hyprland config");
        return false;
    }
    
    emit configChanged();
    return true;
}

bool ConfigManager::saveHyprlandConfig(const QString &path)
{
    QString configContent = generateHyprlandMonitorsConfig(m_displayConfig);
    
    QFile file(path.isEmpty() ? m_monitorsPath : path);
    if (!file.open(QIODevice::WriteOnly)) {
        emit error(QString("Failed to open file for writing: %1").arg(file.fileName()));
        return false;
    }
    
    file.write(configContent.toUtf8());
    file.close();
    
    emit success("Hyprland config saved successfully");
    return true;
}

bool ConfigManager::loadHyprlandMonitors(const QString &path)
{
    QString filePath = path.isEmpty() ? m_monitorsPath : path;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Failed to open monitors file: %1").arg(filePath));
        return false;
    }
    
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    
    if (!parseHyprlandMonitorsConfig(content)) {
        emit error("Failed to parse monitors configuration");
        return false;
    }
    
    emit displayConfigChanged();
    return true;
}

bool ConfigManager::saveHyprlandMonitors(const QString &path)
{
    QString configContent = generateHyprlandMonitorsConfig(m_displayConfig);
    
    QFile file(path.isEmpty() ? m_monitorsPath : path);
    if (!file.open(QIODevice::WriteOnly)) {
        emit error(QString("Failed to open file for writing: %1").arg(file.fileName()));
        return false;
    }
    
    file.write(configContent.toUtf8());
    file.close();
    
    emit success("Monitors configuration saved successfully");
    return true;
}

bool ConfigManager::loadHyprlandWorkspaces(const QString &path)
{
    QString filePath = path.isEmpty() ? m_workspacesPath : path;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Failed to open workspaces file: %1").arg(filePath));
        return false;
    }
    
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    
    if (!parseHyprlandWorkspacesConfig(content)) {
        emit error("Failed to parse workspaces configuration");
        return false;
    }
    
    emit workspaceConfigChanged();
    return true;
}

bool ConfigManager::saveHyprlandWorkspaces(const QString &path)
{
    QString configContent = generateHyprlandWorkspacesConfig(m_workspaceConfig);
    
    QFile file(path.isEmpty() ? m_workspacesPath : path);
    if (!file.open(QIODevice::WriteOnly)) {
        emit error(QString("Failed to open file for writing: %1").arg(file.fileName()));
        return false;
    }
    
    file.write(configContent.toUtf8());
    file.close();
    
    emit success("Workspaces configuration saved successfully");
    return true;
}

QJsonObject ConfigManager::getDisplayConfig() const
{
    return m_displayConfig;
}

void ConfigManager::setDisplayConfig(const QJsonObject &config)
{
    m_displayConfig = config;
    emit displayConfigChanged();
}

QJsonObject ConfigManager::getWorkspaceConfig() const
{
    return m_workspaceConfig;
}

void ConfigManager::setWorkspaceConfig(const QJsonObject &config)
{
    m_workspaceConfig = config;
    emit workspaceConfigChanged();
}

bool ConfigManager::createBackup(const QString &path)
{
    QString backupPath = path;
    if (backupPath.isEmpty()) {
        QDateTime now = QDateTime::currentDateTime();
        backupPath = m_backupPath + QString("/hyprdisplays_backup_%1.json")
                    .arg(now.toString("yyyyMMdd_hhmmss"));
    }
    
    QJsonObject backupData;
    backupData["displayConfig"] = m_displayConfig;
    backupData["workspaceConfig"] = m_workspaceConfig;
    backupData["applicationConfig"] = m_applicationConfig;
    backupData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    backupData["version"] = "1.0.0";
    
    QJsonDocument doc(backupData);
    QFile file(backupPath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        emit error(QString("Failed to create backup: %1").arg(backupPath));
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    emit backupCreated(backupPath);
    emit success("Backup created successfully");
    return true;
}

bool ConfigManager::restoreBackup(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Failed to open backup file: %1").arg(path));
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        emit error("Invalid backup file format");
        return false;
    }
    
    QJsonObject backupData = doc.object();
    
    m_displayConfig = backupData["displayConfig"].toObject();
    m_workspaceConfig = backupData["workspaceConfig"].toObject();
    m_applicationConfig = backupData["applicationConfig"].toObject();
    
    emit backupRestored(path);
    emit success("Backup restored successfully");
    return true;
}

QStringList ConfigManager::getBackups() const
{
    QStringList backups;
    QDir backupDir(m_backupPath);
    
    QFileInfoList files = backupDir.entryInfoList(QStringList() << "*.json", QDir::Files);
    for (const QFileInfo &fileInfo : files) {
        if (m_backupFileRegex.match(fileInfo.fileName()).hasMatch()) {
            backups.append(fileInfo.absoluteFilePath());
        }
    }
    
    // Sort by modification time (newest first)
    std::sort(backups.begin(), backups.end(), [](const QString &a, const QString &b) {
        return QFileInfo(a).lastModified() > QFileInfo(b).lastModified();
    });
    
    return backups;
}

bool ConfigManager::deleteBackup(const QString &path)
{
    QFile file(path);
    if (file.remove()) {
        emit backupDeleted(path);
        emit success("Backup deleted successfully");
        return true;
    } else {
        emit error(QString("Failed to delete backup: %1").arg(path));
        return false;
    }
}

bool ConfigManager::validateConfig(const QJsonObject &config)
{
    m_validationErrors.clear();
    
    if (config.contains("displays")) {
        validateMonitorsConfig(config);
    }
    
    if (config.contains("workspaces")) {
        validateWorkspacesConfig(config);
    }
    
    return m_validationErrors.isEmpty();
}

bool ConfigManager::validateMonitorsConfig(const QJsonObject &config)
{
    QJsonArray displays = config["displays"].toArray();
    
    for (int i = 0; i < displays.size(); ++i) {
        QJsonObject display = displays[i].toObject();
        DisplayInfo displayInfo = DisplayInfo::fromJson(display);
        
        if (!validateDisplayInfo(displayInfo)) {
            m_validationErrors.append(QString("Display %1: Invalid configuration").arg(i + 1));
        }
    }
    
    return m_validationErrors.isEmpty();
}

bool ConfigManager::validateWorkspacesConfig(const QJsonObject &config)
{
    QJsonArray workspaces = config["workspaces"].toArray();
    
    for (int i = 0; i < workspaces.size(); ++i) {
        QJsonObject workspace = workspaces[i].toObject();
        QString workspaceName = workspace["name"].toString();
        QString monitorName = workspace["monitor"].toString();
        
        if (!validateWorkspaceAssignment(workspaceName, monitorName)) {
            m_validationErrors.append(QString("Workspace %1: Invalid assignment").arg(i + 1));
        }
    }
    
    return m_validationErrors.isEmpty();
}

QStringList ConfigManager::getValidationErrors() const
{
    return m_validationErrors;
}

bool ConfigManager::importConfig(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Implement config import
    return true;
}

bool ConfigManager::exportConfig(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Implement config export
    return true;
}

bool ConfigManager::importFromNwgDisplays(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Implement nwg-displays import
    return true;
}

bool ConfigManager::exportToNwgDisplays(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Implement nwg-displays export
    return true;
}

void ConfigManager::onSettingsChanged()
{
    emit settingsChanged();
}

void ConfigManager::onConfigChanged()
{
    emit configChanged();
}

void ConfigManager::onDisplayConfigChanged()
{
    emit displayConfigChanged();
}

void ConfigManager::onWorkspaceConfigChanged()
{
    emit workspaceConfigChanged();
}

void ConfigManager::onFileChanged(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Implement file change monitoring
}

void ConfigManager::onDirectoryChanged(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Implement directory change monitoring
}

bool ConfigManager::readJsonFile(const QString &path, QJsonObject &json)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        return false;
    }
    
    json = doc.object();
    return true;
}

bool ConfigManager::writeJsonFile(const QString &path, const QJsonObject &json)
{
    QJsonDocument doc(json);
    QFile file(path);
    
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    return true;
}

bool ConfigManager::readTextFile(const QString &path, QString &content)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    content = QString::fromUtf8(file.readAll());
    file.close();
    return true;
}

bool ConfigManager::writeTextFile(const QString &path, const QString &content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(content.toUtf8());
    file.close();
    return true;
}

bool ConfigManager::parseHyprlandMonitorsConfig(const QString &content)
{
    m_displayConfig = QJsonObject();
    QJsonArray displaysArray;
    
    QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.startsWith("monitor=")) {
            QString monitorConfig = trimmedLine.mid(8); // Remove "monitor="
            
            // Parse monitor configuration
            QStringList parts = monitorConfig.split(',');
            if (parts.size() >= 4) {
                QJsonObject display;
                display["name"] = parts[0];
                
                // Parse resolution and refresh rate
                QString resolution = parts[1];
                int atIndex = resolution.indexOf('@');
                if (atIndex > 0) {
                    QString res = resolution.left(atIndex);
                    QString refresh = resolution.mid(atIndex + 1);
                    
                    QStringList resParts = res.split('x');
                    if (resParts.size() == 2) {
                        display["width"] = resParts[0].toInt();
                        display["height"] = resParts[1].toInt();
                        display["refreshRate"] = refresh.toDouble();
                    }
                }
                
                // Parse position
                QString position = parts[2];
                QStringList posParts = position.split('x');
                if (posParts.size() == 2) {
                    display["x"] = posParts[0].toInt();
                    display["y"] = posParts[1].toInt();
                }
                
                // Parse scale
                if (parts.size() > 3) {
                    display["scale"] = parts[3].toDouble();
                }
                
                // Parse additional parameters
                int vrrMode = 0;
                bool hdrEnabled = false;
                double sdrBrightness = 1.0;
                double sdrSaturation = 1.0;
                bool tenBit = false;
                bool wideGamut = false;
                
                for (int i = 4; i < parts.size(); ++i) {
                    QString part = parts[i];
                    if (part == "cm") {
                        // Color management enabled - check next part for type
                        if (i + 1 < parts.size()) {
                            QString cmType = parts[i + 1];
                            if (cmType == "hdr") {
                                hdrEnabled = true;
                                i++; // Skip the cm type since we consumed it
                            } else if (cmType == "wide") {
                                wideGamut = true;
                                i++; // Skip the cm type since we consumed it
                            }
                        }
                    } else if (part == "bitdepth") {
                        // Check if the next part is "10" for 10-bit
                        if (i + 1 < parts.size() && parts[i + 1] == "10") {
                            tenBit = true;
                            i++; // Skip the next part since we consumed it
                        }
                    } else if (part == "vrr") {
                        // VRR mode follows
                        if (i + 1 < parts.size()) {
                            vrrMode = parts[i + 1].toInt();
                            i++; // Skip the next part since we consumed it
                        }
                    } else if (part == "sdrbrightness") {
                        // SDR brightness follows
                        if (i + 1 < parts.size()) {
                            sdrBrightness = parts[i + 1].toDouble();
                            i++; // Skip the next part since we consumed it
                        }
                    } else if (part == "sdrsaturation") {
                        // SDR saturation follows
                        if (i + 1 < parts.size()) {
                            sdrSaturation = parts[i + 1].toDouble();
                            i++; // Skip the next part since we consumed it
                        }
                    } else if (part == "mirror") {
                        // Mirror target follows
                        if (i + 1 < parts.size()) {
                            display["mirrorOf"] = parts[i + 1];
                            i++; // Skip the next part since we consumed it
                        }
                    } else if (!part.isEmpty() && part != "auto") {
                        // Assume it's a transform
                        display["transform"] = part;
                    }
                }
                
                display["vrrMode"] = vrrMode;
                display["hdr"] = hdrEnabled;
                display["sdrBrightness"] = sdrBrightness;
                display["sdrSaturation"] = sdrSaturation;
                display["tenBit"] = tenBit;
                display["wideGamut"] = wideGamut;
                
                displaysArray.append(display);
            }
        }
    }
    
    m_displayConfig["displays"] = displaysArray;
    return true;
}

bool ConfigManager::parseHyprlandWorkspacesConfig(const QString &content)
{
    m_workspaceConfig = QJsonObject();
    QJsonArray workspacesArray;
    
    QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.startsWith("workspace=")) {
            QString workspaceConfig = trimmedLine.mid(10); // Remove "workspace="
            
            // Parse workspace configuration
            QStringList parts = workspaceConfig.split(',');
            if (parts.size() >= 2) {
                QJsonObject workspace;
                workspace["name"] = parts[0];
                
                if (parts[1].startsWith("monitor:")) {
                    workspace["monitor"] = parts[1].mid(8); // Remove "monitor:"
                }
                
                workspacesArray.append(workspace);
            }
        }
    }
    
    m_workspaceConfig["workspaces"] = workspacesArray;
    return true;
}

QString ConfigManager::generateHyprlandMonitorsConfig(const QJsonObject &config)
{
    QString content;
    QJsonArray displays = config["displays"].toArray();
    for (const QJsonValue &value : displays) {
        QJsonObject display = value.toObject();
        QString name = display["name"].toString();
        int width = display["width"].toInt();
        int height = display["height"].toInt();
        double refresh = display["refreshRate"].toDouble();
        int x = display["x"].toInt();
        int y = display["y"].toInt();
        double scale = display["scale"].toDouble(1.0);
        QString line = QString("monitor=%1,%2x%3@%4,%5x%6,%7")
            .arg(name)
            .arg(width)
            .arg(height)
            .arg(refresh, 0, 'f', 2)
            .arg(x)
            .arg(y)
            .arg(scale, 0, 'f', 2);

        // Hyprland HDR/CM options
        bool hdrEnabled = display.contains("hdr") && display["hdr"].toBool();
        bool tenBit = display.contains("tenBit") && display["tenBit"].toBool();
        bool wideGamut = display.contains("wideGamut") && display["wideGamut"].toBool();
        
        qDebug() << "Generating config for" << name << "HDR:" << hdrEnabled << "10-bit:" << tenBit << "Wide gamut:" << wideGamut;
        
        // Build additional options list
        QStringList additionalOptions;
        
        // Handle HDR and wide gamut options (they go together with cm)
        if (hdrEnabled || wideGamut) {
            additionalOptions << "cm";
            if (hdrEnabled) {
                additionalOptions << "hdr";
                double sdrb = display.contains("sdrBrightness") ? display["sdrBrightness"].toDouble(1.0) : 1.0;
                double sdrs = display.contains("sdrSaturation") ? display["sdrSaturation"].toDouble(1.0) : 1.0;
                additionalOptions << QString("sdrbrightness,%1").arg(sdrb, 0, 'f', 2) << QString("sdrsaturation,%1").arg(sdrs, 0, 'f', 2);
            } else if (wideGamut) {
                additionalOptions << "wide";
            }
        }
        
        if (tenBit) {
            additionalOptions << "bitdepth,10";
        }
        
        // Hyprland VRR option
        if (display.contains("vrrMode")) {
            int vrr = display["vrrMode"].toInt();
            if (vrr != 0) {
                additionalOptions << QString("vrr,%1").arg(vrr);
            }
        }
        
        // Add all additional options to the line
        if (!additionalOptions.isEmpty()) {
            line += "," + additionalOptions.join(",");
        }

        qDebug() << "Generated line:" << line;
        content += line + "\n";
    }
    return content;
}

QString ConfigManager::generateHyprlandWorkspacesConfig(const QJsonObject &config)
{
    QString content;
    QJsonArray workspaces = config["workspaces"].toArray();
    
    for (const QJsonValue &value : workspaces) {
        QJsonObject workspace = value.toObject();
        
        QString line = QString("workspace=%1,monitor:%2")
                      .arg(workspace["name"].toString())
                      .arg(workspace["monitor"].toString());
        
        content += line + "\n";
    }
    
    return content;
}

QString ConfigManager::getConfigPath() const
{
    return m_configPath;
}

QString ConfigManager::getSettingsPath() const
{
    return m_settingsPath;
}

QString ConfigManager::getBackupPath() const
{
    return m_backupPath;
}

QString ConfigManager::getDefaultMonitorsPath() const
{
    return m_monitorsPath;
}

QString ConfigManager::getDefaultWorkspacesPath() const
{
    return m_workspacesPath;
}

bool ConfigManager::validateDisplayInfo(const DisplayInfo &display)
{
    if (display.name.isEmpty()) {
        return false;
    }
    
    if (display.width <= 0 || display.height <= 0) {
        return false;
    }
    
    if (display.refreshRate <= 0) {
        return false;
    }
    
    if (display.scale <= 0) {
        return false;
    }
    
    return true;
}

bool ConfigManager::validateWorkspaceAssignment(const QString &workspace, const QString &monitor)
{
    if (workspace.isEmpty()) {
        return false;
    }
    
    return true;
}

bool ConfigManager::validateMonitorName(const QString &name)
{
    return !name.isEmpty();
}

bool ConfigManager::validateResolution(const QString &resolution)
{
    QRegularExpression regex(R"(\d+x\d+)");
    return regex.match(resolution).hasMatch();
}

bool ConfigManager::validateRefreshRate(int refreshRate)
{
    return refreshRate > 0 && refreshRate <= 360;
}

bool ConfigManager::validateScale(double scale)
{
    return scale > 0 && scale <= 5.0;
}

bool ConfigManager::validateTransform(const QString &transform)
{
    QStringList validTransforms = {"normal", "90", "180", "270", "flipped", "flipped-90"};
    return validTransforms.contains(transform);
}

bool ConfigManager::validatePosition(int x, int y)
{
    return x >= -10000 && x <= 10000 && y >= -10000 && y <= 10000;
}

QJsonObject ConfigManager::displayInfoToJson(const DisplayInfo &display)
{
    return display.toJson();
}

DisplayInfo ConfigManager::jsonToDisplayInfo(const QJsonObject &json)
{
    return DisplayInfo::fromJson(json);
}

QJsonObject ConfigManager::workspaceAssignmentToJson(const QString &workspace, const QString &monitor)
{
    QJsonObject json;
    json["workspace"] = workspace;
    json["monitor"] = monitor;
    return json;
}

QString ConfigManager::jsonToWorkspaceAssignment(const QJsonObject &json)
{
    return json["workspace"].toString();
}

bool ConfigManager::migrateFromOldConfig()
{
    // TODO: Implement migration from old config format
    return true;
}

bool ConfigManager::migrateFromNwgDisplays()
{
    // TODO: Implement migration from nwg-displays
    return true;
}

bool ConfigManager::migrateToNewFormat()
{
    // TODO: Implement migration to new format
    return true;
} 