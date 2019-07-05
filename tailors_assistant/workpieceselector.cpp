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

    piece_selection_model = NULL;
    proxy_model = NULL;

    generateWorkPieceLists(); //get data from the database

    setSelectionMode(record); //sets default selection mode
    setConversionMode(none); //default conversion mode

    selectedRowNr = 0; //default value
    selected_piece = NULL; //default
}

void WorkPieceSelector::deleteRecordsAndOffers()
{
    for (int i = 0; i < all_records.length(); i++)
    {
        if (all_records.at(i) != selected_piece)
        {
            delete all_records.at(i);
        }
    }
    for (int i = 0; i < all_offers.length(); i++)
    {
        if (all_offers.at(i) != selected_piece)
        {
            delete all_offers.at(i);
        }
    }
}

WorkPieceSelector::~WorkPieceSelector()
{
    delete ui;
    delete piece_selection_model;
    delete proxy_model;
    deleteRecordsAndOffers();
}

// sets the mode for record or offer selection
void WorkPieceSelector::setSelectionMode(PieceStatusMode mode)
{
    selection_mode = mode;
    if (piece_selection_model != NULL)
    {
        delete piece_selection_model;
    }
    piece_selection_model = new PieceTableModel();

    if (mode == record)
    {
        piece_selection_model->setMode(record);
        piece_selection_model->setDataSource(&all_records);
        ui->label->setText("Werkstück zum Öffnen auswählen");
    }
    if (mode == offer)
    {
        piece_selection_model->setMode(offer);
        piece_selection_model->setDataSource(&all_offers);
        ui->label->setText("Angebot zum Öffnen auswählen");
    }

    if (proxy_model != NULL)
    {
        delete proxy_model;
    }
    proxy_model = new QSortFilterProxyModel();
    proxy_model->setSourceModel(piece_selection_model);
    //empty, case insensitive default filter:
    proxy_model->setFilterRegExp((QRegExp("", Qt::CaseInsensitive, QRegExp::RegExp)));
    proxy_model->setFilterKeyColumn(-1); //filter from all columns
    ui->tableView->setModel(proxy_model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

    //sort the table view by date, descending:
    if (mode == record)
    {
        ui->tableView->sortByColumn(3, Qt::DescendingOrder);
    }
    if (mode == offer)
    {
        ui->tableView->sortByColumn(2, Qt::DescendingOrder);
    }

    //connect the selection changed signal to the slot that registers the selected row:
    QObject::connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &WorkPieceSelector::rowSelectionChanged);

    //connect the filter input to the filtering function:
    QObject::connect(ui->filterLineEdit, &QLineEdit::textChanged, proxy_model, QOverload<const QString &>::of(&QSortFilterProxyModel::setFilterRegExp));

}

void WorkPieceSelector::setConversionMode(const PieceConversionMode& value)
{
    conversion_mode = value;
}

//generates a list of all workpieces stored in the database
void WorkPieceSelector::generateWorkPieceLists()
{
    //clear any existing pieces from the datastructures
    deleteRecordsAndOffers();
    all_records.clear();
    all_offers.clear();

    QSqlQuery query;

    //request all pieces from the database
    if (!query.exec("SELECT * FROM pieces"))
    {
        printf("error while fetching pieces list from database\n");
        exit(EXIT_FAILURE);
    }
    //go through the result set:
    while (query.next())
    {
        WorkPiece *nextpiece = new WorkPiece();
        nextpiece->setId(query.value(0).toInt());
        nextpiece->setName(query.value(2).toString());
        nextpiece->setCustomer(query.value(3).toString());
        nextpiece->setType(query.value(4).toString());
        nextpiece->setDate(query.value(5).toDate());
        nextpiece->setComment(query.value(6).toString());
        //get and convert picture:
        QByteArray pic_bytes = query.value(7).toByteArray();
        if (!pic_bytes.isNull())
        {
            QPixmap pic;
            bool ok = pic.loadFromData(pic_bytes);
            if (!ok)
            {
                QString name = nextpiece->getName();
                name += " (" + QString::number(nextpiece->getId()) + "): ";
                printf("%serror while loading picture from database.\n", name.toStdString().c_str());
            }
            else
            {
                nextpiece->setPicture(pic);
                nextpiece->setPicture_bytes(pic_bytes);
                //for debugging:
                //printf("opened JPG image of size %d bytes\n", pic_bytes.size());


                //for debugging:
                /*
                QString name = nextpiece->getName();
                name += " (" + QString::number(nextpiece->getId()) + "): ";
                printf("%ssuccessfully loaded picture from database.\n", name.toStdString().c_str());
                */
            }
        }
        //sort by status:
        if (query.value(1).toInt() == record)
        {
            nextpiece->setStatus(record);
            all_records.append(nextpiece);
        }
        else if (query.value(1).toInt() == offer)
        {
            nextpiece->setStatus(offer);
            all_offers.append(nextpiece);
        }
        else
        {
            printf("piece with invalid status found in database\n");
        }
    }
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
    if (conversion_mode == none)
    {
        if (selection_mode == record)
        {
            selected_piece = all_records.at(selectedRowNr);
        }
        else if (selection_mode == offer)
        {
            selected_piece = all_offers.at(selectedRowNr);
        }
    }
    if (conversion_mode == offer_to_record)
    {
        WorkPiece * piece = all_offers.at(selectedRowNr);
        piece->loadStepsFromDatabase();
        piece->setStatus(record);
        piece->setDate(QDate::currentDate());
        piece->savePieceToDatabase();
        selected_piece = piece;
    }
    if (conversion_mode == record_copy_to_offer)
    {
        WorkPiece * piece = new WorkPiece();
        WorkPiece * oldpiece = all_records.at(selectedRowNr);
        oldpiece->loadStepsFromDatabase();

        //set new values for some piece data:
        piece->setStatus(offer);
        piece->setDate(QDate::currentDate());

        //copy piece data from old to new one:
        piece->setName(oldpiece->getName());
        piece->setCustomer(oldpiece->getCustomer());
        piece->setType(oldpiece->getType());
        piece->setComment(oldpiece->getComment());

        //copy step data from old to new piece:
        for (int i = 0; i < oldpiece->getSteps().length(); i++)
        {
            Step * step = new Step();
            step->setName(oldpiece->getSteps().at(i)->getName());
            step->setCount(oldpiece->getSteps().at(i)->getCount());
            step->setMinutesAll(oldpiece->getSteps().at(i)->getMinutesAll());
            step->setSeamType(oldpiece->getSteps().at(i)->getSeamType());
            step->setMaterial(oldpiece->getSteps().at(i)->getMaterial());
            step->setDetail(oldpiece->getSteps().at(i)->getDetail());
            step->setComment(oldpiece->getSteps().at(i)->getComment());

            piece->addStep(step);
        }

        //save the newly generated offer piece:
        piece->savePieceToDatabase();

        selected_piece = piece;
    }

    return selected_piece;
}
