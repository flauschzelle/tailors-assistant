#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::setVersionText(QString text)
{
    ui->versionLabel->setText(text);
}

void AboutDialog::setInfoText(QString text)
{
   ui->infoLabel->setText(text);
   ui->infoLabel->setTextFormat(Qt::RichText);
   ui->infoLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
   ui->infoLabel->setOpenExternalLinks(true);
}

void AboutDialog::setTitle(QString title)
{
    this->setWindowTitle(title);
}
