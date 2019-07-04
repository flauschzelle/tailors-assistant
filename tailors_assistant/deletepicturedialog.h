#ifndef DELETEPICTUREDIALOG_H
#define DELETEPICTUREDIALOG_H

#include <QDialog>

namespace Ui {
class DeletePictureDialog;
}

class DeletePictureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeletePictureDialog(QWidget *parent = 0);
    ~DeletePictureDialog();

private:
    Ui::DeletePictureDialog *ui;
};

#endif // DELETEPICTUREDIALOG_H
