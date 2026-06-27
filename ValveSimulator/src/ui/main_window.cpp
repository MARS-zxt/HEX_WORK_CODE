#include "main_window.h"
#include "valve_params_dialog.h"
#include "version_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSplitter>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

// ============================================================================
// Construction
// ============================================================================
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    // Frameless window with translucent background for rounded corners + shadow
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setMinimumSize(860, 680);
    resize(960, 740);

    // Window shadow
    auto *windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(30);
    windowShadow->setOffset(0, 8);
    windowShadow->setColor(QColor(0, 0, 0, 80));
    setGraphicsEffect(windowShadow);

    // Simulation engine
    simulator_ = new ValveSimulator(this);
    connect(simulator_, &ValveSimulator::stateChanged, this, &MainWindow::onSimulatorTick);
    connect(simulator_, &ValveSimulator::actionStarted, this, &MainWindow::onSimulatorStarted);
    connect(simulator_, &ValveSimulator::actionStopped, this, &MainWindow::onSimulatorStopped);

    setupUi();
    setupTcpServer();
}

MainWindow::~MainWindow()
{
    tcp_server_->close();
    for (auto *client : tcp_clients_) {
        client->disconnectFromHost();
        client->deleteLater();  // explicit cleanup — disconnectFromHost is async
    }
    tcp_clients_.clear();
}

