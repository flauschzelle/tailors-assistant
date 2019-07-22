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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workpieceselector.h"
#include "workpiece.h"
#include "step.h"
#include "deletepiecedialog.h"
#include "deletestepdialog.h"
#include "steptablemodel.h"
#include "aboutdialog.h"

#include <QStandardItemModel>
#include <QStringList>
#include <QFile>
#include <QCloseEvent>
#include <QDesktopServices>

// constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupConfigFile();
    initDatabase();
    setupDatabase();
    fillPieceDataComboBoxes();

    //set version:
    version = "Version 0.1.0";

    //model for table view:
    empty_step_list = new QVector<Step*>();

    steps_model = new StepTableModel();
    steps_model->setMode(record); //default value
    steps_model->setDataSource(empty_step_list);

    ui->dataTableView->setModel(steps_model);
    ui->dataTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->dataTableView->resizeColumnsToContents();

    //default visibility:
    ui->resultViewWidget->setEnabled(false);

    ui->pieceDataBox->setEnabled(false);
    ui->stepDataBox->setEnabled(false);
    setInputMode(record); //default value for input mode

    //init with default/null values:
    currentPiece = NULL;
    stepIndex = 0;

    selector = NULL;
    about_dialog = NULL;
    db_settings_dialog = NULL;
    export_textfile_dialog = NULL;
    picture_upload_dialog = NULL;

    lastUsedExportPath = QDir::homePath() + "/tmp/tailors_assistant/werkstueck.txt"; //initialize with default value
    lastUsedPicturePath = QDir::homePath(); //initialize with default value

    pricePerHour = 35.70; //default price
    ui->priceDoubleSpinBox->setValue(pricePerHour); //show default price
    ui->sumsDisplayLabel->setText(""); //set sums label to empty

    //connections to ui:
    QObject::connect(ui->actionNeues_Werkst_ck, &QAction::triggered, this, &MainWindow::newPiece);
    QObject::connect(ui->actionNeues_Angebot, &QAction::triggered, this, &MainWindow::newOffer);

    QObject::connect(ui->actionWerkst_ck_ffnen, &QAction::triggered, this, &MainWindow::openPieceSelector);
    QObject::connect(ui->actionAngebot_ffnen, &QAction::triggered, this, &MainWindow::openOfferSelector);

    QObject::connect(ui->actionWerkst_ck_aus_Angebot, &QAction::triggered, this, &MainWindow::newPieceFromOffer);
    QObject::connect(ui->actionAngebot_aus_Werkst_ck, &QAction::triggered, this, &MainWindow::newOfferFromPiece);

    QObject::connect(ui->actionDatenbank_Einstellungen, &QAction::triggered, this, &MainWindow::openDatabaseSettings);

    QObject::connect(ui->actionOnline_Dokumentation, &QAction::triggered, this, &MainWindow::openWebsite);
    QObject::connect(ui->action_ber_Tailor_s_Assistant, &QAction::triggered, this, &MainWindow::openAboutDialog);

    QObject::connect(ui->addPictureToolButton, &QToolButton::clicked, this, &MainWindow::openPictureUploadDialog);
    QObject::connect(ui->deletePictureToolButton, &QToolButton::clicked, this, &MainWindow::tryToDeletePicture);

    QObject::connect(ui->deletePiecePushButton, &QPushButton::clicked, this, &MainWindow::tryToDeleteCurrentPiece);
    QObject::connect(ui->editStepsPushButton, &QPushButton::clicked, this, &MainWindow::activateStepEdits);

    QObject::connect(ui->exportPiecePushButton, &QPushButton::clicked, this, &MainWindow::openFileExportDialog);

    QObject::connect(ui->previousStepPushButton, &QPushButton::clicked, this, &MainWindow::prevStep);
    QObject::connect(ui->nextStepPushButton, &QPushButton::clicked, this, &MainWindow::nextStep);

    QObject::connect(ui->deleteStepPushButton, &QPushButton::clicked, this, &MainWindow::tryToDeleteCurrentStep);

    QObject::connect(ui->upToolButton, &QToolButton::clicked, this, &MainWindow::stepMovedUp);
    QObject::connect(ui->downToolButton, &QToolButton::clicked, this, &MainWindow::stepMovedDown);

    QObject::connect(ui->priceDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::setPricePerHour);
}

// destructor
MainWindow::~MainWindow()
{
    db.close();
    delete ui;
    delete empty_step_list;
    delete currentPiece;
    delete steps_model;
    delete selector;
}

//reimplementation of closeEvent
//so that the current work is auto-saved before closing:
void MainWindow::closeEvent(QCloseEvent *event)
{
    //save current piece
    if (currentPiece != NULL && !currentPiece->isEmpty())
    {
        currentPiece->savePieceToDatabase();
    }
    event->accept();
}

// sets the mode for record input or offer calculation
void MainWindow::setInputMode(PieceStatusMode mode)
{
    this->mode = mode;

    if (mode == record)
    {
        ui->pieceDataBox->setTitle("Werkstück-Stammdaten");
        ui->filterWidget->setVisible(false);
        ui->pictureWidget->setVisible(true);
        ui->deletePiecePushButton->setText("Werkstück löschen");

        steps_model->setMode(record);
    }

    if (mode == offer)
    {
        ui->pieceDataBox->setTitle("Angebots-Stammdaten");
        ui->filterWidget->setVisible(true);
        ui->pictureWidget->setVisible(false);
        ui->deletePiecePushButton->setText("Angebot löschen");

        steps_model->setMode(offer);
    }
}

//returns the currently used database path
QString MainWindow::getDatabasePath()
{
    return databasePath;
}

