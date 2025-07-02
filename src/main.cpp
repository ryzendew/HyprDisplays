#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QStandardPaths>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
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
    MainWindow window;
    window.setMonitorsPath(parser.value(monitorsPathOption));
    window.setNumWorkspaces(parser.value(numWorkspacesOption).toInt());
    window.show();

    return app.exec();
} 