#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workpieceselector.h"
#include "workpiece.h"
#include "step.h"
#include "deletepiecedialog.h"
#include "deletestepdialog.h"
#include "steptablemodel.h"

#include <QStandardItemModel>
#include <QStringList>
#include <QFile>

MainWindow* MainWindow::instance = NULL; //initialize the static member variable

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

    //simple model for testing the table view:
    test_model = new QStandardItemModel(24, 7);
        for (int row = 0; row < 24; ++row) {
            for (int column = 0; column < 7; ++column) {
                QStandardItem *item = new QStandardItem(QString("row%0, col%1").arg(row).arg(column));
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); //make the items not editable (for the table view)
                test_model->setItem(row, column, item);
            }
        }
    //TODO: remove all traces of the test model!

    //model for table view:
    empty_step_list = new QVector<Step*>();

    steps_model = new StepTableModel();
    steps_model->setMode(record); //default value
    steps_model->setDataSource(empty_step_list);

    ui->dataTableView->setModel(steps_model);
    ui->dataTableView->resizeColumnsToContents();
    ui->dataTableView->setEnabled(false);

    ui->pieceDataBox->setEnabled(false);
    ui->stepDataBox->setEnabled(false);
    setInputMode(record); //default value for input mode
    currentPiece = NULL; //default value for piece pointer
    stepIndex = 0; //default value for step index
    selector = NULL; //default value for selector pointer
    db_settings_dialog = NULL; //default value for db settings dialog pointer

    QObject::connect(ui->actionNeues_Werkst_ck, &QAction::triggered, this, &MainWindow::newPiece); //connect signal & slot for new piece menu item
    QObject::connect(ui->actionNeues_Angebot, &QAction::triggered, this, &MainWindow::newOffer); //connect signal & slot for new offer menu item

    QObject::connect(ui->actionWerkst_ck_ffnen, &QAction::triggered, this, &MainWindow::openPieceSelector); //connect signal & slot for open piece menu item
    QObject::connect(ui->actionAngebot_ffnen, &QAction::triggered, this, &MainWindow::openOfferSelector); //connect signal & slot for open offer menu item

    QObject::connect(ui->actionDatenbank_Einstellungen, &QAction::triggered, this, &MainWindow::openDatabaseSettings); //connect signal from menu to database settigns dialog

    QObject::connect(ui->deletePiecePushButton, &QPushButton::clicked, this, &MainWindow::tryToDeleteCurrentPiece);
    QObject::connect(ui->editStepsPushButton, &QPushButton::clicked, this, &MainWindow::activateStepEdits);

    QObject::connect(ui->previousStepPushButton, &QPushButton::clicked, this, &MainWindow::prevStep);
    QObject::connect(ui->nextStepPushButton, &QPushButton::clicked, this, &MainWindow::nextStep);

    QObject::connect(ui->deleteStepPushButton, &QPushButton::clicked, this, &MainWindow::tryToDeleteCurrentStep);

    QObject::connect(ui->upToolButton, &QToolButton::clicked, this, &MainWindow::stepMovedUp);
    QObject::connect(ui->downToolButton, &QToolButton::clicked, this, &MainWindow::stepMovedDown);

    MainWindow::instance = this;
}

// destructor
MainWindow::~MainWindow()
{
    db.close();
    delete ui;
    delete test_model;
    delete empty_step_list;
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
        //QStringList record_header_labels = {"#", "Bezeichnung", "Nahttyp", "Material", "Detail", "Zeit", "Kommentar"};
        //steps_model->setHorizontalHeaderLabels(record_header_labels);
    }

    if (mode == offer)
    {
        ui->pieceDataBox->setTitle("Angebots-Stammdaten");
        ui->filterWidget->setVisible(true);
        ui->pictureWidget->setVisible(false);
        ui->deletePiecePushButton->setText("Angebot löschen");

        steps_model->setMode(offer);
        //QStringList offer_header_labels = {"#", "Bezeichnung", "Nahttyp", "Material", "Detail", "Filter", "Daten", "min", "med", "avg", "max", "man", "Kommentar"};
        //steps_model->setHorizontalHeaderLabels(offer_header_labels);
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
    setInputMode(record);
    setCurrentPiece(new WorkPiece());
    currentPiece->setStatus(record);
}

//slot function to start creating a new offer:
void MainWindow::newOffer()
{
    if (currentPiece != NULL && !currentPiece->isEmpty())
    {
        currentPiece->savePieceToDatabase();
    }
    setInputMode(offer);
    setCurrentPiece(new WorkPiece());
    currentPiece->setStatus(offer);
}