//slot function to start recording a new piece:
void MainWindow::newPiece()
{
    if (currentPiece != NULL && !currentPiece->isEmpty())
    {
        currentPiece->savePieceToDatabase();
    }
    //setInputMode(record);
    WorkPiece * newpiece = new WorkPiece();
    newpiece->setStatus(record);
    setCurrentPiece(newpiece);
}

//slot function to start creating a new offer:
void MainWindow::newOffer()
{
    if (currentPiece != NULL && !currentPiece->isEmpty())
    {
        currentPiece->savePieceToDatabase();
    }
    //setInputMode(offer);
    WorkPiece * newpiece = new WorkPiece();
    newpiece->setStatus(offer);
    setCurrentPiece(newpiece);
}

//slot function to convert an existing offer into a new piece
void MainWindow::newPieceFromOffer()
{
    //setInputMode(record);
    if (selector != NULL)
    {
        delete selector;
    }
    if (currentPiece != NULL && !currentPiece->isEmpty())
    {
        currentPiece->savePieceToDatabase();
    }
    selector = new WorkPieceSelector(this);
    selector->setSelectionMode(offer);
    selector->setConversionMode(offer_to_record);
    selector->open();
    QObject::connect(selector, &QDialog::accepted, this, &MainWindow::getWorkpieceFromSelector);
    QObject::connect(selector, &QDialog::rejected, this, &MainWindow::cleanUpSelector);
}

//slot function to create a new offer from the data of an existing piece
void MainWindow::newOfferFromPiece()
{
    //setInputMode(offer);
    if (selector != NULL)
    {
        delete selector;
    }
    if (currentPiece != NULL && !currentPiece->isEmpty())
    {
        currentPiece->savePieceToDatabase();
    }
    selector = new WorkPieceSelector(this);
    selector->setSelectionMode(record);
    selector->setConversionMode(record_copy_to_offer);
    selector->open();
    QObject::connect(selector, &QDialog::accepted, this, &MainWindow::getWorkpieceFromSelector);
    QObject::connect(selector, &QDialog::rejected, this, &MainWindow::cleanUpSelector);
}

//slot function for a selection view to open an existing piece:
void MainWindow::openPieceSelector()
{
    if (selector != NULL)
    {
        delete selector;
    }
    if (currentPiece != NULL && !currentPiece->isEmpty())
    {
        currentPiece->savePieceToDatabase();
    }
    selector = new WorkPieceSelector(this);
    selector->setSelectionMode(record);
    selector->setConversionMode(none);
    selector->open();
    QObject::connect(selector, &QDialog::accepted, this, &MainWindow::getWorkpieceFromSelector);
    QObject::connect(selector, &QDialog::rejected, this, &MainWindow::cleanUpSelector);
}

//slot function for a selection view to open an existing offer:
void MainWindow::openOfferSelector()
{
    if (selector != NULL)
    {
        delete selector;
    }
    if (currentPiece != NULL && !currentPiece->isEmpty())
    {
        currentPiece->savePieceToDatabase();
    }
    selector = new WorkPieceSelector(this);
    selector->setSelectionMode(offer);
    selector->setConversionMode(none);
    selector->open();
    QObject::connect(selector, &QDialog::accepted, this, &MainWindow::getWorkpieceFromSelector);
    QObject::connect(selector, &QDialog::rejected, this, &MainWindow::cleanUpSelector);

}

//slot function for applying the selection from the workpiece selector:
void MainWindow::getWorkpieceFromSelector()
{
    if (currentPiece !=NULL && !currentPiece->isEmpty())
    {
        currentPiece->savePieceToDatabase();
    }
    setCurrentPiece(selector->getSelectedPiece());
    if (!currentPiece->getSteps().isEmpty())
    {
            activateStepEdits();
    }
    delete selector;
    selector = NULL; //re-initialize selector to null pointer
    //for debugging:
    //printf("selected piece: %s (%d)\n", currentPiece->getName().toStdString().c_str(), currentPiece->getId());
}

//slot function for cleaning up the selector window's object
void MainWindow::cleanUpSelector()
{
    delete selector;
    selector = NULL; //re-initialize selector to null pointer
}

//to be called in the constructor
void MainWindow::initDatabase()
{
    QFileInfo fi(databasePath);
    databaseDirPath = fi.absolutePath();

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databasePath);
    bool ok = db.open();
    if (!ok) //check if connection was successful and exit if no
    {
        printf ("Error: unable to open database connection");
        exit (EXIT_FAILURE);
    }
}

//slot function to open a dialog for exporting a text file
void MainWindow::openFileExportDialog()
{
    export_textfile_dialog = new FilePathSettingsDialog(this);
    export_textfile_dialog->setTextAndPath(lastUsedExportPath);
    export_textfile_dialog->setWindowTitle("Textdatei exportieren");
    QString labeltext = "In diese Datei exportieren \n"
                        "(falls die Datei bereits existiert, wird der Inhalt überschrieben):";
    export_textfile_dialog->setLabelText(labeltext);
    export_textfile_dialog->setFileDialogTitle("Textdatei zum Export auswählen");
    export_textfile_dialog->setMode(saveFile);
    export_textfile_dialog->setFileDialogDirPath(QFileInfo(lastUsedExportPath).absolutePath());
    export_textfile_dialog->setFileDialogTypes("Text Files (*.txt)");
    export_textfile_dialog->open();
    QObject::connect(export_textfile_dialog, &QDialog::accepted, this, &MainWindow::getExportPathFromSelector);
    QObject::connect(export_textfile_dialog, &QDialog::rejected, this, &MainWindow::cleanUpExportSelector);
}

