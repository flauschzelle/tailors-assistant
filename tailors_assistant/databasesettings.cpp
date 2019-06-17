#include "databasesettings.h"
#include "ui_databasesettings.h"
#include "mainwindow.h"

#include <QFileDialog>

DatabaseSettings::DatabaseSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseSettings)
{
    ui->setupUi(this);
    this->setWindowTitle("Datenbank-Einstellungen");

    path = MainWindow::instance->getDatabasePath();
    ui->lineEdit->setText(path);

    QObject::connect(ui->pushButton, &QPushButton::clicked, this, &DatabaseSettings::setPathFromFilesystem);
    QObject::connect(ui->lineEdit, &QLineEdit::textEdited, this, &DatabaseSettings::setPath);
}

DatabaseSettings::~DatabaseSettings()
{
    delete ui;
}

QString DatabaseSettings::getNewPath()
{
    return path;
}

//slot to be called when the text is edited
void DatabaseSettings::setPath(const QString& text)
{
    path = text;
}

//slot to be called when the browse button is pressed
void DatabaseSettings::setPathFromFilesystem()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Datenbank-Datei auswählen", MainWindow::instance->getDatabaseDirPath(), "Database Files (*.db)");
    if (!filePath.isEmpty())
    {
        path = filePath;
    }
    ui->lineEdit->setText(path);
}
