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

#ifndef WORKPIECE_H
#define WORKPIECE_H

#include <QObject>
#include <QDate>
#include <QPixmap>
#include <QVector>

#include "general.h"
#include "step.h"

class WorkPiece : public QObject
{
    Q_OBJECT
public:
    explicit WorkPiece(QObject *parent = nullptr);
    ~WorkPiece();

    int getId() const;
    void setId(int value);

    PieceStatusMode getStatus() const;
    QString getName() const;
    QString getCustomer() const;
    QString getType() const;
    QString getComment() const;
    QDate getDate() const;
    QPixmap getPicture() const;
    QVector<Step*> getSteps();
    QVector<Step*>* getStepsPointer();
    void addStep(Step * step);

    static void setupTable();

    void savePieceToDatabase();
    bool isEmpty();

    void loadStepsFromDatabase();
    void saveStepsToDatabase();

    void deleteStep(int index);
    void deleteAllSteps();
    void moveStepBack(int index);
    void moveStepForward(int index);


    void deleteSteps();

    void setSums(const QString& value);

    QString getExportableText();

    void setPicture_bytes(const QByteArray& value);

signals:
    void stepOrderChanged();

public slots:
    void setStatus(const PieceStatusMode& value);
    void setName(const QString& value);
    void setCustomer(const QString& value);
    void setType(const QString& value);
    void setComment(const QString& value);
    void setDate(const QDate& value);
    void setPicture(const QPixmap& value);

    bool exportToTextfile(const QString filename);

    void setPictureFromFile(QString filename);
    void deletePicture();
private:
    int id;
    PieceStatusMode status;
    QString customer;
    QString name;
    QString type;
    QString comment;
    QDate date;
    QPixmap picture;
    QByteArray picture_bytes;

    int max_picture_size;

    QVector<Step*> steps;

    QString sums;
};

#endif // WORKPIECE_H