//slot
void MainWindow::getExportPathFromSelector()
{
    QString path = export_textfile_dialog->getNewPath();
    if (currentPiece != NULL && path != (""))
    {
        currentPiece->savePieceToDatabase(); //auto-save for security,
        //then let the piece export itself:
        bool ok = currentPiece->exportToTextfile(path);
        if (!ok)
        {
            QMessageBox::critical(this, "Fehler", "Die Datei konnte nicht gespeichert werden.");
        }
        lastUsedExportPath = path; //remember the path for next time
    }
    delete export_textfile_dialog;
    export_textfile_dialog = NULL; //re-initialize selector to null pointer
}

void MainWindow::openWebsite()
{
    QDesktopServices::openUrl(QUrl("http://metakiki.net/tailors-assistant/"));
}

//slot
void MainWindow::cleanUpExportSelector()
{
    delete export_textfile_dialog;
    export_textfile_dialog = NULL; //re-initialize selector to null pointer
}

//slot function to open a dialog for uploading a picture
void MainWindow::openPictureUploadDialog()
{
    picture_upload_dialog = new FilePathSettingsDialog(this);
    picture_upload_dialog->setTextAndPath(lastUsedPicturePath);
    picture_upload_dialog->setWindowTitle("Bild aus Datei einfügen");
    QString labeltext = "Bild aus dieser Datei laden:";
    picture_upload_dialog->setLabelText(labeltext);
    picture_upload_dialog->setFileDialogTitle("Bild-Datei auswählen");
    picture_upload_dialog->setMode(openPicture);
    picture_upload_dialog->setPictureLabelVisibility(true);
    picture_upload_dialog->setFileDialogDirPath(QFileInfo(lastUsedPicturePath).absolutePath());
    picture_upload_dialog->setFileDialogTypes("JPEG Image Files (*.jpg *.jpeg *.JPG *.JPEG)");
    picture_upload_dialog->open();
    QObject::connect(picture_upload_dialog, &QDialog::accepted, this, &MainWindow::getPicturePathFromSelector);
    QObject::connect(picture_upload_dialog, &QDialog::rejected, this, &MainWindow::cleanUpPictureSelector);
}

//slot
void MainWindow::getPicturePathFromSelector()
{
    QString path = picture_upload_dialog->getNewPath();
    if (currentPiece != NULL && path != (""))
    {
        currentPiece->savePieceToDatabase(); //auto-save for security
        currentPiece->setPictureFromFile(path); //then set picture
        ui->pictureDisplayLabel->setPixmap(currentPiece->getPicture()); //display the picture
        lastUsedPicturePath = path; //remember the path for next time
    }
    delete picture_upload_dialog;
    picture_upload_dialog = NULL; //re-initialize selector to null pointer
}

//slot
void MainWindow::cleanUpPictureSelector()
{
    delete picture_upload_dialog;
    picture_upload_dialog = NULL; //re-initialize selector to null pointer
}

//public slot for the delete picture button to use
void MainWindow::tryToDeletePicture()
{
    DeletePictureDialog * dpd = new DeletePictureDialog(this);
    dpd->open();
    QObject::connect(dpd, &QDialog::accepted, currentPiece, &WorkPiece::deletePicture);
    QObject::connect(dpd, &QDialog::accepted, ui->pictureDisplayLabel, &QLabel::clear);
    QObject::connect(dpd, &QDialog::rejected, [=](){ delete dpd; });
}

