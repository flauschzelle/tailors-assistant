#include "filepathsettingsdialog.h"
#include "ui_filepathsettingsdialog.h"
#include "mainwindow.h"

#include <QFileDialog>

FilePathSettingsDialog::FilePathSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilePathSettingsDialog)
{
    ui->setupUi(this);

    //initialize with default values:
    path = "";
    fileDialogTitle = "Datei auswählen";
    fileDialogDirPath = QDir::homePath();
    fileDialogTypes = "All Files (*)";

    QObject::connect(ui->pushButton, &QPushButton::clicked, this, &FilePathSettingsDialog::setPathFromFilesystem);
    QObject::connect(ui->lineEdit, &QLineEdit::textEdited, this, &FilePathSettingsDialog::setPath);
}

FilePathSettingsDialog::~FilePathSettingsDialog()
{
    delete ui;
}

QString FilePathSettingsDialog::getNewPath()
{
    return path;
}

void FilePathSettingsDialog::setLabelText(const QString& text)
{
    ui->label->setText(text);
}

void FilePathSettingsDialog::setTextAndPath(const QString& text)
{
    ui->lineEdit->setText(text);
    path = text;
}

//slot to be called when the text is edited
void FilePathSettingsDialog::setPath(const QString& text)
{
    path = text;
}

//slot to be called when the browse button is pressed
void FilePathSettingsDialog::setPathFromFilesystem()
{
    QString filePath = QFileDialog::getSaveFileName(this, fileDialogTitle, fileDialogDirPath, fileDialogTypes);
    if (!filePath.isEmpty())
    {
        path = filePath;
    }
    ui->lineEdit->setText(path);
}

void FilePathSettingsDialog::setFileDialogTypes(const QString& value)
{
    fileDialogTypes = value;
}

void FilePathSettingsDialog::setFileDialogDirPath(const QString& value)
{
    fileDialogDirPath = value;
}

void FilePathSettingsDialog::setFileDialogTitle(const QString& value)
{
    fileDialogTitle = value;
}
