#include "workpieceselector.h"
#include "ui_workpieceselector.h"

#include <QStandardItemModel>

WorkPieceSelector::WorkPieceSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WorkPieceSelector)
{
    ui->setupUi(this);

    //simple model for testing the table view:
    /*selector_test_model = new QStandardItemModel(24, 7);
    for (int row = 0; row < 24; ++row) {
        for (int column = 0; column < 7; ++column) {
            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); //make the items not editable (for the table view)
            selector_test_model->setItem(row, column, item);
        }
    }

    ui->tableView->setModel(selector_test_model);
    */
    setSelectionMode(record); //sets default selection mode
}

WorkPieceSelector::~WorkPieceSelector()
{
    delete ui;
    //delete selector_test_model;
    delete selection_model;
}

// sets the mode for record or offer selection
void WorkPieceSelector::setSelectionMode(PieceStatusMode mode)
{
    selection_mode = mode;
    if (mode == record)
    {
        selection_model = new PieceTableModel();
        selection_model->setMode(record);
        generateRecordsList();
        selection_model->setDataSource(&records_list);
        ui->tableView->setModel(selection_model);
        ui->label->setText("Werkstück zum Öffnen auswählen");
    }
    if (mode == offer)
    {
        selection_model = new PieceTableModel();
        selection_model->setMode(offer);
        generateOffersList();
        selection_model->setDataSource(&offers_list);
        ui->tableView->setModel(selection_model);
        ui->label->setText("Angebot zum Öffnen auswählen");
    }
}

//generates the list of workpieces, later to be used for getting it from the database
void WorkPieceSelector::generateRecordsList()
{
    WorkPiece *test_record1 = new WorkPiece();
    test_record1->setStatus(record);
    test_record1->setCustomer("test_customer1");
    test_record1->setName("test_werkstueck1");
    test_record1->setType("test_type1");
    test_record1->setDate({2010,6,7});

    WorkPiece *test_record2 = new WorkPiece();
    test_record2->setStatus(record);
    test_record2->setCustomer("test_customer2");
    test_record2->setName("test_werkstueck2");
    test_record2->setType("test_type1");
    test_record2->setDate({2010,10,22});
    test_record2->setComment("bla bla bla");

    WorkPiece *test_record3 = new WorkPiece();
    test_record3->setStatus(record);
    test_record3->setCustomer("test_customer3");
    test_record3->setName("test_werkstueck3");
    test_record3->setType("test_type2");
    test_record3->setDate({2012,3,15});

    records_list.append(test_record1);
    records_list.append(test_record2);
    records_list.append(test_record3);
}

//generates the list of offers, later to be used for getting it from the database
void WorkPieceSelector::generateOffersList()
{
    WorkPiece *test_offer1 = new WorkPiece();
    test_offer1->setStatus(offer);
    test_offer1->setCustomer("test_customer1");
    test_offer1->setName("test_angebot1");
    test_offer1->setType("test_type1");
    test_offer1->setDate({2010,6,7});

    WorkPiece *test_offer2 = new WorkPiece();
    test_offer2->setStatus(offer);
    test_offer2->setCustomer("test_customer2");
    test_offer2->setName("test_angebot2");
    test_offer2->setType("test_type1");
    test_offer2->setDate({2010,10,22});
    test_offer2->setComment("bla bla bla");

    WorkPiece *test_offer3 = new WorkPiece();
    test_offer3->setStatus(offer);
    test_offer3->setCustomer("test_customer3");
    test_offer3->setName("test_angebot3");
    test_offer3->setType("test_type2");
    test_offer3->setDate({2012,3,15});

    offers_list.append(test_offer1);
    offers_list.append(test_offer2);
    offers_list.append(test_offer3);
}
