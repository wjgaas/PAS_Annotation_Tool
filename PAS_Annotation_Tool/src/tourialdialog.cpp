#include "tourialdialog.h"
#include "ui_tourialdialog.h"

TourialDialog::TourialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TourialDialog)
{
    ui->setupUi(this);
}

TourialDialog::~TourialDialog()
{
    delete ui;
}