// ============================================================================
// UI Setup
// ============================================================================
void MainWindow::setupUi()
{
    // Outer layout with shadow margin
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(SHADOW_MARGIN, SHADOW_MARGIN,
                                     SHADOW_MARGIN, SHADOW_MARGIN);
    outerLayout->setSpacing(0);

    // --- Content container (white rounded rect) ---
    auto *contentWidget = new QWidget(this);
    contentWidget->setObjectName("contentWidget");
    contentWidget->setStyleSheet(
        "#contentWidget {"
        "  background: #F5F5F5;"
        "  border-radius: 20px;"
        "}");

    auto *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // --- Title bar ---
    title_bar_ = new CustomTitleBar(contentWidget);
    connect(title_bar_, &CustomTitleBar::minimizeClicked, this, &MainWindow::onMinimize);
    connect(title_bar_, &CustomTitleBar::maximizeClicked,   this, &MainWindow::onMaximize);
    connect(title_bar_, &CustomTitleBar::closeClicked,     this, &MainWindow::onClose);
    contentLayout->addWidget(title_bar_);

    // --- Separator ---
    // (handled in title bar paint)

    // =====================================================================
    // Left panel: Virtual Valve
    // =====================================================================
    auto *valveTab = new QWidget();
    auto *valveTabLayout = new QVBoxLayout(valveTab);
    valveTabLayout->setContentsMargins(12, 12, 8, 12);
    valveTabLayout->setSpacing(10);

    // -- Control buttons row --
    auto *ctrlLayout = new QHBoxLayout();
    ctrlLayout->setSpacing(10);

    // 3D button stylesheet
    const char *btn3DStyle = R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #fafafa, stop:0.4 #eaeaea, stop:0.5 #dddddd, stop:0.6 #d0d0d0, stop:1 #e2e2e2);
            border: 1px solid #b0b0b0;
            border-top-color: #e0e0e0;
            border-left-color: #e0e0e0;
            border-right-color: #a0a0a0;
            border-bottom-color: #909090;
            border-radius: 6px;
            padding: 8px 24px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff, stop:0.4 #f2f2f2, stop:0.5 #e6e6e6, stop:0.6 #d8d8d8, stop:1 #eaeaea);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #c8c8c8, stop:0.4 #d0d0d0, stop:0.5 #d4d4d4, stop:0.6 #d8d8d8, stop:1 #d0d0d0);
            border-top-color: #909090;
            border-left-color: #909090;
            border-right-color: #c0c0c0;
            border-bottom-color: #c0c0c0;
            padding-top: 9px;
            padding-bottom: 7px;
        }
    )";

    auto *openBtn = new QPushButton(QStringLiteral("开阀"), valveTab);
    openBtn->setStyleSheet(QString(btn3DStyle) + "QPushButton { color: #20A040; }");
    openBtn->setCursor(Qt::PointingHandCursor);
    openBtn->setFixedWidth(100);
    connect(openBtn, &QPushButton::clicked, this, &MainWindow::onOpenValve);

    auto *closeBtn = new QPushButton(QStringLiteral("关阀"), valveTab);
    closeBtn->setStyleSheet(QString(btn3DStyle) + "QPushButton { color: #C04020; }");
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setFixedWidth(100);
    connect(closeBtn, &QPushButton::clicked, this, &MainWindow::onCloseValve);

    auto *stopBtn = new QPushButton(QStringLiteral("停止"), valveTab);
    stopBtn->setStyleSheet(QString(btn3DStyle) + "QPushButton { color: #666; }");
    stopBtn->setCursor(Qt::PointingHandCursor);
    stopBtn->setFixedWidth(100);
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::onStopValve);

    auto *paramsBtn = new QPushButton(QStringLiteral("阀门参数设置"), valveTab);
    paramsBtn->setStyleSheet(QString(btn3DStyle) + "QPushButton { color: #8050C0; }");
    paramsBtn->setCursor(Qt::PointingHandCursor);
    paramsBtn->setFixedWidth(130);
    connect(paramsBtn, &QPushButton::clicked, this, &MainWindow::onParamsClicked);

    auto *aboutBtn = new QPushButton(QStringLiteral("关于"), valveTab);
    aboutBtn->setStyleSheet(QString(btn3DStyle) + "QPushButton { color: #2070C0; }");
    aboutBtn->setCursor(Qt::PointingHandCursor);
    aboutBtn->setFixedWidth(80);
    connect(aboutBtn, &QPushButton::clicked, this, &MainWindow::onAboutClicked);

    ctrlLayout->addWidget(openBtn);
    ctrlLayout->addWidget(closeBtn);
    ctrlLayout->addWidget(stopBtn);
    ctrlLayout->addSpacing(20);
    ctrlLayout->addWidget(paramsBtn);
    ctrlLayout->addWidget(aboutBtn);
    ctrlLayout->addStretch();

    valveTabLayout->addLayout(ctrlLayout);

    // -- Valve display --
    valve_display_ = new ValveDisplay(valveTab);
    valveTabLayout->addWidget(valve_display_);

    // -- Waveform chart --
    auto *chartGroup = new QGroupBox(QStringLiteral("电流波形图 (时域)"), valveTab);
    chartGroup->setStyleSheet(
        "QGroupBox {"
        "  font-size: 12px; font-weight: bold; color: #555;"
        "  border: 1px solid #D8D8D8; border-radius: 6px;"
        "  margin-top: 8px; padding-top: 16px;"
        "  background: #FAFAFA;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin; left: 12px; padding: 0 6px;"
        "}");
    auto *chartLayout = new QVBoxLayout(chartGroup);
    chartLayout->setContentsMargins(4, 18, 4, 4);
    waveform_chart_ = new WaveformChart(chartGroup);
    chartLayout->addWidget(waveform_chart_);
    valveTabLayout->addWidget(chartGroup, 1);

    // =====================================================================
    // Right panel: Test Program
    // =====================================================================
    test_panel_ = new TestPanel();
    test_panel_->setMinimumWidth(280);

    // --- Splitter: valve (left) | test (right) ---
    auto *splitter = new QSplitter(Qt::Horizontal, contentWidget);
    splitter->addWidget(valveTab);
    splitter->addWidget(test_panel_);
    splitter->setStretchFactor(0, 3);  // valve gets more space
    splitter->setStretchFactor(1, 1);
    splitter->setHandleWidth(2);
    splitter->setStyleSheet(
        "QSplitter::handle { background: #D0D0D0; }"
        "QSplitter::handle:hover { background: #3080F0; }");

    contentLayout->addWidget(splitter, 1);

    // --- Status bar ---
    auto *statusBar = new QWidget(contentWidget);
    statusBar->setFixedHeight(24);
    statusBar->setStyleSheet("background: #E8E8E8; border-bottom-left-radius: 20px; border-bottom-right-radius: 20px;");
    auto *statusLayout = new QHBoxLayout(statusBar);
    statusLayout->setContentsMargins(12, 0, 12, 0);
    auto *statusLabel = new QLabel(QStringLiteral("就绪 — 点击开阀/关阀启动模拟"), statusBar);
    statusLabel->setStyleSheet("font-size: 11px; color: #888; background: transparent;");
    statusLayout->addWidget(statusLabel);

    // Update status on state changes
    connect(simulator_, &ValveSimulator::stateChanged, this, [statusLabel, this]() {
        auto snap = simulator_->snapshot();
        statusLabel->setText(QStringLiteral("状态: %1 | 位置: %2% | 时间: %3 s | 电流: %4 mA")
            .arg(valveStateName(snap.state))
            .arg(snap.position * 100.0, 0, 'f', 0)
            .arg(snap.elapsed_time, 0, 'f', 1)
            .arg(snap.current, 0, 'f', 1));
    });

    contentLayout->addWidget(statusBar);
    outerLayout->addWidget(contentWidget);

    // Connect test panel button → launch external C test program
    connect(test_panel_, &TestPanel::testButtonClicked, this, &MainWindow::onLaunchTestProgram);
}

