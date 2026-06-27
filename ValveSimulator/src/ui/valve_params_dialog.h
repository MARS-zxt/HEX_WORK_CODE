#ifndef VALVE_PARAMS_DIALOG_H
#define VALVE_PARAMS_DIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include "valve_types.h"

// ============================================================================
// ValveParamsDialog — modal dialog for editing valve parameters
// ============================================================================
class ValveParamsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ValveParamsDialog(const ValveParams &current, QWidget *parent = nullptr);

    ValveParams getParams() const;

private:
    void setupUi(const ValveParams &params);
    void applyStyle();

    QDoubleSpinBox *startup_time_   = nullptr;
    QDoubleSpinBox *travel_time_    = nullptr;
    QDoubleSpinBox *running_current_= nullptr;
    QDoubleSpinBox *stall_current_  = nullptr;
    QDoubleSpinBox *startup_current_= nullptr;
};

#endif // VALVE_PARAMS_DIALOG_H