//slot function for a selection view to open an existing piece:
void MainWindow::openPieceSelector()
{
    if (selector != NULL)
    {
        delete selector;
    }
    selector = new WorkPieceSelector(this);
    selector->setSelectionMode(record);
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
    selector = new WorkPieceSelector(this);
    selector->setSelectionMode(offer);
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
    delete selector;
    selector = NULL; //re-initialize selector to null pointer
    printf("selected piece: %s (%d)\n", currentPiece->getName().toStdString().c_str(), currentPiece->getId());
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

//slot function for opening the database settings dialog
void MainWindow::openDatabaseSettings()
{
    db_settings_dialog = new DatabaseSettings();
    db_settings_dialog->open();
    QObject::connect(db_settings_dialog, &QDialog::accepted, this, &MainWindow::getDBPathFromSelector);
    QObject::connect(db_settings_dialog, &QDialog::rejected, this, &MainWindow::cleanUpDBSelector);
}

//slot function for changing the path to the database
void MainWindow::changeDatabasePath(QString path)
{
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
    DeleteStepDialog *dsd = new DeleteStepDialog();
    dsd->open();
    QObject::connect(dsd, &QDialog::accepted, this, &MainWindow::deleteCurrentStep);
    QObject::connect(dsd, &QDialog::rejected, [=](){ delete dsd; });

}

//public slot to actually delete the current step from the database
void MainWindow::deleteCurrentStep()
{
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
    fillStepDataUIElements(currentPiece->getSteps().at(stepIndex));
}

//slot that is called when a step is selected from the table view
void MainWindow::stepSelected(const QItemSelection &selected, const QItemSelection &)
{
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
    currentPiece = NULL;
    steps_model->setDataSource(empty_step_list);
    ui->editStepsPushButton->setEnabled(true);
    ui->pieceDataBox->setEnabled(false);
    ui->stepDataBox->setEnabled(false);
    ui->dataTableView->setEnabled(false);
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
    ui->dataTableView->setEnabled(true);
    ui->dataTableView->selectRow(stepIndex);
    //fillStepDataUIElements(currentPiece->getSteps().at(stepIndex));
    //connectStepDataInputs(currentPiece->getSteps().at(stepIndex));
}

//this will be called when the "next step" button is pressed
void MainWindow::nextStep()
{
    currentPiece->saveStepsToDatabase(); //save current input data
    currentPiece->loadStepsFromDatabase(); //reload step data
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
    //connectStepDataInputs(currentPiece->getSteps().at(stepIndex));
}

//this will be called when the previous step button is pressed
void MainWindow::prevStep()
{
    currentPiece->saveStepsToDatabase(); //save current input data
    currentPiece->loadStepsFromDatabase(); //reload step data
    //go to the previous step only if you're not already at the first one:
    if (stepIndex > 0)
    {
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
    //connectStepDataInputs(currentPiece->getSteps().at(stepIndex));
}

void MainWindow::setCurrentPiece(WorkPiece * piece)
{
    currentPiece = piece;
    currentPiece->loadStepsFromDatabase();
    stepIndex = 0; //reset step index to start at the first step
    setInputMode(currentPiece->getStatus());
    //activate ui elements:
    ui->editStepsPushButton->setEnabled(true);
    ui->pieceDataBox->setEnabled(true);
    ui->stepDataBox->setEnabled(false);
    fillPieceDataComboBoxes();
    fillPieceDataUIElements(currentPiece);
    connectPieceDataInputs();
    //set data source for table view:
    steps_model->setDataSource(currentPiece->getStepsPointer());
    ui->dataTableView->resizeColumnsToContents();
    ui->dataTableView->setEnabled(false);
}

void MainWindow::setupDatabase()
{
    //create the tables that are needed, if they don't exist:
    WorkPiece::setupTable();
    Step::setupTable();

    //fill the new database with test data, if it is empty:
    QSqlQuery query;
    query.exec("SELECT * FROM pieces");
    if (!query.next())
    {
        WorkPieceSelector::writeTestDataToDatabase();
    }
}

//fill the comboBoxes with data from the db:
void MainWindow::fillPieceDataComboBoxes()
{
    ui->customerComboBox->clear();
    QStringList customers;
    QSqlQuery query;
    query.exec("SELECT DISTINCT customer FROM pieces");
    while (query.next())
    {
        customers.append(query.value(0).toString());
    }
    ui->customerComboBox->addItems(customers);


    ui->pieceTypeComboBox->clear();
    QStringList types;
    query.exec("SELECT DISTINCT type FROM pieces");
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
    query.exec("SELECT DISTINCT name FROM steps");
    while (query.next())
    {
        stepNames.append(query.value(0).toString());
    }
    ui->stepNameComboBox->addItems(stepNames);

    ui->seamTypeComboBox->clear();
    QStringList seamTypes;
    query.exec("SELECT DISTINCT seam_type FROM steps");
    while (query.next())
    {
        seamTypes.append(query.value(0).toString());
    }
    ui->seamTypeComboBox->addItems(seamTypes);

    ui->materialComboBox->clear();
    QStringList materials;
    query.exec("SELECT DISTINCT material FROM steps");
    while (query.next())
    {
        materials.append(query.value(0).toString());
    }
    ui->materialComboBox->addItems(materials);

    ui->detailComboBox->clear();
    QStringList details;
    query.exec("SELECT DISTINCT detail FROM steps");
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

    QObject::disconnect(step, &Step::stepDataChanged, steps_model, &StepTableModel::stepDataChanged);

}

//connects the step data inputs to the given step
void MainWindow::connectStepDataInputs(Step * step)
{
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

    QObject::connect(step, &Step::stepDataChanged, steps_model, &StepTableModel::stepDataChanged);


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

        //write the default database path to the newly created config file:
        QTextStream out(&configfile);
        out << databasePath;

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

