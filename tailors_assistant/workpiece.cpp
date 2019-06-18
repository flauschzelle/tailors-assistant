#include "workpiece.h"

WorkPiece::WorkPiece(QObject *parent) : QObject(parent)
{
    id = 0; //initialize id with default value
    date = QDate(1970,1,1); //initialize date with default value
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

QVector<Step*> WorkPiece::getSteps()
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
void WorkPiece::loadStepsFromDatabase()
{
    //make sure that this is not a new & unsaved piece
    if (id == 0)
    {
        savePieceToDatabase();
    }
    //clear any pre-exisiting steps:
    steps.clear();

    //get data sets from db for all steps of this piece:
    QSqlQuery query;
    query.prepare("SELECT * FROM steps WHERE piece = (:id) ORDER BY ordinal_no ASC");
    query.bindValue(":id", this->getId());
    query.exec();
    //go through the result set:
    while (query.next())
    {
        Step *nextStep = new Step();
        nextStep->setId(query.value(0).toInt());
        nextStep->setName(query.value(2).toString());
        nextStep->setCount(query.value(3).toInt());
        nextStep->setMinutesAll(query.value(5).toInt());
        nextStep->setSeamType(query.value(6).toString());
        nextStep->setMaterial(query.value(7).toString());
        nextStep->setDetail(query.value(8).toString());
        nextStep->setFilterSeamType(query.value(9).toBool());
        nextStep->setFilterMaterial(query.value(10).toBool());
        nextStep->setFilterDetail(query.value(11).toBool());
        nextStep->setFilterPieceType(query.value(12).toBool());
        nextStep->setComment(query.value(13).toString());

        steps.append(nextStep);
    }
}

void WorkPiece::saveStepsToDatabase()
{
    //start transaction in the db:
    QSqlDatabase::database().transaction();

    //delete all steps of the current piece from the db:
    QSqlQuery query;
    query.prepare("DELETE FROM steps WHERE piece = (:id)");
    query.bindValue(":id", id);
    query.exec();

    //save all the current steps:
    for (int i=0; i<steps.length(); i++)
    {
        Step *s = steps.at(i);
        query.prepare("INSERT INTO steps (piece, name, count, ordinal_no, minutes_all, seam_type, material, detail, filter_seam_type, filter_material, filter_detail, filter_piece_type, comment) "
                      "VALUES (:pid, :name, :ct, :on, :min, :st, :mat, :det, :fst, :fmat; fdet:, :fpt, :comm)");
        query.bindValue(":pid", id);
        query.bindValue(":name", s->getName());
        query.bindValue(":ct", s->getCount());
        query.bindValue(":on", i);
        query.bindValue(":min", s->getMinutesAll());
        query.bindValue(":st", s->getSeamType());
        query.bindValue(":mat", s->getMaterial());
        query.bindValue(":det", s->getDetail());
        query.bindValue(":fst", s->getFilterSeamType());
        query.bindValue(":fmat", s->getFilterMaterial());
        query.bindValue(":fdet", s->getFilterDetail());
        query.bindValue(":fpt", s->getFilterPieceType());
        query.bindValue(":comm", s->getComment());

        query.exec();
    }

    //commit transaction to the db:
    if (!QSqlDatabase::database().commit())
    {
        printf("error while trying to save steps to the database");
        exit(EXIT_FAILURE);
    }
}

void WorkPiece::savePieceToDatabase()
{
    QSqlQuery query;

    if (id == 0)
    {
        query.prepare("INSERT INTO pieces (status, name, customer, type, date, comment) "
                      "VALUES (:stat, :name, :cust, :type, :date, :comm)");
    }
    else
    {
        QString q = "UPDATE pieces "
                    "SET (status, name, customer, type, date, comment)"
                    " = "
                    "(:stat, :name, :cust, :type, :date, :comm) "
                    "WHERE piece_id=";
        q.append(QString::number(id));
        query.prepare(q);

    }
    query.bindValue(":stat", status);
    query.bindValue(":name", name);
    query.bindValue(":cust", customer);
    query.bindValue(":type", type);
    query.bindValue(":date", date.toString(Qt::ISODate));
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

    saveStepsToDatabase();
}

bool WorkPiece::isEmpty()
{
    return((id == 0)&&(name.length() == 0));
}

