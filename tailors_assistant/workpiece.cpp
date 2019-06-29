#include "workpiece.h"

WorkPiece::WorkPiece(QObject *parent) : QObject(parent)
{
    //initialize with default values:
    id = 0;
    date = QDate::currentDate();
    status = offer;
    customer = "";
    name = "";
    type = "";
    comment = "";
    //picture = NULL;
    sums = "";
}

//destructor
WorkPiece::~WorkPiece()
{
    deleteSteps();
}

void WorkPiece::deleteSteps()
{
    for (int i = 0; i < steps.length(); i++)
    {
         delete steps.at(i);
    }
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

    //if there are any steps,
    //save piece and refresh view on type change
    //for correct functionality of type filter:
    if (steps.length() > 0)
    {
        savePieceToDatabase();
        for (int i = 0; i < steps.length(); i++)
        {
            steps.at(i)->calculateStatistics(this);
        }
        emit stepOrderChanged();
    }
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

QVector<Step*>* WorkPiece::getStepsPointer()
{
    return &steps;
}

void WorkPiece::addStep(Step* step)
{
    steps.append(step);
    emit stepOrderChanged();
}

void WorkPiece::deleteStep(int index)
{
    steps.remove(index);
    emit stepOrderChanged();
}

void WorkPiece::deleteAllSteps()
{
    deleteSteps();
    steps.clear();
    emit stepOrderChanged();
}

void WorkPiece::moveStepBack(int index)
{
    if (index > 0)
    {
        steps.move(index, index-1);
    }
    emit stepOrderChanged();
}

void WorkPiece::moveStepForward(int index)
{
    if (index < (steps.length()-1))
    {
        steps.move(index, index+1);
    }
    emit stepOrderChanged();
}

QString WorkPiece::getName() const
{
    return name;
}

void WorkPiece::setName(const QString& value)
{
    name = value;
}

void WorkPiece::setSums(const QString& value)
{
    sums = value;
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
    deleteSteps(); //delete from memory
    steps.clear(); //remove from vector

    //get data sets from db for all steps of this piece:
    QSqlQuery query;
    query.prepare("SELECT * FROM steps WHERE piece = (:id) ORDER BY ordinal_no ASC");
    query.bindValue(":id", this->getId());
    bool z = query.exec();
    if (z)
    {
        printf("successfully loaded steps from db.\n");
    }
    else
    {
        printf("%s", query.lastError().text().toStdString().c_str());
    }
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

        if (status == offer)
        {
            nextStep->calculateStatistics(this);
        }

        steps.append(nextStep);
        printf("loaded step: %s (%d)\n", nextStep->getName().toStdString().c_str(), nextStep->getId());
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
    bool x = query.exec();
    if (x)
    {
        printf("successfully deleted step(s) from db.\n");
    }
    else
    {
        printf("deleting step(s) from db failed.\n");
    }

    //save all the current steps:
    for (int i=0; i<steps.length(); i++)
    {
        Step *s = steps.at(i);
        query.prepare("INSERT INTO steps (piece, name, count, ordinal_no, minutes_all, seam_type, material, detail, filter_seam_type, filter_material, filter_detail, filter_piece_type, comment) "
                      "VALUES (:pid, :name, :ct, :on, :min, :st, :mat, :det, :fst, :fmat, :fdet, :fpt, :comm)");
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

        bool y = query.exec();
        if (y)
        {
            printf("added %d new step to db: %s (%d)\n", y, s->getName().toStdString().c_str(), s->getId());
        }
        else
        {
            printf("%s\n", query.lastError().text().toStdString().c_str());
        }
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
        printf("Inserted new row into pieces table: %s (%d)\n", name.toStdString().c_str(), id);
    }

    saveStepsToDatabase();
}

bool WorkPiece::isEmpty()
{
    return((id == 0)&&(name.length() == 0));
}

//slot to be called when a textfile shall be exported
//parameter is the target filename including the complete path
bool WorkPiece::exportToTextfile(const QString filename)
{
    QFile exportfile;

    exportfile.setFileName(filename);

    if (!exportfile.exists()) //if the file does not exist yet
    {
        QDir dir;
        //if the directory to where the file should be written does not exist yet:
        if (!dir.exists(QFileInfo(exportfile).absolutePath()))
        {
            //try to create the dir:
            bool ok = dir.mkpath(QFileInfo(exportfile).absolutePath());
            if (!ok)
            {
                printf("the directory for the export file could not be created");
                return false;
            }
        }
    }
    else //if the file already existed (and shall be overwritten)
    {
        //anything special to do here?
    }

    //open (or create) the file with (over)write access:
    bool ok = exportfile.open(QIODevice::WriteOnly);
    if (!ok)
    {
        printf("export file could not be opened or created");
        return false;
    }

    QString exportText = getExportableText();

    //write "defaultText" to the file:
    QTextStream out(&exportfile);
    out << exportText;

    //close the file connection and return:
    exportfile.close();
    return true;
}

QString WorkPiece::getExportableText()
{
    QString text = "Tailor's Assistant - ";

    if (status == record)
    {
        text += "Werkstück\n"
                "==============================\n\n";
    }
    if (status == offer)
    {
        text += "Angebot\n"
                "============================\n\n";
    }

    text += customer + " – " + name + " – " + date.toString("dd.MM.yyyy") + "\n";
    text += "Typ: " + type + "\n";
    text += "Anmerkungen: \n" + comment + "\n\n";

    //datastructure for counting how wide the colums should be:
    int maxletters[12];
    maxletters[0] = 3; //max length of count
    maxletters[1] = 8; //max length of name
    maxletters[2] = 8; //max length of seam type
    maxletters[3] = 9; //max length of material
    maxletters[4] = 7; //max length of detail
    maxletters[5] = 6; //length of filters
    maxletters[6] = 3; //max length of base data
    maxletters[7] = 4; //max length of min time
    maxletters[8] = 4; //max length of med time
    maxletters[9] = 4; //max length of avg time
    maxletters[10] = 4; //max length of max time
    maxletters[11] = 5; //max length of man time

    for (int i = 0; i < steps.length(); i++)
    {
        Step* step = steps.at(i);
        if (step->getName().length() > maxletters[1]) maxletters[1] = step->getName().length();
        if (step->getSeamType().length() > maxletters[2]) maxletters[2] = step->getSeamType().length();
        if (step->getMaterial().length() > maxletters[3]) maxletters[3] = step->getMaterial().length();
        if (step->getDetail().length() > maxletters[4]) maxletters[4] = step->getDetail().length();
    }

    QString tc = "#   ";
    QString tn = "Schritt";
    while (tn.length() <= maxletters[1]) {tn += " ";}
    QString ts = "Nahttyp";
    while (ts.length() <= maxletters[2]) {ts += " ";}
    QString tm = "Material";
    while (tm.length() <= maxletters[3]) {tm += " ";}
    QString td = "Detail";
    while (td.length() <= maxletters[4]) {td += " ";}

    text += tc + tn + ts + tm + td;

    if (status == offer)
    {
        QString tf = "Filter";
        while (tf.length() <= maxletters[5]) {tf += " ";}
        QString tbd = "db";
        while (tbd.length() <= maxletters[6]) {tbd += " ";}
        QString tmin = "min";
        while (tmin.length() <= maxletters[7]) {tmin += " ";}
        QString tmed = "med";
        while (tmed.length() <= maxletters[8]) {tmed += " ";}
        QString tavg = "avg";
        while (tavg.length() <= maxletters[9]) {tavg += " ";}
        QString tmax = "max";
        while (tmax.length() <= maxletters[10]) {tmax += " ";}
        QString tman = "man";
        while (tman.length() <= maxletters[11]) {tman += " ";}

        text += tf + tbd + tmin + tmed + tavg + tmax + tman;
    }
    if (status == record)
    {
        QString tt = "Zeit";
        while (tt.length() <= maxletters[11]) {tt += " ";}

        text += tt;
    }

    text += "Kommentar\n\n";

    for (int i = 0; i < steps.length(); i++)
    {
        Step* step = steps.at(i);
        QString sc = QString::number(step->getCount());
        while (sc.length() <= maxletters[0]) {sc += " ";}
        QString sn = step->getName();
        while (sn.length() <= maxletters[1]) {sn += " ";}
        QString sst = step->getSeamType();
        while (sst.length() <= maxletters[2]) {sst += " ";}
        QString smat = step->getMaterial();
        while (smat.length() <= maxletters[3]) {smat += " ";}
        QString sdet = step->getDetail();
        while (sdet.length() <= maxletters[4]) {sdet += " ";}
        QString filt = step->getFilters();
        while (filt.length() <= maxletters[5]) {filt += " ";}
        QString sbd = QString::number(step->getBaseDatasets());
        while (sbd.length() <= maxletters[6]) {sbd += " ";}
        QString smin = QString::number(step->getMin());
        while (smin.length() <= maxletters[7]) {smin += " ";}
        QString smed = QString::number(step->getMed());
        while (smed.length() <= maxletters[8]) {smed += " ";}
        QString savg = QString::number(step->getAvg());
        while (savg.length() <= maxletters[9]) {savg += " ";}
        QString smax = QString::number(step->getMax());
        while (smax.length() <= maxletters[10]) {smax += " ";}
        QString sman = QString::number(step->getMinutesAll());
        while (sman.length() <= maxletters[11]) {sman += " ";}
        QString scomm = step->getComment();

        text += sc + sn + sst + smat + sdet;
        if (status == offer)
        {
            text += filt + sbd + smin + smed + savg + smax;
        }
        text += sman + scomm + "\n";
    }

    text += "\n" + sums;

    return text;
}