void MainWindow::openAboutDialog()
{
    about_dialog = new AboutDialog(this);
    about_dialog->setTitle("Über Tailor's Assistant");
    about_dialog->setVersionText(version);
    about_dialog->setInfoText("Entwickelt von <a href=\"http://metakiki.net/\">Kirstin Rohwer</a>.<br>"
                              "Mehr Info auf der <a href=\"http://metakiki.net/tailors-assistant/\">Website</a>.<br>"
                              "<br>"
                              "Lizensiert unter <a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">GNU GPLv3</a>.");
    about_dialog->open();
    QObject::connect(about_dialog, &QDialog::accepted, [=](){ delete about_dialog; });
    QObject::connect(about_dialog, &QDialog::rejected, [=](){ delete about_dialog; });
}

//slot function for opening the database settings dialog
void MainWindow::openDatabaseSettings()
{
    db_settings_dialog = new FilePathSettingsDialog(this);
    db_settings_dialog->setTextAndPath(databasePath);
    db_settings_dialog->setWindowTitle("Datenbank-Einstellungen");
    QString labeltext = "Pfad zur SQLite-Datenbank-Datei \n"
                        "(wenn die Datei noch nicht existiert, wird sie automatisch erstellt):";
    db_settings_dialog->setLabelText(labeltext);
    db_settings_dialog->setFileDialogTitle("Datenbank-Datei auswählen");
    db_settings_dialog->setMode(setDatabase);
    db_settings_dialog->setFileDialogDirPath(databaseDirPath);
    db_settings_dialog->setFileDialogTypes("Database Files (*.db)");
    db_settings_dialog->open();
    QObject::connect(db_settings_dialog, &QDialog::accepted, this, &MainWindow::getDBPathFromSelector);
    QObject::connect(db_settings_dialog, &QDialog::rejected, this, &MainWindow::cleanUpDBSelector);
}

//slot function for changing the path to the database
void MainWindow::changeDatabasePath(QString path)
{
    if (currentPiece != NULL)
    {
        currentPiece->savePieceToDatabase();
        setCurrentPiece(NULL);
    }

    db.close(); //close existing db connection

    databasePath = path; //set the datbase path to the newly configured path

    QFileInfo fi(databasePath);
    databaseDirPath = fi.absolutePath(); //rewrite the path to the db dir

    //reopen the database connection:
    db.setDatabaseName(databasePath);
    bool ok = db.open();
    if (!ok) //check if connection was successful and exit if no
    {
        printf ("Error: unable to open database connection");
        exit (EXIT_FAILURE);
    }

    //open the config file with write only access:
    configfile.open(QIODevice::WriteOnly);

    //write the new database path to the config file:
    QTextStream out(&configfile);
    out << databasePath;

    configfile.close(); //close connection to the file

    setupDatabase(); //setup the newly connected database
}

void MainWindow::getDBPathFromSelector()
{
    QString path = db_settings_dialog->getNewPath();
    changeDatabasePath(path);

    delete db_settings_dialog;
    db_settings_dialog = NULL; //re-initialize selector to null pointer
}

void MainWindow::cleanUpDBSelector()
{
    delete db_settings_dialog;
    db_settings_dialog = NULL; //re-initialize selector to null pointer
}

QString MainWindow::getDatabaseDirPath() const
{
    return databaseDirPath;
}

//public slot for the delete current step button to use
void MainWindow::tryToDeleteCurrentStep()
{
    DeleteStepDialog *dsd = new DeleteStepDialog(this);
    dsd->open();
    QObject::connect(dsd, &QDialog::accepted, this, &MainWindow::deleteCurrentStep);
    QObject::connect(dsd, &QDialog::rejected, [=](){ delete dsd; });

}

//public slot to actually delete the current step from the database
void MainWindow::deleteCurrentStep()
{
    disconnectStepDataInputs(currentPiece->getSteps().at(stepIndex)); //disconnect inputs
    currentPiece->deleteStep(stepIndex);
    //if that was the last step, go back to the previous one:
    if (stepIndex >= currentPiece->getSteps().length())
    {
        //except if it was the only one, then just empty it...
        if (stepIndex != 0)
        {
            stepIndex = stepIndex - 1;
        }
        else
        {
            Step * newemptystep = new Step();
            currentPiece->addStep(newemptystep);
        }
    }
    //save changes to the database:
    currentPiece->saveStepsToDatabase();
    //display changes in ui:
    calculateStepStatistics();
    fillStepDataUIElements(currentPiece->getSteps().at(stepIndex));
    connectStepDataInputs(currentPiece->getSteps().at(stepIndex));
    setDisplayedSums(); //so the deleted step is also removed from the sums
}

//slot that is called when a step is selected from the table view
void MainWindow::stepSelected(const QItemSelection &selected, const QItemSelection &)
{
    if (selected.indexes().isEmpty())
    {
        printf("error: empty selection\n");
        return;
    }
    //disconnect inputs from previous step
    disconnectStepDataInputs(currentPiece->getSteps().at(stepIndex));
    //update StepIndex from selection
    stepIndex = selected.indexes().at(0).row();
    //dis-/enable prev button for first step/later stpes
    if (stepIndex > 0)
    {
        ui->previousStepPushButton->setEnabled(true);
    }
    else
    {
        ui->previousStepPushButton->setEnabled(false);
    }
    //fill and connect inputs to newly selected step
    calculateStepStatistics();
    fillStepDataUIElements(currentPiece->getSteps().at(stepIndex));
    connectStepDataInputs(currentPiece->getSteps().at(stepIndex));
}

//slot that is called when the "up" button is pressed
void MainWindow::stepMovedUp()
{
    currentPiece->moveStepBack(stepIndex);
    if (stepIndex > 0)
    {
        stepIndex = stepIndex - 1;
    }
    ui->dataTableView->selectRow(stepIndex);
}

//slot that is called when the "down" button is pressed
void MainWindow::stepMovedDown()
{
    currentPiece->moveStepForward(stepIndex);
    if (stepIndex < (currentPiece->getSteps().length()-1))
    {
        stepIndex = stepIndex + 1;
    }
    ui->dataTableView->selectRow(stepIndex);
}

//public slot for the delete current piece button to use
void MainWindow::tryToDeleteCurrentPiece()
{
    DeletePieceDialog *dpd = new DeletePieceDialog();
    dpd->setMode(mode);
    dpd->open();
    QObject::connect(dpd, &QDialog::accepted, this, &MainWindow::deleteCurrentPiece);
    QObject::connect(dpd, &QDialog::rejected, [=](){ delete dpd; });

}

//public slot to actually delete the current piece from the database
void MainWindow::deleteCurrentPiece()
{
    //first, delete the piece's steps:
    if (currentPiece->getId() != 0)
    {
        currentPiece->deleteAllSteps();
        stepIndex = 0;
        currentPiece->saveStepsToDatabase();
    }
    //then, delete piece data from db:
    if (currentPiece->getId() != 0)
    {
        QSqlQuery query;
        query.prepare("DELETE FROM pieces WHERE piece_id=(:id)");
        query.bindValue(":id", currentPiece->getId());
        query.exec();
    }
    //remove current piece data from ui:
    setCurrentPiece(NULL);
}

void MainWindow::activateStepEdits()
{
    currentPiece->savePieceToDatabase();
    ui->editStepsPushButton->setEnabled(false);
    ui->stepDataBox->setEnabled(true);
    ui->previousStepPushButton->setEnabled(false);

    fillStepDataComboBoxes();
    //if the piece has no steps yet, set a new empty step as the first one:
    if (currentPiece->getSteps().isEmpty())
    {
        Step * firstStep = new Step();
        currentPiece->addStep(firstStep);
    }
    //connect everything to the first step:
    stepIndex = 0;
    ui->resultViewWidget->setEnabled(true);
    ui->dataTableView->selectRow(stepIndex);
    //fillStepDataUIElements(currentPiece->getSteps().at(stepIndex));
    connectStepDataInputs(currentPiece->getSteps().at(stepIndex));
}

//this will be called when the "next step" button is pressed
void MainWindow::nextStep()
{
    currentPiece->saveStepsToDatabase(); //save current input data
    currentPiece->loadStepsFromDatabase(); //reload step data
    disconnectStepDataInputs(currentPiece->getSteps().at(stepIndex));
    stepIndex = stepIndex + 1;
    //(re-)enable prev button:
    ui->previousStepPushButton->setEnabled(true);
    //if the next step does not exist yet, create it:
    if (currentPiece->getSteps().length() <= stepIndex)
    {
        Step * newstep = new Step();
        currentPiece->addStep(newstep);
    }
    //fill and connect inputs:
    fillStepDataComboBoxes();
    ui->dataTableView->selectRow(stepIndex);
    //fillStepDataUIElements(currentPiece->getSteps().at(stepIndex));
    connectStepDataInputs(currentPiece->getSteps().at(stepIndex));
}

//this will be called when the previous step button is pressed
void MainWindow::prevStep()
{
    currentPiece->saveStepsToDatabase(); //save current input data
    currentPiece->loadStepsFromDatabase(); //reload step data
    //go to the previous step only if you're not already at the first one:
    if (stepIndex > 0)
    {
        disconnectStepDataInputs(currentPiece->getSteps().at(stepIndex));
        stepIndex = stepIndex - 1;
    }
    //disable prev button on the first step
    if (stepIndex == 0)
    {
        ui->previousStepPushButton->setEnabled(false);
    }
    //fill and connect inputs:
    fillStepDataComboBoxes();
    ui->dataTableView->selectRow(stepIndex);
    //fillStepDataUIElements(currentPiece->getSteps().at(stepIndex));
    connectStepDataInputs(currentPiece->getSteps().at(stepIndex));
}

void MainWindow::setCurrentPiece(WorkPiece * piece)
{
    if (currentPiece != NULL)
    {
        disconnectPieceDataInputs();
        ui->dataTableView->selectionModel()->clearSelection();
    }
    if (piece != currentPiece)
    {
        delete currentPiece;
    }

    currentPiece = piece;
    stepIndex = 0; //reset step index to start at the first step

    if (piece == NULL)
    {
        ui->pieceDataBox->setEnabled(false);
        steps_model->setDataSource(empty_step_list);
    }
    else
    {
        if (currentPiece != NULL && !currentPiece->isEmpty())
        {
            currentPiece->loadStepsFromDatabase();
        }

        setInputMode(currentPiece->getStatus());
        //activate ui elements:
        ui->pieceDataBox->setEnabled(true);
        fillPieceDataComboBoxes();
        fillPieceDataUIElements(currentPiece);
        connectPieceDataInputs();
        //set data source for table view:
        steps_model->setDataSource(currentPiece->getStepsPointer());
        ui->dataTableView->resizeColumnsToContents();
        //pre-calculate the sums
        setDisplayedSums();
    }

    ui->resultViewWidget->setEnabled(false);
    ui->stepDataBox->setEnabled(false);
    ui->editStepsPushButton->setEnabled(true);

}

void MainWindow::setupDatabase()
{
    //create the tables that are needed, if they don't exist:
    WorkPiece::setupTable();
    Step::setupTable();

    //fill the new database with test data, if it is empty:
    //QSqlQuery query;
    //query.exec("SELECT * FROM pieces");
    //if (!query.next())
    //{
        //WorkPieceSelector::writeTestDataToDatabase();
    //}
}

//slot to be called when the statistics of the current step shall be refreshed
void MainWindow::calculateStepStatistics()
{
    if (mode == offer && currentPiece->getSteps().length() > 0)
    {
        currentPiece->getSteps().at(stepIndex)->calculateStatistics(currentPiece);
    }
}

void MainWindow::setPricePerHour(double value)
{
    pricePerHour = value;
    setDisplayedSums();
}

void MainWindow::setDisplayedSums()
{
    QString sums = "";
    if (mode == record)
    {
        int sum_minutes = 0;
        for (int i=0; i < currentPiece->getSteps().length(); i++)
        {
            sum_minutes += currentPiece->getSteps().at(i)->getMinutesAll();
        }
        int sum_tenth_hours = (sum_minutes+3)/6; //rounded time in 0.1 hour units
        int sum_hours = sum_tenth_hours/10; //number before the decimal point
        int sum_hours_decimal = sum_tenth_hours % 10; //number after the decimal point

        int centsPerHour = pricePerHour*100; //hourly price rate in cents

        int priceInCents = (sum_hours*centsPerHour)+((sum_hours_decimal*centsPerHour)+5)/10;
        int priceInEuro = priceInCents/100; //euro value before the decimal point
        int priceInEuroCents = priceInCents % 100; //cent value after the decimal point

        sums = "" + QString::number(sum_minutes) + " min  |  " + QString::number(sum_hours) + "," +
               QString::number(sum_hours_decimal) + " h  |  " + QString::number(priceInEuro) + "," +
               QString::number(priceInEuroCents) + " €";
    }
    if (mode == offer)
    {
        for (int j=0; j < 5; j++)
        {
            int sum_minutes = 0;
            for (int i=0; i < currentPiece->getSteps().length(); i++)
            {
                switch (j) {
                    case 0:
                        sum_minutes += currentPiece->getSteps().at(i)->getMin();
                        break;
                    case 1:
                        sum_minutes += currentPiece->getSteps().at(i)->getMed();
                        break;
                    case 2:
                        sum_minutes += currentPiece->getSteps().at(i)->getAvg();
                        break;
                    case 3:
                        sum_minutes += currentPiece->getSteps().at(i)->getMax();
                        break;
                    case 4:
                        sum_minutes += currentPiece->getSteps().at(i)->getMinutesAll();
                        break;
                    default:
                        sum_minutes += currentPiece->getSteps().at(i)->getMinutesAll();
                        break;
                }
            }
            int sum_tenth_hours = (sum_minutes+3)/6; //rounded time in 0.1 hour units
            int sum_hours = sum_tenth_hours/10; //number before the decimal point
            int sum_hours_decimal = sum_tenth_hours % 10; //number after the decimal point

            int centsPerHour = pricePerHour*100; //hourly price rate in cents

            int priceInCents = (sum_hours*centsPerHour)+((sum_hours_decimal*centsPerHour)+5)/10;
            int priceInEuro = priceInCents/100; //euro value before the decimal point
            int priceInEuroCents = priceInCents % 100; //cent value after the decimal point
            QString prefix = "";
            switch (j) {
                case 0:
                    prefix = "min:  ";
                    break;
                case 1:
                    prefix = "\nmed:  ";
                    break;
                case 2:
                    prefix = "\navg:  ";
                    break;
                case 3:
                    prefix = "\nmax:  ";
                    break;
                case 4:
                    prefix = "\nman:  ";
                    break;
                default:
                    prefix = "\nman:  ";
                    break;
            }

            QString smin = QString::number(sum_minutes);
            while (smin.length() < 4) {smin = " " + smin;}
            QString shr = QString::number(sum_hours);
            while (shr.length() < 2) {shr = " " + shr;}
            QString seur = QString::number(priceInEuro);
            while (seur.length() < 4) {seur = " " + seur;}

            QString sum = prefix + smin + " min  |  " + shr + "," +
                   QString::number(sum_hours_decimal) + " h  |  " + seur + "," +
                   QString::number(priceInEuroCents) + " €";
            sums = sums + sum;
        }
    }
    ui->sumsDisplayLabel->setText(sums);
    currentPiece->setSums(sums); //give the piece its sums for file export purposes
}

//fill the comboBoxes with data from the db:
void MainWindow::fillPieceDataComboBoxes()
{
    ui->customerComboBox->clear();
    QStringList customers;
    QSqlQuery query;
    query.exec("SELECT DISTINCT customer FROM pieces ORDER BY customer COLLATE NOCASE");
    while (query.next())
    {
        customers.append(query.value(0).toString());
    }
    ui->customerComboBox->addItems(customers);


    ui->pieceTypeComboBox->clear();
    QStringList types;
    query.exec("SELECT DISTINCT type FROM pieces ORDER BY type COLLATE NOCASE");
    while (query.next())
    {
        types.append(query.value(0).toString());
    }
    ui->pieceTypeComboBox->addItems(types);

}

void MainWindow::fillPieceDataUIElements(WorkPiece* piece)
{
    ui->customerComboBox->setCurrentText(piece->getCustomer());
    ui->pieceNameLineEdit->setText(piece->getName());
    ui->pieceTypeComboBox->setCurrentText(piece->getType());
    ui->dateEdit->setDate(piece->getDate());
    ui->pieceCommentLineEdit->setText(piece->getComment());
    if (!piece->getPicture().isNull())
    {
        ui->pictureDisplayLabel->setPixmap(piece->getPicture());
    }
    else
    {
        ui->pictureDisplayLabel->clear();
    }
}

//disconnects ui elements from the current piece
void MainWindow::disconnectPieceDataInputs()
{
    QObject::disconnect(ui->customerComboBox, QOverload<const QString &>::of(&QComboBox::activated),
                     currentPiece, &WorkPiece::setCustomer);
    QObject::disconnect(ui->customerComboBox, &QComboBox::editTextChanged, currentPiece, &WorkPiece::setCustomer);
    QObject::disconnect(ui->pieceNameLineEdit, &QLineEdit::textEdited, currentPiece, &WorkPiece::setName);
    QObject::disconnect(ui->pieceTypeComboBox, QOverload<const QString &>::of(&QComboBox::activated),
                     currentPiece, &WorkPiece::setType);
    QObject::disconnect(ui->pieceTypeComboBox, &QComboBox::editTextChanged, currentPiece, &WorkPiece::setType);
    QObject::disconnect(ui->dateEdit, &QDateEdit::dateChanged, currentPiece, &WorkPiece::setDate);
    QObject::disconnect(ui->pieceCommentLineEdit, &QLineEdit::textEdited, currentPiece, &WorkPiece::setComment);
    //refresh the table view when a step is added, deleted or moved:
    QObject::disconnect(currentPiece, &WorkPiece::stepOrderChanged, steps_model, &StepTableModel::stepOrderChanged);
    //load the seleceted step when a step is selected from the table:
    QObject::disconnect(ui->dataTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::stepSelected);
}

//connects ui elements to the current piece
void MainWindow::connectPieceDataInputs()
{
    QObject::connect(ui->customerComboBox, QOverload<const QString &>::of(&QComboBox::activated),
                     currentPiece, &WorkPiece::setCustomer);
    QObject::connect(ui->customerComboBox, &QComboBox::editTextChanged, currentPiece, &WorkPiece::setCustomer);
    QObject::connect(ui->pieceNameLineEdit, &QLineEdit::textEdited, currentPiece, &WorkPiece::setName);
    QObject::connect(ui->pieceTypeComboBox, QOverload<const QString &>::of(&QComboBox::activated),
                     currentPiece, &WorkPiece::setType);
    QObject::connect(ui->pieceTypeComboBox, &QComboBox::editTextChanged, currentPiece, &WorkPiece::setType);
    QObject::connect(ui->dateEdit, &QDateEdit::dateChanged, currentPiece, &WorkPiece::setDate);
    QObject::connect(ui->pieceCommentLineEdit, &QLineEdit::textEdited, currentPiece, &WorkPiece::setComment);
    //refresh the table view when a step is added, deleted or moved:
    QObject::connect(currentPiece, &WorkPiece::stepOrderChanged, steps_model, &StepTableModel::stepOrderChanged);
    //load the seleceted step when a step is selected from the table:
    QObject::connect(ui->dataTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::stepSelected);
}

//fills the drop down boxes with the values to choose from
void MainWindow::fillStepDataComboBoxes()
{
    ui->stepNameComboBox->clear();
    QStringList stepNames;
    QSqlQuery query;
    query.exec("SELECT DISTINCT name FROM steps ORDER BY name COLLATE NOCASE");
    while (query.next())
    {
        stepNames.append(query.value(0).toString());
    }
    ui->stepNameComboBox->addItems(stepNames);

    ui->seamTypeComboBox->clear();
    QStringList seamTypes;
    query.exec("SELECT DISTINCT seam_type FROM steps ORDER BY seam_type COLLATE NOCASE");
    while (query.next())
    {
        seamTypes.append(query.value(0).toString());
    }
    ui->seamTypeComboBox->addItems(seamTypes);

    ui->materialComboBox->clear();
    QStringList materials;
    query.exec("SELECT DISTINCT material FROM steps ORDER BY material COLLATE NOCASE");
    while (query.next())
    {
        materials.append(query.value(0).toString());
    }
    ui->materialComboBox->addItems(materials);

    ui->detailComboBox->clear();
    QStringList details;
    query.exec("SELECT DISTINCT detail FROM steps ORDER BY detail COLLATE NOCASE");
    while (query.next())
    {
        details.append(query.value(0).toString());
    }
    ui->detailComboBox->addItems(details);
}

//fills in the step data from the given step
void MainWindow::fillStepDataUIElements(Step* step)
{
    ui->stepNameComboBox->setCurrentText(step->getName());
    ui->stepCountSpinBox->setValue(step->getCount());
    ui->seamTypeComboBox->setCurrentText(step->getSeamType());
    ui->materialComboBox->setCurrentText(step->getMaterial());
    ui->detailComboBox->setCurrentText(step->getDetail());
    ui->seamTypeCheckBox->setChecked(step->getFilterSeamType());
    ui->materialCheckBox->setChecked(step->getFilterMaterial());
    ui->detailCheckBox->setChecked(step->getFilterDetail());
    ui->pieceTypeCheckBox->setChecked(step->getFilterPieceType());
    ui->stepCommentLineEdit->setText(step->getComment());
    ui->timeSpinBox->setValue(step->getMinutesAll());
    //select the current step in the table view
    //ui->dataTableView->selectRow(stepIndex);
}

//disconnects the step data inputs from the given step:
void MainWindow::disconnectStepDataInputs(Step * step)
{
    //inputs
    QObject::disconnect(ui->stepNameComboBox, QOverload<const QString &>::of(&QComboBox::activated), step, &Step::setName);
    QObject::disconnect(ui->stepNameComboBox, &QComboBox::editTextChanged, step, &Step::setName);

    QObject::disconnect(ui->seamTypeComboBox, QOverload<const QString &>::of(&QComboBox::activated), step, &Step::setSeamType);
    QObject::disconnect(ui->seamTypeComboBox, &QComboBox::editTextChanged, step, &Step::setSeamType);

    QObject::disconnect(ui->materialComboBox, QOverload<const QString &>::of(&QComboBox::activated), step, &Step::setMaterial);
    QObject::disconnect(ui->materialComboBox, &QComboBox::editTextChanged, step, &Step::setMaterial);

    QObject::disconnect(ui->detailComboBox, QOverload<const QString &>::of(&QComboBox::activated), step, &Step::setDetail);
    QObject::disconnect(ui->detailComboBox, &QComboBox::editTextChanged, step, &Step::setDetail);

    QObject::disconnect(ui->stepCountSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), step, &Step::setCount);
    QObject::disconnect(ui->timeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), step, &Step::setMinutesAll);

    QObject::disconnect(ui->seamTypeCheckBox, &QCheckBox::toggled, step, &Step::setFilterSeamType);
    QObject::disconnect(ui->materialCheckBox, &QCheckBox::toggled, step, &Step::setFilterMaterial);
    QObject::disconnect(ui->detailCheckBox, &QCheckBox::toggled, step, &Step::setFilterDetail);
    QObject::disconnect(ui->pieceTypeCheckBox, &QCheckBox::toggled, step, &Step::setFilterPieceType);

    QObject::disconnect(ui->stepCommentLineEdit, &QLineEdit::textEdited, step, &Step::setComment);

    //table resizing:
    QObject::disconnect(ui->stepNameComboBox, QOverload<const QString &>::of(&QComboBox::activated), ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->stepNameComboBox, &QComboBox::editTextChanged, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->seamTypeComboBox, QOverload<const QString &>::of(&QComboBox::activated), ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->seamTypeComboBox, &QComboBox::editTextChanged, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->materialComboBox, QOverload<const QString &>::of(&QComboBox::activated), ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->materialComboBox, &QComboBox::editTextChanged, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->detailComboBox, QOverload<const QString &>::of(&QComboBox::activated),ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->detailComboBox, &QComboBox::editTextChanged, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->seamTypeCheckBox, &QCheckBox::toggled, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->materialCheckBox, &QCheckBox::toggled, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->detailCheckBox, &QCheckBox::toggled, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->pieceTypeCheckBox, &QCheckBox::toggled, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::disconnect(ui->stepCommentLineEdit, &QLineEdit::textEdited, ui->dataTableView, &QTableView::resizeColumnsToContents);

    //data changes
    QObject::disconnect(step, &Step::stepDataChanged, steps_model, &StepTableModel::stepDataChanged);
    QObject::disconnect(step, &Step::stepDataChanged, this, &MainWindow::setDisplayedSums);

    //statistics
    QObject::disconnect(ui->stepNameComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->stepNameComboBox, &QComboBox::editTextChanged, this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->seamTypeComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->seamTypeComboBox, &QComboBox::editTextChanged, this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->materialComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->materialComboBox, &QComboBox::editTextChanged, this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->detailComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->detailComboBox, &QComboBox::editTextChanged, this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->stepCountSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->timeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->seamTypeCheckBox, &QCheckBox::toggled, this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->materialCheckBox, &QCheckBox::toggled, this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->detailCheckBox, &QCheckBox::toggled, this, &MainWindow::calculateStepStatistics);
    QObject::disconnect(ui->pieceTypeCheckBox, &QCheckBox::toggled, this, &MainWindow::calculateStepStatistics);

}

