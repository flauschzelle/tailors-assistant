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

#ifndef STEP_H
#define STEP_H

#include <QObject>
#include <QString>
#include <QSql>
#include <QSqlQuery>

class WorkPiece;

class Step : public QObject
{
    Q_OBJECT
public:
    explicit Step(QObject *parent = nullptr);

    static void setupTable();

    int getId() const;
    void setId(int value);

    QString getName() const;
    int getMinutesAll() const;
    int getCount() const;
    QString getSeamType() const;
    QString getMaterial() const;
    QString getDetail() const;
    QString getComment() const;
    bool getFilterSeamType() const;
    bool getFilterMaterial() const;
    bool getFilterDetail() const;
    bool getFilterPieceType() const;
    QString getFilters() const;

    int getBaseDatasets() const;
    int getMin() const;
    int getMed() const;
    int getAvg() const;
    int getMax() const;

    void calculateStatistics(WorkPiece * piece);

signals:
    void stepDataChanged(Step * step);

public slots:
    void setName(const QString& value);
    void setMinutesAll(int value);
    void setCount(int value);
    void setSeamType(const QString& value);
    void setMaterial(const QString& value);
    void setDetail(const QString& value);
    void setComment(const QString& value);
    void setFilterSeamType(bool value);
    void setFilterMaterial(bool value);
    void setFilterDetail(bool value);
    void setFilterPieceType(bool value);

private:
    int id;
    QString name;
    int minutesAll;
    int count;
    QString seamType;
    QString material;
    QString detail;
    QString comment;
    bool filterSeamType;
    bool filterMaterial;
    bool filterDetail;
    bool filterPieceType;
    int min;
    int med;
    int avg;
    int max;
    int baseDatasets;

    void calculateStatsPart(bool median, WorkPiece* piece);
};

#endif // STEP_H
