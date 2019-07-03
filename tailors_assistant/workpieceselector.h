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

    WorkPiece* getSelectedPiece();

    void generateWorkPieceLists();

    void setConversionMode(const PieceConversionMode& value);

    void deleteRecordsAndOffers();

public slots:

    void rowSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    Ui::WorkPieceSelector *ui;

    PieceStatusMode selection_mode;
    PieceConversionMode conversion_mode;

    QVector<WorkPiece*> all_records;
    QVector<WorkPiece*> all_offers;

    PieceTableModel *piece_selection_model;
    QSortFilterProxyModel *proxy_model;
    int selectedRowNr;

    WorkPiece * selected_piece;

};

#endif // WORKPIECESELECTOR_H
