#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMessageBox>

#include "general.h"
#include "workpiece.h"
#include "workpieceselector.h"
#include "filepathsettingsdialog.h"
#include "steptablemodel.h"

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

    QString getDatabaseDirPath() const;

    void setCurrentPiece(WorkPiece * piece);

public slots:

    void newPiece();
    void newOffer();

    void newPieceFromOffer();
    void newOfferFromPiece();

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

    void tryToDeleteCurrentStep();
    void deleteCurrentStep();

    void stepSelected(const QItemSelection& selected, const QItemSelection &);
    void stepMovedUp();
    void stepMovedDown();

    void calculateStepStatistics();
    void setPricePerHour(double value);
    void setDisplayedSums();

    void openFileExportDialog();
    void cleanUpExportSelector();
    void getExportPathFromSelector();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::MainWindow *ui;

    PieceStatusMode mode;

    StepTableModel *steps_model;
    QVector<Step*>* empty_step_list;

    WorkPiece *currentPiece;

    int stepIndex;

    WorkPieceSelector *selector;

    QSqlDatabase db;

    QString databasePath;
    QString databaseDirPath;

    QFile configfile;

    FilePathSettingsDialog *db_settings_dialog;
    FilePathSettingsDialog *export_textfile_dialog;

    QString lastUsedExportPath;

    double pricePerHour;

    void setupConfigFile();
    void initDatabase();
    void setupDatabase();

    void fillPieceDataComboBoxes();
    void fillPieceDataUIElements(WorkPiece * piece);
    void connectPieceDataInputs();
    void disconnectPieceDataInputs();

    void fillStepDataComboBoxes();
    void fillStepDataUIElements(Step * step);
    void connectStepDataInputs(Step* step);
    void disconnectStepDataInputs(Step* step);

};

#endif // MAINWINDOW_H
