#include "deletepicturedialog.h"
#include "ui_deletepicturedialog.h"

DeletePictureDialog::DeletePictureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeletePictureDialog)
{
    ui->setupUi(this);
}

DeletePictureDialog::~DeletePictureDialog()
{
    delete ui;
}
