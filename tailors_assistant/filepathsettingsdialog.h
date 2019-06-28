#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include <QDialog>

namespace Ui {
class FilePathSettingsDialog;
}

class FilePathSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePathSettingsDialog(QWidget *parent = 0);
    ~FilePathSettingsDialog();

    QString getNewPath();

    void setTextAndPath(const QString& text);
    void setLabelText(const QString& text);
    void setFileDialogTitle(const QString& value);

    void setFileDialogDirPath(const QString& value);

    void setFileDialogTypes(const QString& value);

public slots:
    void setPath(const QString &text);
    void setPathFromFilesystem();

private:
    Ui::FilePathSettingsDialog *ui;

    QString path;
    QString fileDialogTitle;
    QString fileDialogDirPath;
    QString fileDialogTypes;
};

#endif // DATABASESETTINGS_H
