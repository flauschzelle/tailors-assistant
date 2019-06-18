#include "deletepiecedialog.h"
#include "ui_deletepiecedialog.h"

DeletePieceDialog::DeletePieceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeletePieceDialog)
{
    ui->setupUi(this);
}

DeletePieceDialog::~DeletePieceDialog()
{
    delete ui;
}

void DeletePieceDialog::setMode(PieceStatusMode mode)
{
    this->mode = mode;
    if (mode == record)
    {
        ui->label->setText("Aktuelles Werkstück wirklich löschen?");
    }
    if (mode == offer)
    {
        ui->label->setText("Aktuelles Angebot wirklich löschen?");
    }
}
