#ifndef DELETEPIECEDIALOG_H
#define DELETEPIECEDIALOG_H

#include <QDialog>
#include "general.h"

namespace Ui {
class DeletePieceDialog;
}

class DeletePieceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeletePieceDialog(QWidget *parent = 0);
    ~DeletePieceDialog();

    void setMode(PieceStatusMode mode);

private:
    Ui::DeletePieceDialog *ui;

    PieceStatusMode mode;
};

#endif // DELETEPIECEDIALOG_H
