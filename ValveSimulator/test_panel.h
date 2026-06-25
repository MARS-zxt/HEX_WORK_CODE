#ifndef TEST_PANEL_H
#define TEST_PANEL_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

// ============================================================================
// TestPanel — user test-program interface
// ============================================================================
// Provides:
//   1. A button to launch an external test program (via callback)
//   2. Six labeled data-display fields for test results
//   3. A log output area with clear button
// ============================================================================
class TestPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TestPanel(QWidget *parent = nullptr);

    // --- Data display setters (exposed for external C API) -----------------
    void setCloseSmallTravelTime(double val);
    void setCloseCurrentUpper(double val);
    void setCloseCurrentLower(double val);
    void setOpenSmallTravelTime(double val);
    void setOpenCurrentUpper(double val);
    void setOpenCurrentLower(double val);

    // --- Log output --------------------------------------------------------
    void appendLog(const QString &text);
    void clearLog();

    // --- Test button callback ----------------------------------------------
    void setTestButtonCallback(void (*callback)());

signals:
    void testButtonClicked();

private slots:
    void onTestButtonClicked();

private:
    void setupUi();

    // Data display labels
    QLabel *close_time_val_  = nullptr;
    QLabel *close_upper_val_ = nullptr;
    QLabel *close_lower_val_ = nullptr;
    QLabel *open_time_val_   = nullptr;
    QLabel *open_upper_val_  = nullptr;
    QLabel *open_lower_val_  = nullptr;

    // Log
    QTextEdit *log_output_ = nullptr;
    QPushButton *clear_log_btn_ = nullptr;
    QPushButton *test_btn_ = nullptr;

    void (*test_callback_)() = nullptr;
};

#endif // TEST_PANEL_H
