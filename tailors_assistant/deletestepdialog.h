#ifndef DELETESTEPDIALOG_H
#define DELETESTEPDIALOG_H

#include <QDialog>

namespace Ui {
class DeleteStepDialog;
}

class DeleteStepDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteStepDialog(QWidget *parent = 0);
    ~DeleteStepDialog();

private:
    Ui::DeleteStepDialog *ui;
};

#endif // DELETESTEPDIALOG_H
