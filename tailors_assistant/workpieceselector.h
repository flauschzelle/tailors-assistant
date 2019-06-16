#ifndef WORKPIECESELECTOR_H
#define WORKPIECESELECTOR_H

#include <QDialog>
#include <QStandardItemModel>
#include <QVector>
#include <QSortFilterProxyModel>

#include "general.h"
#include "workpiece.h"
#include "piecetablemodel.h"

namespace Ui {
class WorkPieceSelector;
}

class WorkPieceSelector : public QDialog
{
    Q_OBJECT

public:
    explicit WorkPieceSelector(QWidget *parent = 0);
    ~WorkPieceSelector();

    void setSelectionMode(PieceStatusMode mode);
    void generateRecordsList();
    void generateOffersList();

private:
    Ui::WorkPieceSelector *ui;

    PieceStatusMode selection_mode;

    QVector<WorkPiece*> records_list;
    QVector<WorkPiece*> offers_list;

    PieceTableModel *selection_model;
    QSortFilterProxyModel *proxy_model;
};

#endif // WORKPIECESELECTOR_H
