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
