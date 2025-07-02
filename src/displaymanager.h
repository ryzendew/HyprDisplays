#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QTimer>
#include <QScreen>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

struct DisplayInfo {
    QString name;
    QString description;
    QString manufacturer;
    QString model;
    QString serial;
    int width;
    int height;
    int refreshRate;
    int x;
    int y;
    double scale;
    bool enabled;
    bool primary;
    QString resolution;
    QString position;
    QString transform;
    QString mirrorOf;
    QString workspace;
    
    // New Hyprland properties
    bool hdr;
    double sdrBrightness;
    double sdrSaturation;
    int vrrMode;  // 0=disabled, 1=global, 2=fullscreen only
    QList<QString> availableModes;
    bool vrrCapable;
    bool hdrCapable;
    bool tenBit;
    bool wideGamut;
    
    QJsonObject toJson() const;
    static DisplayInfo fromJson(const QJsonObject &json);
};

class DisplayManager : public QObject
{
    Q_OBJECT

public:
    explicit DisplayManager(QObject *parent = nullptr);
    ~DisplayManager();

    QList<DisplayInfo> getDisplays() const;
    DisplayInfo getDisplay(const QString &name) const;
    void setDisplay(const DisplayInfo &display);
    void updateDisplayInMemory(const DisplayInfo &display);
    void removeDisplay(const QString &name);
    void clearDisplays();
    
    bool refreshDisplays();
    bool applyConfiguration();
    bool saveConfiguration(const QString &path);
    bool loadConfiguration(const QString &path);
    
    QJsonObject getConfiguration() const;
    void setConfiguration(const QJsonObject &config);
    
    QStringList getDisplayNames() const;
    QStringList getWorkspaceNames() const;
    
    void setNumWorkspaces(int num);
    int getNumWorkspaces() const;

public slots:
    void onDisplayChanged();
    void onConfigurationChanged();

signals:
    void displaysChanged();
    void configurationChanged();
    void error(const QString &message);
    void success(const QString &message);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    bool parseHyprctlOutput(const QString &output);
    bool parseMonitorOutput(const QString &output);
    bool parseDeviceOutput(const QString &output);
    bool parseWorkspaceOutput(const QString &output);
    
    QString executeHyprctlCommand(const QStringList &args);
    bool executeHyprctlCommandAsync(const QStringList &args);
    
    void updateDisplayPositions();
    void validateConfiguration();
    void sortDisplays();
    QString buildMonitorCommand(const DisplayInfo &monitor);
    
    QList<DisplayInfo> m_displays;
    QJsonObject m_configuration;
    int m_numWorkspaces;
    
    QProcess *m_hyprctlProcess;
    QTimer *m_refreshTimer;
    
    QStringList m_workspaceNames;
    bool m_isRefreshing;
};

#endif // DISPLAYMANAGER_H 