/*  
    Tailor's Assistant - a tool to help with estimating work times as a tailor
    Copyright (C) 2019  by Kirstin Rohwer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

	For contact information, see the author's website <http://metakiki.net>.
*/

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
