#ifndef PIECETABLEMODEL_H
#define PIECETABLEMODEL_H

#include <QAbstractTableModel>

#include "general.h"
#include "workpiece.h"

class PieceTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PieceTableModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void setHorizontalHeaderLabels(const QStringList& labels); //added this one myself

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setDataSource(QVector<WorkPiece*>* source);

    PieceStatusMode getMode() const;
    void setMode(const PieceStatusMode& value);

private:

    QStringList horizontalHeader;
    int column_count;
    QVector<WorkPiece*>* data_source;
    PieceStatusMode mode;
};

#endif // PIECETABLEMODEL_H
