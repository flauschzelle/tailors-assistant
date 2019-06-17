#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QStringList>

#include "workpieceselector.h"

// constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    QObject::connect(ui->actionNeues_Werkst_ck, &QAction::triggered, this, &MainWindow::newPiece); //connect signal & slot for new piece menu item
    QObject::connect(ui->actionNeues_Angebot, &QAction::triggered, this, &MainWindow::newOffer); //connect signal & slot for new offer menu item

    QObject::connect(ui->actionWerkst_ck_ffnen, &QAction::triggered, this, &MainWindow::openPieceSelector); //connect signal & slot for open piece menu item
    QObject::connect(ui->actionAngebot_ffnen, &QAction::triggered, this, &MainWindow::openOfferSelector); //connect signal & slot for open offer menu item
}

// destructor
MainWindow::~MainWindow()
{
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
