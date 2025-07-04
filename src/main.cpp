#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QStandardPaths>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
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
        qDebug() << "Creating MainWindow...";
        MainWindow window;
        qDebug() << "Setting monitors path:" << parser.value(monitorsPathOption);
        window.setMonitorsPath(parser.value(monitorsPathOption));
        qDebug() << "Setting num workspaces:" << parser.value(numWorkspacesOption).toInt();
        window.setNumWorkspaces(parser.value(numWorkspacesOption).toInt());
        qDebug() << "Showing window...";
        window.show();
        qDebug() << "Window shown successfully";

        return app.exec();
    } catch (const std::exception& e) {
        qCritical() << "Fatal error:" << e.what();
        return 1;
    } catch (...) {
        qCritical() << "Unknown fatal error occurred";
        return 1;
    }
} 