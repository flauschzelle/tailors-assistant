#ifndef STEPTABLEMODEL_H
#define STEPTABLEMODEL_H

#include <QAbstractTableModel>

#include "general.h"
#include "workpiece.h"
#include "step.h"

class StepTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit StepTableModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void setHorizontalHeaderLabels(const QStringList& labels); //added this one myself

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setDataSource(QVector<Step*>* source);

    PieceStatusMode getMode() const;
    void setMode(const PieceStatusMode& value);

public slots:
    void stepDataChanged(Step * step);
    void stepOrderChanged();

private:

    QStringList horizontalHeader;
    int column_count;
    QVector<Step*>* data_source;
    PieceStatusMode mode;
};

#endif // STEPTABLEMODEL_H
