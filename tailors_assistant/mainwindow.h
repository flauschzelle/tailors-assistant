#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "general.h"
#include "workpiece.h"
#include "workpieceselector.h"
#include "databasesettings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setInputMode(PieceStatusMode mode);

    QString getDatabasePath();

    static MainWindow* instance;

    QString getDatabaseDirPath() const;

    void setCurrentPiece(WorkPiece * piece);

public slots:
    void newPiece();
    void newOffer();

    void openPieceSelector();
    void openOfferSelector();

    void getWorkpieceFromSelector();
    void cleanUpSelector();

    void openDatabaseSettings();
    void changeDatabasePath(QString path);
    void getDBPathFromSelector();
    void cleanUpDBSelector();
    void tryToDeleteCurrentPiece();
    void deleteCurrentPiece();

    void activateStepEdits();

    void nextStep();
    void prevStep();

private:
    Ui::MainWindow *ui;

    PieceStatusMode mode;

    QStandardItemModel *test_model;

    WorkPiece *currentPiece;

    int stepIndex;

    WorkPieceSelector *selector;

    QSqlDatabase db;

    QString databasePath;
    QString databaseDirPath;

    QFile configfile;

    DatabaseSettings *db_settings_dialog;

    void setupConfigFile();
    void initDatabase();
    void setupDatabase();

    void fillPieceDataComboBoxes();
    void fillPieceDataUIElements(WorkPiece * piece);
    void connectPieceDataInputs();

    void fillStepDataComboBoxes();
    void fillStepDataUIElements(Step * step);
    void connectStepDataInputs(Step* step);



};

#endif // MAINWINDOW_H
