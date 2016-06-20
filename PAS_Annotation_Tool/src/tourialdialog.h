#ifndef TOURIALDIALOG_H
#define TOURIALDIALOG_H

#include <QDialog>

namespace Ui {
class TourialDialog;
}

class TourialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TourialDialog(QWidget *parent = 0);
    ~TourialDialog();

private:
    Ui::TourialDialog *ui;
};

#endif // TOURIALDIALOG_H
