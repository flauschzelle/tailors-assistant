#include "workpiece.h"

WorkPiece::WorkPiece(QObject *parent) : QObject(parent)
{
    id = 0; //initialize id with default value
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

//called from the main window when a new database is set up
void WorkPiece::setupTable()
{
    QString table_def = "CREATE TABLE IF NOT EXISTS pieces "
                        "(piece_id INTEGER PRIMARY KEY NOT NULL, "
                        "status INT, "
                        "name TEXT, "
                        "customer TEXT, "
                        "type TEXT, "
                        "date TEXT, "
                        "comment TEXT, "
                        "picture BLOB "
                        ")";

    QSqlQuery query;

    if (!query.exec(table_def))
    {
        printf("error while setting up the pieces table\n");
    }
}


void WorkPiece::savePieceToDatabase()
{
    QSqlQuery query;

    if (id == 0)
    {
        query.prepare("INSERT INTO pieces (status, name, customer, type, date, comment)"
                      "VALUES (:stat, :name, :cust, :type, :date, :comm)");
    }
    else
    {
        query.prepare("UPDATE pieces "
                      "SET (status, name, customer, type, date, comment)"
                      " = VALUES "
                      "(:stat, :name, :cust, :type, :date, :comm) "
                      "WHERE piece_id = :id");
        query.bindValue(":id", id);
    }


    /*
    //prepare insert&upsert statment:
    query.prepare("INSERT INTO pieces (piece_id, status, name, customer, type, date, comment)"
                  "VALUES (:id, :stat, :name, :cust, :type, :date, :comm)");

                  " ON CONFLICT(piece_id) DO UPDATE SET status=excluded.status, name=excluded.name, "
                  "customer=excluded.customer, type=excluded.type, date=excluded.date, "
                  "comment=excluded.comment");
    */
    query.bindValue(":stat", status);
    query.bindValue(":name", name);
    query.bindValue(":cust", customer);
    query.bindValue(":type", type);
    query.bindValue(":date", date.toString());
    query.bindValue(":comm", comment);

    if (!query.exec())
    {
        printf("error while saving piece to database\n");
        printf("%s", query.lastError().text().toStdString().c_str());
        exit(EXIT_FAILURE);
    }

    if (id == 0)
    {
        id = query.lastInsertId().toInt();
        printf("row nr. %d inserted\n", id);
    }

}