//connects the step data inputs to the given step
void MainWindow::connectStepDataInputs(Step * step)
{
    //save step data when input:
    QObject::connect(ui->stepNameComboBox, QOverload<const QString &>::of(&QComboBox::activated), step, &Step::setName);
    QObject::connect(ui->stepNameComboBox, &QComboBox::editTextChanged, step, &Step::setName);

    QObject::connect(ui->seamTypeComboBox, QOverload<const QString &>::of(&QComboBox::activated), step, &Step::setSeamType);
    QObject::connect(ui->seamTypeComboBox, &QComboBox::editTextChanged, step, &Step::setSeamType);

    QObject::connect(ui->materialComboBox, QOverload<const QString &>::of(&QComboBox::activated), step, &Step::setMaterial);
    QObject::connect(ui->materialComboBox, &QComboBox::editTextChanged, step, &Step::setMaterial);

    QObject::connect(ui->detailComboBox, QOverload<const QString &>::of(&QComboBox::activated), step, &Step::setDetail);
    QObject::connect(ui->detailComboBox, &QComboBox::editTextChanged, step, &Step::setDetail);

    QObject::connect(ui->stepCountSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), step, &Step::setCount);
    QObject::connect(ui->timeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), step, &Step::setMinutesAll);

    QObject::connect(ui->seamTypeCheckBox, &QCheckBox::toggled, step, &Step::setFilterSeamType);
    QObject::connect(ui->materialCheckBox, &QCheckBox::toggled, step, &Step::setFilterMaterial);
    QObject::connect(ui->detailCheckBox, &QCheckBox::toggled, step, &Step::setFilterDetail);
    QObject::connect(ui->pieceTypeCheckBox, &QCheckBox::toggled, step, &Step::setFilterPieceType);

    QObject::connect(ui->stepCommentLineEdit, &QLineEdit::textEdited, step, &Step::setComment);

    //table resizing:
    QObject::connect(ui->stepNameComboBox, QOverload<const QString &>::of(&QComboBox::activated), ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->stepNameComboBox, &QComboBox::editTextChanged, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->seamTypeComboBox, QOverload<const QString &>::of(&QComboBox::activated), ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->seamTypeComboBox, &QComboBox::editTextChanged, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->materialComboBox, QOverload<const QString &>::of(&QComboBox::activated), ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->materialComboBox, &QComboBox::editTextChanged, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->detailComboBox, QOverload<const QString &>::of(&QComboBox::activated),ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->detailComboBox, &QComboBox::editTextChanged, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->seamTypeCheckBox, &QCheckBox::toggled, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->materialCheckBox, &QCheckBox::toggled, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->detailCheckBox, &QCheckBox::toggled, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->pieceTypeCheckBox, &QCheckBox::toggled, ui->dataTableView, &QTableView::resizeColumnsToContents);
    QObject::connect(ui->stepCommentLineEdit, &QLineEdit::textEdited, ui->dataTableView, &QTableView::resizeColumnsToContents);


    //recalculate statistics when step data is input
    QObject::connect(ui->stepNameComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->stepNameComboBox, &QComboBox::editTextChanged, this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->seamTypeComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->seamTypeComboBox, &QComboBox::editTextChanged, this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->materialComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->materialComboBox, &QComboBox::editTextChanged, this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->detailComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->detailComboBox, &QComboBox::editTextChanged, this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->stepCountSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->timeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->seamTypeCheckBox, &QCheckBox::toggled, this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->materialCheckBox, &QCheckBox::toggled, this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->detailCheckBox, &QCheckBox::toggled, this, &MainWindow::calculateStepStatistics);
    QObject::connect(ui->pieceTypeCheckBox, &QCheckBox::toggled, this, &MainWindow::calculateStepStatistics);

    //refresh table view when data is changed:
    QObject::connect(step, &Step::stepDataChanged, steps_model, &StepTableModel::stepDataChanged);
    //refresh sums when data is changed:
    QObject::connect(step, &Step::stepDataChanged, this, &MainWindow::setDisplayedSums);

}

void MainWindow::setupConfigFile()
{
    QString homepath = QDir::homePath(); //get path to the user's home dir
    databasePath = (homepath + "/.tailors_assistant/testdb.db"); //default path to the database file

    const QString filename = (homepath + "/.tailors_assistant/config.txt");
    configfile.setFileName(filename); //initialize file name

    if (!configfile.exists()) //if the config file does not exist yet
    {
        //create the directory where the file should be:
        QString configpath(homepath + "/.tailors_assistant/");
        QDir dir;

        if (!dir.exists(configpath))
        {
            bool ok = dir.mkpath(configpath);
            if (!ok)
            {
                printf ("Path to config directory could not be created.\n");
                exit (EXIT_FAILURE);
            }
        }

        //open (create) the file with read/write access:
        configfile.open(QIODevice::ReadWrite);



        configfile.close(); //close connection to the file
    }
    else //if the file already exists:
    {
        //open in read only mode:
        configfile.open(QIODevice::ReadOnly);

        QTextStream in(&configfile);
        QString line = in.readLine(); //read first line of the file
        databasePath = line; //set the read line as database path

        configfile.close(); //close the file connection
    }
}

