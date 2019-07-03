#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    void setVersionText(QString text);
    void setInfoText(QString text);
    void setTitle(QString title);

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
