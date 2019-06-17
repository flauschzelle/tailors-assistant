#include "workpiece.h"

//initialize the static member:
QString WorkPiece::table_def = "CREATE TABLE IF NOT EXISTS pieces "
                               "(piece_id INTEGER PRIMARY KEY NOT NULL, "
                               "status TEXT, "
                               "name TEXT, "
                               "customer TEXT, "
                               "type TEXT, "
                               "date TEXT, "
                               "comment TEXT, "
                               "picture BLOB "
                               ")";

WorkPiece::WorkPiece(QObject *parent) : QObject(parent)
{

}

int WorkPiece::getId() const
{
    return id;
}

void WorkPiece::setId(int value)
{
    id = value;
}

PieceStatusMode WorkPiece::getStatus() const
{
    return status;
}

void WorkPiece::setStatus(const PieceStatusMode& value)
{
    status = value;
}

QString WorkPiece::getCustomer() const
{
    return customer;
}

void WorkPiece::setCustomer(const QString& value)
{
    customer = value;
}

QString WorkPiece::getType() const
{
    return type;
}

void WorkPiece::setType(const QString& value)
{
    type = value;
}

QString WorkPiece::getComment() const
{
    return comment;
}

void WorkPiece::setComment(const QString& value)
{
    comment = value;
}

QDate WorkPiece::getDate() const
{
    return date;
}

void WorkPiece::setDate(const QDate& value)
{
    date = value;
}

QPixmap WorkPiece::getPicture() const
{
    return picture;
}

void WorkPiece::setPicture(const QPixmap& value)
{
    picture = value;
}

QVector<Step>& WorkPiece::getSteps()
{
    return steps;
}

QString WorkPiece::getName() const
{
    return name;
}

void WorkPiece::setName(const QString& value)
{
    name = value;
}