// ============================================================================
// TCP Server (for external C test programs)
// ============================================================================
void MainWindow::setupTcpServer()
{
    tcp_server_ = new QTcpServer(this);
    connect(tcp_server_, &QTcpServer::newConnection, this, &MainWindow::onNewTcpConnection);

    if (tcp_server_->listen(QHostAddress::LocalHost, 9876)) {
        test_panel_->appendLog(QStringLiteral("[系统] TCP服务已启动: localhost:9876"));
    } else {
        test_panel_->appendLog(QStringLiteral("[系统] TCP服务启动失败: ") + tcp_server_->errorString());
    }
}

void MainWindow::onNewTcpConnection()
{
    while (tcp_server_->hasPendingConnections()) {
        auto *client = tcp_server_->nextPendingConnection();
        tcp_clients_.append(client);
        connect(client, &QTcpSocket::readyRead, this, &MainWindow::onTcpData);
        connect(client, &QTcpSocket::disconnected, this, [this, client]() {
            tcp_clients_.removeAll(client);
            client->deleteLater();
        });
        test_panel_->appendLog(QStringLiteral("[系统] TCP客户端已连接"));
    }
}

void MainWindow::onTcpData()
{
    auto *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    while (client->canReadLine()) {
        QString cmd = QString::fromUtf8(client->readLine()).trimmed();
        processTcpCommand(cmd, client);
    }
}

void MainWindow::processTcpCommand(const QString &cmd, QTcpSocket *client)
{
    test_panel_->appendLog(QStringLiteral("[TCP] 收到: ") + cmd);

    if (cmd == "OPEN") {
        simulator_->openValve();
        client->write("OK OPEN\n");
    }
    else if (cmd == "CLOSE") {
        simulator_->closeValve();
        client->write("OK CLOSE\n");
    }
    else if (cmd == "STOP") {
        simulator_->stopValve();
        client->write("OK STOP\n");
    }
    else if (cmd == "GET_DATA") {
        ValveSnapshot snap = simulator_->snapshot();
        QString resp = QString("DATA time=%1 current=%2 position=%3 state=%4 open_limit=%5 close_limit=%6\n")
            .arg(snap.elapsed_time, 0, 'f', 2)
            .arg(snap.current, 0, 'f', 1)
            .arg(snap.position, 0, 'f', 3)
            .arg(valveStateName(snap.state))
            .arg(snap.open_limit ? 1 : 0)
            .arg(snap.close_limit ? 1 : 0);
        client->write(resp.toUtf8());
    }
    else if (cmd.startsWith("SET_PARAMS")) {
        // Format: SET_PARAMS startup_time travel_time running_current stall_current startup_current
        auto parts = cmd.split(' ');
        if (parts.size() == 6) {
            ValveParams p;
            p.startup_time    = parts[1].toDouble();
            p.travel_time     = parts[2].toDouble();
            p.running_current = parts[3].toDouble();
            p.stall_current   = parts[4].toDouble();
            p.startup_current = parts[5].toDouble();
            simulator_->setParams(p);
            client->write("OK SET_PARAMS\n");
        } else {
            client->write("ERROR invalid SET_PARAMS format\n");
        }
    }
    else {
        client->write(QString("ERROR unknown command: %1\n").arg(cmd).toUtf8());
    }
}

