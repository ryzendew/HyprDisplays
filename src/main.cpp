#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>
#include <iostream>
#include "mainwindow.h"

// Custom message handler to log to file
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QFile logFile;
    static QTextStream logStream;
    
    // Initialize log file if not already done
    if (!logFile.isOpen()) {
        QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(logDir);
        QString logPath = logDir + "/hyprdisplays.log";
        logFile.setFileName(logPath);
        logFile.open(QIODevice::WriteOnly | QIODevice::Append);
        logStream.setDevice(&logFile);
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString typeStr;
    
    switch (type) {
        case QtDebugMsg:
            typeStr = "DEBUG";
            break;
        case QtInfoMsg:
            typeStr = "INFO";
            break;
        case QtWarningMsg:
            typeStr = "WARNING";
            break;
        case QtCriticalMsg:
            typeStr = "CRITICAL";
            break;
        case QtFatalMsg:
            typeStr = "FATAL";
            break;
    }
    
    QString logMessage = QString("[%1] %2: %3").arg(timestamp, typeStr, msg);
    logStream << logMessage << Qt::endl;
    logStream.flush();
    
    // Also output to console for immediate feedback
    std::cout << logMessage.toStdString() << std::endl;
    
    // For fatal messages, also show a message box
    if (type == QtFatalMsg) {
        QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/hyprdisplays.log";
        QMessageBox::critical(nullptr, "HyprDisplays Fatal Error", 
                             QString("A fatal error occurred:\n%1\n\nLog file location:\n%2").arg(msg, logPath));
    }
}

int main(int argc, char *argv[])
{
    // Install custom message handler
    qInstallMessageHandler(messageHandler);
    
    qInfo() << "=== HyprDisplays Starting ===";
    qInfo() << "Version: 1.0.0";
    qInfo() << "Qt Version:" << QT_VERSION_STR;
    qInfo() << "Build Date:" << __DATE__ << __TIME__;
    qInfo() << "Command line arguments:" << argc;
    for (int i = 0; i < argc; ++i) {
        qInfo() << "  Arg" << i << ":" << argv[i];
    }
    
    try {
        QApplication app(argc, argv);
        app.setApplicationName("HyprDisplays");
        app.setApplicationVersion("1.0.0");
        app.setOrganizationName("HyprDisplays");
        app.setOrganizationDomain("hyprdisplays.org");

        // Set up command line parser
        QCommandLineParser parser;
        parser.setApplicationDescription("Hyprland display management utility");
        parser.addHelpOption();
        parser.addVersionOption();

        QCommandLineOption monitorsPathOption(
            QStringList() << "m" << "monitors-path",
            "Path to save the monitors.conf file to",
            "path",
            QDir::homePath() + "/.config/hypr/monitors.conf"
        );
        parser.addOption(monitorsPathOption);

        QCommandLineOption numWorkspacesOption(
            QStringList() << "n" << "num-ws",
            "Number of workspaces in use",
            "number",
            "10"
        );
        parser.addOption(numWorkspacesOption);

        parser.process(app);

        // Create main window
        qInfo() << "Creating MainWindow...";
        MainWindow window;
        qInfo() << "Setting monitors path:" << parser.value(monitorsPathOption);
        window.setMonitorsPath(parser.value(monitorsPathOption));
        qInfo() << "Setting num workspaces:" << parser.value(numWorkspacesOption).toInt();
        window.setNumWorkspaces(parser.value(numWorkspacesOption).toInt());
        qInfo() << "Showing window...";
        window.show();
        qInfo() << "Window shown successfully";
        qInfo() << "=== HyprDisplays Started Successfully ===";

        return app.exec();
    } catch (const std::exception& e) {
        qCritical() << "Fatal error:" << e.what();
        qCritical() << "=== HyprDisplays Failed to Start ===";
        return 1;
    } catch (...) {
        qCritical() << "Unknown fatal error occurred";
        qCritical() << "=== HyprDisplays Failed to Start ===";
        return 1;
    }
} 