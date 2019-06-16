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
    void setStatus(const PieceStatusMode& value);

    QString getCustomer() const;
    void setCustomer(const QString& value);

    QString getType() const;
    void setType(const QString& value);

    QString getComment() const;
    void setComment(const QString& value);

    QDate getDate() const;
    void setDate(const QDate& value);

    QPixmap getPicture() const;
    void setPicture(const QPixmap& value);

    QVector<Step>& getSteps();

    QString getName() const;
    void setName(const QString& value);

signals:

public slots:

private:
    int id;
    PieceStatusMode status;
    QString customer;
    QString name;
    QString type;
    QString comment;
    QDate date;
    QPixmap picture;

    QVector<Step> steps;
};

#endif // WORKPIECE_H
