#include "filepathsettingsdialog.h"
#include "ui_filepathsettingsdialog.h"
#include "mainwindow.h"
#include "general.h"

#include <QFileDialog>

FilePathSettingsDialog::FilePathSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilePathSettingsDialog)
{
    ui->setupUi(this);

    //initialize with default values:
    mode = openFile;
    path = "";
    fileDialogTitle = "Datei auswählen";
    fileDialogDirPath = QDir::homePath();
    fileDialogTypes = "All Files (*)";

    ui->pictureLabel->clear(); //empty the picture label
    ui->pictureLabel->setVisible(false);

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

void FilePathSettingsDialog::setLabelPicture(const QPixmap& picture)
{
    ui->pictureLabel->setPixmap(picture);
}

void FilePathSettingsDialog::setPictureLabelVisibility(bool visible)
{
    ui->pictureLabel->setVisible(visible);
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
    QString filePath;
    //open a file dialog to get the file path:
    if (mode == saveFile)
    {
        filePath = QFileDialog::getSaveFileName(this, fileDialogTitle, fileDialogDirPath, fileDialogTypes);
    }
    if (mode == openFile)
    {
        filePath = QFileDialog::getOpenFileName(this, fileDialogTitle, fileDialogDirPath, fileDialogTypes);
    }
    if (mode == openPicture)
    {
        filePath = QFileDialog::getOpenFileName(this, fileDialogTitle, fileDialogDirPath, fileDialogTypes);
        if (!filePath.isEmpty())
        {
            setLabelPicture(QPixmap(filePath).scaledToHeight(100));
        }
    }
    if (!filePath.isEmpty())
    {
        path = filePath;
        //remember path for possible reselection:
        fileDialogDirPath = QFileInfo(filePath).absolutePath();
    }
    ui->lineEdit->setText(path);
}

void FilePathSettingsDialog::setMode(const FileDialogMode& value)
{
    mode = value;
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
