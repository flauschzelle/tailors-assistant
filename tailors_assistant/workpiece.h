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
    void addStep(Step * step);

    static void setupTable();

    void savePieceToDatabase();
    bool isEmpty();

    void loadStepsFromDatabase();
    void saveStepsToDatabase();

signals:

public slots:
    void setStatus(const PieceStatusMode& value);
    void setName(const QString& value);
    void setCustomer(const QString& value);
    void setType(const QString& value);
    void setComment(const QString& value);
    void setDate(const QDate& value);
    void setPicture(const QPixmap& value);

private:
    int id;
    PieceStatusMode status;
    QString customer;
    QString name;
    QString type;
    QString comment;
    QDate date;
    QPixmap picture;

    QVector<Step*> steps;
};

#endif // WORKPIECE_H
