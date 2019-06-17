#include "workpieceselector.h"
#include "ui_workpieceselector.h"

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <stdio.h>

WorkPieceSelector::WorkPieceSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WorkPieceSelector)
{
    ui->setupUi(this);

    setSelectionMode(record); //sets default selection mode

    selectedRowNr = 0; //default value
}

WorkPieceSelector::~WorkPieceSelector()
{
    delete ui;
    delete piece_selection_model;
    delete proxy_model;
}

// sets the mode for record or offer selection
void WorkPieceSelector::setSelectionMode(PieceStatusMode mode)
{
    selection_mode = mode;
    piece_selection_model = new PieceTableModel();

    if (mode == record)
    {
        piece_selection_model->setMode(record);
        generateRecordsList();
        piece_selection_model->setDataSource(&records_list);
        ui->label->setText("Werkstück zum Öffnen auswählen");
    }
    if (mode == offer)
    {
        piece_selection_model->setMode(offer);
        generateOffersList();
        piece_selection_model->setDataSource(&offers_list);
        ui->label->setText("Angebot zum Öffnen auswählen");
    }

    proxy_model = new QSortFilterProxyModel();
    proxy_model->setSourceModel(piece_selection_model);
    ui->tableView->setModel(proxy_model);
    ui->tableView->resizeColumnsToContents();

    //connect the selection changed signal to the slot that registers the selected row:
    QObject::connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &WorkPieceSelector::rowSelectionChanged);
}

//generates the list of workpieces, later to be used for getting it from the database
void WorkPieceSelector::generateRecordsList()
{
    records_list.clear();

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
    offers_list.clear();

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

//slot that is called when a row is selected:
void WorkPieceSelector::rowSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    if (selected.indexes().length() != 0) //if anything is selected
    {
        selectedRowNr = (proxy_model->mapSelectionToSource(selected)).indexes().at(0).row();

        //this was used for debug purposes:
        //printf("proxy row nr: %d \n", selected.indexes().at(0).row());
        //printf("source row nr: %d \n", selectedRowNr);
    }
}

//returns the piece that was selected in the dialog
WorkPiece* WorkPieceSelector::getSelectedPiece()
{
    if (selection_mode == record)
    {
        return records_list.at(selectedRowNr);
    }
    else if (selection_mode == offer)
    {
        return offers_list.at(selectedRowNr);
    }
    return NULL;
}
