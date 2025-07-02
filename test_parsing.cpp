#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
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
};

bool parseHyprctlOutput(const QString &output, QList<DisplayInfo> &displays)
{
    displays.clear();
    
    // Split output into lines
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    
    DisplayInfo currentDisplay;
    bool inMonitor = false;
    
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        
        // Check for monitor start line: "Monitor DP-1 (ID 0):"
        QRegularExpression monitorStartRegex(R"(Monitor (\w+)\s*\(ID \d+\):)");
        QRegularExpressionMatch monitorMatch = monitorStartRegex.match(trimmedLine);
        
        if (monitorMatch.hasMatch()) {
            // Save previous monitor if exists
            if (inMonitor && !currentDisplay.name.isEmpty()) {
                displays.append(currentDisplay);
            }
            
            // Start new monitor
            currentDisplay = DisplayInfo();
            currentDisplay.name = monitorMatch.captured(1);
            currentDisplay.enabled = true;
            currentDisplay.primary = false;
            currentDisplay.scale = 1.0;
            currentDisplay.transform = "normal";
            currentDisplay.mirrorOf = "";
            currentDisplay.workspace = "";
            inMonitor = true;
            continue;
        }
        
        if (!inMonitor) continue;
        
        // Parse resolution and position: "2560x1440@179.99899 at 2560x0"
        QRegularExpression resolutionRegex(R"((\d+)x(\d+)@([\d.]+)\s+at\s+(\d+)x(\d+))");
        QRegularExpressionMatch resMatch = resolutionRegex.match(trimmedLine);
        
        if (resMatch.hasMatch()) {
            currentDisplay.width = resMatch.captured(1).toInt();
            currentDisplay.height = resMatch.captured(2).toInt();
            currentDisplay.refreshRate = resMatch.captured(3).toDouble();
            currentDisplay.x = resMatch.captured(4).toInt();
            currentDisplay.y = resMatch.captured(5).toInt();
            currentDisplay.resolution = QString("%1x%2").arg(currentDisplay.width).arg(currentDisplay.height);
            currentDisplay.position = QString("%1x%2").arg(currentDisplay.x).arg(currentDisplay.y);
            continue;
        }
        
        // Parse description: "description: Shenzhen KTC Technology Group H27S17 0000000000000"
        if (trimmedLine.startsWith("description:")) {
            currentDisplay.description = trimmedLine.mid(12).trimmed();
            continue;
        }
        
        // Parse make: "make: Shenzhen KTC Technology Group"
        if (trimmedLine.startsWith("make:")) {
            currentDisplay.manufacturer = trimmedLine.mid(5).trimmed();
            continue;
        }
        
        // Parse model: "model: H27S17"
        if (trimmedLine.startsWith("model:")) {
            currentDisplay.model = trimmedLine.mid(6).trimmed();
            continue;
        }
        
        // Parse serial: "serial: 0000000000000"
        if (trimmedLine.startsWith("serial:")) {
            currentDisplay.serial = trimmedLine.mid(7).trimmed();
            continue;
        }
        
        // Parse scale: "scale: 1.00"
        if (trimmedLine.startsWith("scale:")) {
            currentDisplay.scale = trimmedLine.mid(6).trimmed().toDouble();
            continue;
        }
        
        // Parse transform: "transform: 0"
        if (trimmedLine.startsWith("transform:")) {
            QString transform = trimmedLine.mid(10).trimmed();
            if (transform == "0") {
                currentDisplay.transform = "normal";
            } else if (transform == "1") {
                currentDisplay.transform = "90";
            } else if (transform == "2") {
                currentDisplay.transform = "180";
            } else if (transform == "3") {
                currentDisplay.transform = "270";
            } else {
                currentDisplay.transform = transform;
            }
            continue;
        }
        
        // Parse mirrorOf: "mirrorOf: none"
        if (trimmedLine.startsWith("mirrorOf:")) {
            QString mirror = trimmedLine.mid(9).trimmed();
            if (mirror != "none") {
                currentDisplay.mirrorOf = mirror;
            }
            continue;
        }
        
        // Parse active workspace: "active workspace: 1 (1)"
        if (trimmedLine.startsWith("active workspace:")) {
            QRegularExpression workspaceRegex(R"(active workspace: (\d+))");
            QRegularExpressionMatch wsMatch = workspaceRegex.match(trimmedLine);
            if (wsMatch.hasMatch()) {
                currentDisplay.workspace = wsMatch.captured(1);
            }
            continue;
        }
        
        // Parse disabled: "disabled: false"
        if (trimmedLine.startsWith("disabled:")) {
            currentDisplay.enabled = !trimmedLine.mid(9).trimmed().contains("true", Qt::CaseInsensitive);
            continue;
        }
    }
    
    // Add the last monitor
    if (inMonitor && !currentDisplay.name.isEmpty()) {
        displays.append(currentDisplay);
    }
    
    // Set primary display (leftmost display)
    if (!displays.isEmpty()) {
        DisplayInfo *primaryDisplay = nullptr;
        int minX = INT_MAX;
        
        for (int i = 0; i < displays.size(); ++i) {
            if (displays[i].x < minX) {
                minX = displays[i].x;
                primaryDisplay = &displays[i];
            }
        }
        
        if (primaryDisplay) {
            primaryDisplay->primary = true;
        }
    }
    
    return !displays.isEmpty();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // Execute hyprctl monitors command
    QProcess process;
    process.start("hyprctl", {"monitors"});
    process.waitForFinished(5000);
    
    if (process.exitCode() != 0) {
        qWarning() << "Failed to execute hyprctl monitors";
        return 1;
    }
    
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    qDebug() << "Raw output:";
    qDebug() << output;
    
    QList<DisplayInfo> displays;
    if (parseHyprctlOutput(output, displays)) {
        qDebug() << "Successfully parsed" << displays.size() << "displays:";
        for (const DisplayInfo &display : displays) {
            qDebug() << "Display:" << display.name;
            qDebug() << "  Resolution:" << display.resolution;
            qDebug() << "  Position:" << display.position;
            qDebug() << "  Refresh Rate:" << display.refreshRate;
            qDebug() << "  Scale:" << display.scale;
            qDebug() << "  Primary:" << display.primary;
            qDebug() << "  Enabled:" << display.enabled;
            qDebug() << "  Description:" << display.description;
            qDebug() << "  Make:" << display.manufacturer;
            qDebug() << "  Model:" << display.model;
            qDebug() << "---";
        }
    } else {
        qWarning() << "Failed to parse monitor output";
        return 1;
    }
    
    return 0;
} 