#ifndef VERSION_DIALOG_H
#define VERSION_DIALOG_H

#include <QDialog>

// ============================================================================
// VersionDialog — frameless modal, shows version + changelog
// ============================================================================
class VersionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VersionDialog(QWidget *parent = nullptr);

private:
    void setupUi();
};

#endif // VERSION_DIALOG_H
