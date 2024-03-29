/*  
    Tailor's Assistant - a tool to help with estimating work times as a tailor
    Copyright (C) 2019  by Kirstin Rohwer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

	For contact information, see the author's website <http://metakiki.net>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QIcon>

#include "general.h"
#include "workpiece.h"
#include "workpieceselector.h"
#include "filepathsettingsdialog.h"
#include "steptablemodel.h"
#include "aboutdialog.h"
#include "deletepicturedialog.h"

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

    void openWebsite();
    void openAboutDialog();

    void tryToDeletePicture();
    void openPictureUploadDialog();
    void getPicturePathFromSelector();
    void cleanUpPictureSelector();
protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::MainWindow *ui;

    QString version;

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

    AboutDialog* about_dialog;

    FilePathSettingsDialog *db_settings_dialog;
    FilePathSettingsDialog *export_textfile_dialog;
    FilePathSettingsDialog *picture_upload_dialog;

    QString lastUsedExportPath;
    QString lastUsedPicturePath;

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
