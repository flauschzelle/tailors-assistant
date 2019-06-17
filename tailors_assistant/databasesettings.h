#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include <QDialog>

namespace Ui {
class DatabaseSettings;
}

class DatabaseSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseSettings(QWidget *parent = 0);
    ~DatabaseSettings();

    QString getNewPath();

public slots:
    void setPath(const QString &text);
    void setPathFromFilesystem();

private:
    Ui::DatabaseSettings *ui;

    QString path;
};

#endif // DATABASESETTINGS_H
