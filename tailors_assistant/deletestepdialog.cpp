#include "deletestepdialog.h"
#include "ui_deletestepdialog.h"

DeleteStepDialog::DeleteStepDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteStepDialog)
{
    ui->setupUi(this);
}

DeleteStepDialog::~DeleteStepDialog()
{
    delete ui;
}
