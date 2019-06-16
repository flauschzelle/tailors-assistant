#include "piecetablemodel.h"

PieceTableModel::PieceTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    //default mode:
    setMode(record);
}

QVariant PieceTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{   
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            return horizontalHeader[section];
        }
        else if (orientation == Qt::Vertical)
        {
            return (section+1);
        }
    }
    else
    {
        //do nothing if the role is something else
    }
    return QVariant();
}

void PieceTableModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    horizontalHeader = labels;
    column_count = horizontalHeader.length();
}

int PieceTableModel::rowCount(const QModelIndex &) const
{
    return data_source->length();
}

int PieceTableModel::columnCount(const QModelIndex &) const
{
    return column_count;
}

QVariant PieceTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        WorkPiece* data_row = data_source->at(index.row());

        if (mode == record)
        {
            switch (index.column()) {
                case 0:
                    return ("Bild");
                case 1:
                    return data_row->getName();
                case 2:
                    return data_row->getCustomer();
                case 3:
                    return data_row->getDate();
                case 4:
                    return data_row->getType();
                case 5:
                    return data_row->getComment();
                default:
                    return (".");
            }
        }
        if (mode == offer)
        {
            switch (index.column()) {
                case 0:
                    return data_row->getName();
                case 1:
                    return data_row->getCustomer();
                case 2:
                    return data_row->getDate();
                case 3:
                    return data_row->getType();
                case 4:
                    return data_row->getComment();
                default:
                    return (".");
            }
        }

        return 0;
    }

    return QVariant(); //left over from pre-made function
}

void PieceTableModel::setDataSource(QVector<WorkPiece*>* source)
{
    data_source = source;
}

PieceStatusMode PieceTableModel::getMode() const
{
    return mode;
}

void PieceTableModel::setMode(const PieceStatusMode& value)
{
    mode = value;
    if (mode == record)
    {
        setHorizontalHeaderLabels({"Bild","Bezeichnung","Kunde","Datum","Typ","Kommentar"});
    }
    if (mode == offer)
    {
        setHorizontalHeaderLabels({"Bezeichnung","Kunde","Datum","Typ","Kommentar"});
    }
}
