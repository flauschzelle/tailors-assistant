#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workpieceselector.h"
#include "workpiece.h"
#include "step.h"
#include "deletepiecedialog.h"

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
                QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); //make the items not editable (for the table view)
                test_model->setItem(row, column, item);
            }
        }

    ui->dataTableView->setModel(test_model);

    ui->pieceDataBox->setEnabled(false);
    ui->stepDataBox->setEnabled(false);
    setInputMode(record); //default value for input mode
    currentPiece = NULL; //default value for piece pointer
    selector = NULL; //default value for selector pointer
    db_settings_dialog = NULL; //default value for db settings dialog pointer

    QObject::connect(ui->actionNeues_Werkst_ck, &QAction::triggered, this, &MainWindow::newPiece); //connect signal & slot for new piece menu item
    QObject::connect(ui->actionNeues_Angebot, &QAction::triggered, this, &MainWindow::newOffer); //connect signal & slot for new offer menu item

    QObject::connect(ui->actionWerkst_ck_ffnen, &QAction::triggered, this, &MainWindow::openPieceSelector); //connect signal & slot for open piece menu item
    QObject::connect(ui->actionAngebot_ffnen, &QAction::triggered, this, &MainWindow::openOfferSelector); //connect signal & slot for open offer menu item

    QObject::connect(ui->actionDatenbank_Einstellungen, &QAction::triggered, this, &MainWindow::openDatabaseSettings); //connect signal from menu to database settigns dialog

    QObject::connect(ui->deletePiecePushButton, &QPushButton::clicked, this, &MainWindow::tryToDeleteCurrentPiece);
    QObject::connect(ui->editStepsPushButton, &QPushButton::clicked, this, &MainWindow::activateStepEdits);

    MainWindow::instance = this;
}

// destructor
MainWindow::~MainWindow()
{
    db.close();
    delete ui;
    delete test_model;
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

        QStringList record_header_labels = {"#", "Bezeichnung", "Nahttyp", "Material", "Detail", "Zeit", "Kommentar"};
        test_model->setHorizontalHeaderLabels(record_header_labels);
    }

    if (mode == offer)
    {
        ui->pieceDataBox->setTitle("Angebots-Stammdaten");
        ui->filterWidget->setVisible(true);
        ui->pictureWidget->setVisible(false);
        ui->deletePiecePushButton->setText("Angebot löschen");

        QStringList offer_header_labels = {"#", "Bezeichnung", "Nahttyp", "Material", "Detail", "Filter", "Daten", "min", "med", "avg", "max", "man", "Kommentar"};
        test_model->setHorizontalHeaderLabels(offer_header_labels);
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
    printf("current piece: %s\n", currentPiece->getName().toStdString().c_str());
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
    if (currentPiece->getId() != 0)
    {
        QSqlQuery query;
        query.prepare("DELETE FROM pieces WHERE piece_id=(:id)");
        query.bindValue(":id", currentPiece->getId());
        query.exec();
    }
    currentPiece = NULL;
    ui->pieceDataBox->setEnabled(false);
    ui->stepDataBox->setEnabled(false);

    //TODO:also delete steps!
}

void MainWindow::activateStepEdits()
{
    currentPiece->savePieceToDatabase();
    ui->stepDataBox->setEnabled(true);
    currentPiece->loadStepsFromDatabase();

}

void MainWindow::setCurrentPiece(WorkPiece * piece)
{
    currentPiece = piece;
    setInputMode(currentPiece->getStatus());
    //activate ui elements:
    ui->pieceDataBox->setEnabled(true);
    ui->stepDataBox->setEnabled(false);
    fillPieceDataComboBoxes();
    fillPieceDataUIElements(currentPiece);
    connectPieceDataInputs();
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

void MainWindow::fillStepDataUIElements(Step* step)
{

}

void MainWindow::connectStepDataInputs()
{

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

