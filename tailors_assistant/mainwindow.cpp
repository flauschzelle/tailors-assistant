#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workpieceselector.h"
#include "workpiece.h"
#include "step.h"

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


    //config & files stuff:---------------------------------

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

    //end of files stuff------------------------------------

    //database stuff:---------------------------------------

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

        setupDatabase(); //setup the newly connected database

    //end of database stuff---------------------------------

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

    setInputMode(record); //default value for input mode
    currentPiece = NULL; //default value for piece pointer
    selector = NULL; //default value for selector pointer
    db_settings_dialog = NULL; //default value for db settings dialog pointer

    QObject::connect(ui->actionNeues_Werkst_ck, &QAction::triggered, this, &MainWindow::newPiece); //connect signal & slot for new piece menu item
    QObject::connect(ui->actionNeues_Angebot, &QAction::triggered, this, &MainWindow::newOffer); //connect signal & slot for new offer menu item

    QObject::connect(ui->actionWerkst_ck_ffnen, &QAction::triggered, this, &MainWindow::openPieceSelector); //connect signal & slot for open piece menu item
    QObject::connect(ui->actionAngebot_ffnen, &QAction::triggered, this, &MainWindow::openOfferSelector); //connect signal & slot for open offer menu item

    QObject::connect(ui->actionDatenbank_Einstellungen, &QAction::triggered, this, &MainWindow::openDatabaseSettings); //connect signal from menu to database settigns dialog

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
    setInputMode(record);
}

//slot function to start creating a new offer:
void MainWindow::newOffer()
{
    setInputMode(offer);
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
    currentPiece = selector->getSelectedPiece();
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

void MainWindow::setupDatabase()
{
    WorkPiece::setupTable();
    Step::setupTable();

    QSqlQuery query;
    query.exec("SELECT * FROM pieces");
    if (!query.next())
    {
        WorkPieceSelector::writeTestDataToDatabase();
    }
}
