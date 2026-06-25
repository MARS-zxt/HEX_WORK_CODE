#include <QApplication>
#include <QFile>
#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Application metadata
    app.setApplicationName(QStringLiteral("ValveSimulator"));
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ValveSim");

    // Global default font
    QFont defaultFont = app.font();
    defaultFont.setFamily("Microsoft YaHei UI");
    defaultFont.setPointSize(10);
    app.setFont(defaultFont);

    // Load global stylesheet
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QString::fromUtf8(styleFile.readAll());
        app.setStyleSheet(style);
        styleFile.close();
    }

    MainWindow window;
    window.show();

    return app.exec();
}
