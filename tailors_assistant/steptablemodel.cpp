#include "steptablemodel.h"

StepTableModel::StepTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    //default mode:
    setMode(record);
}

QVariant StepTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void StepTableModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    horizontalHeader = labels;
    column_count = horizontalHeader.length();
    emit headerDataChanged(Qt::Horizontal, 1, column_count);
}

int StepTableModel::rowCount(const QModelIndex &) const
{
    return data_source->length();
}

int StepTableModel::columnCount(const QModelIndex &) const
{
    return column_count;
}

QVariant StepTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        Step* data_row = data_source->at(index.row());

        if (mode == record)
        {
            switch (index.column()) {
                case 0:
                    return (data_row->getCount());
                case 1:
                    return data_row->getName();
                case 2:
                    return data_row->getSeamType();
                case 3:
                    return data_row->getMaterial();
                case 4:
                    return data_row->getDetail();
                case 5:
                    return data_row->getMinutesAll();
                case 6:
                    return data_row->getComment();
                default:
                    return (".");
            }
        }
        if (mode == offer)
        {
            switch (index.column()) {
                case 0:
                    return (data_row->getCount());
                case 1:
                    return data_row->getName();
                case 2:
                    return data_row->getSeamType();
                case 3:
                    return data_row->getMaterial();
                case 4:
                    return data_row->getDetail();
                case 5:
                    return data_row->getFilters();
                case 6:
                    return data_row->getBaseDatasets();
                case 7:
                    return data_row->getMin();
                case 8:
                    return data_row->getMed();
                case 9:
                    return data_row->getAvg();
                case 10:
                    return data_row->getMax();
                case 11:
                    return data_row->getMinutesAll();
                case 12:
                    return data_row->getComment();
                default:
                    return (".");
            }
        }

        return 0;
    }

    return QVariant(); //left over from pre-made function
}

void StepTableModel::setDataSource(QVector<Step*>* source)
{
    data_source = source;
    //emit dataChanged(); //this needs parameters, not sure if it is the right one to use

}

PieceStatusMode StepTableModel::getMode() const
{
    return mode;
}

void StepTableModel::setMode(const PieceStatusMode& value)
{
    mode = value;
    if (mode == record)
    {
        setHorizontalHeaderLabels({"#", "Bezeichnung", "Nahttyp", "Material", "Detail", "Zeit", "Kommentar"});
    }
    if (mode == offer)
    {
        setHorizontalHeaderLabels({"#", "Bezeichnung", "Nahttyp", "Material", "Detail", "Filter", "Daten", "min", "med", "avg", "max", "man", "Kommentar"});
    }
}

//slot that is called when the data in a step change
void StepTableModel::stepDataChanged(Step* step)
{
    int step_index = data_source->indexOf(step);
    QModelIndex firstindex = createIndex(step_index, 0);
    QModelIndex lastindex = createIndex(step_index, column_count-1);
    emit dataChanged(firstindex, lastindex);
}

//slot that is called when the number or order of steps is changed
void StepTableModel::stepOrderChanged()
{
    //these will make all attached views reset themselves:
    beginResetModel();
    //I think I don't need to explicitly do anything here...
    endResetModel();
}