// ============================================================================
// Slots
// ============================================================================
void MainWindow::onOpenValve()
{
    simulator_->openValve();
}

void MainWindow::onCloseValve()
{
    simulator_->closeValve();
}

void MainWindow::onStopValve()
{
    simulator_->stopValve();
}

void MainWindow::onParamsClicked()
{
    ValveParamsDialog dlg(simulator_->params(), this);
    if (dlg.exec() == QDialog::Accepted) {
        simulator_->setParams(dlg.getParams());
        test_panel_->appendLog(QStringLiteral("[系统] 阀门参数已更新"));
    }
}

void MainWindow::onAboutClicked()
{
    VersionDialog dlg(this);
    dlg.exec();
}

void MainWindow::onSimulatorTick()
{
    ValveSnapshot snap = simulator_->snapshot();
    valve_display_->updateFromSnapshot(snap);
    waveform_chart_->setData(simulator_->waveformData());
}

void MainWindow::onSimulatorStarted()
{
    waveform_chart_->clearData();
}

void MainWindow::onSimulatorStopped()
{
    // Keep waveform visible; just stop updates
}

// ============================================================================
// External test program launcher
// ============================================================================
void MainWindow::onLaunchTestProgram()
{
    const QString programPath = QDir(QCoreApplication::applicationDirPath())
                                    .absoluteFilePath(QStringLiteral("my_test.exe"));

    if (!QFile::exists(programPath)) {
        test_panel_->appendLog(QStringLiteral("[系统] ⚠ 未找到测试程序"));
        test_panel_->appendLog(QStringLiteral("[系统]   路径: ") + programPath);
        test_panel_->appendLog(QStringLiteral("[系统]   请将编译好的 my_test.exe 放在模拟器目录下"));
        return;
    }

    if (test_process_ && test_process_->state() != QProcess::NotRunning) {
        test_panel_->appendLog(QStringLiteral("[系统] ⚠ 测试程序已在运行中"));
        return;
    }

    if (!test_process_) {
        test_process_ = new QProcess(this);
        test_process_->setProcessChannelMode(QProcess::SeparateChannels);
        connect(test_process_,
                QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this](int exitCode, QProcess::ExitStatus /*status*/) {
                    test_panel_->appendLog(
                        QStringLiteral("[系统] 测试程序已退出 (code=%1)").arg(exitCode));
                });
        connect(test_process_, &QProcess::readyReadStandardOutput, this, [this]() {
            const QString output =
                QString::fromLocal8Bit(test_process_->readAllStandardOutput()).trimmed();
            if (!output.isEmpty())
                test_panel_->appendLog(output);
        });
        connect(test_process_, &QProcess::readyReadStandardError, this, [this]() {
            const QString err =
                QString::fromLocal8Bit(test_process_->readAllStandardError()).trimmed();
            if (!err.isEmpty())
                test_panel_->appendLog(QStringLiteral("[stderr] ") + err);
        });
    }

    test_process_->start(programPath, QStringList(), QIODevice::ReadWrite);
    test_panel_->appendLog(QStringLiteral("[系统] ▶ 启动测试程序"));
}


void MainWindow::onMinimize()
{
    showMinimized();
}

void MainWindow::onMaximize()
{
    if (isMaximized())
        showNormal();
    else
        showMaximized();
}

void MainWindow::onClose()
{
    close();
}

// ============================================================================
// Window painting — rounded corners + background
// ============================================================================
void MainWindow::paintEvent(QPaintEvent *)
{
    // The actual rounded background is handled by the content widget;
    // this outer area is transparent due to WA_TranslucentBackground.
    // We still need the shadow effect which is on the window itself.
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}
