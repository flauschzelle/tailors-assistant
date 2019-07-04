#include "step.h"
#include "workpiece.h"

Step::Step(QObject *parent) : QObject(parent)
{
    //initialize members with default values:
    id = 0;
    name = "";
    minutesAll = 0;
    count = 0;
    seamType = "";
    material = "";
    detail = "";
    comment = "";
    filterSeamType = false;
    filterMaterial = false;
    filterDetail = false;
    filterPieceType = false;
    min = 0;
    med = 0;
    avg = 0;
    max = 0;
    baseDatasets = 0;
}

void Step::setupTable()
{
    QString table_def = "CREATE TABLE IF NOT EXISTS steps "
                        "(step_id INTEGER PRIMARY KEY NOT NULL, "
                        "piece INTEGER NOT NULL REFERENCES pieces(piece_id), "
                        "name TEXT, "
                        "count INTEGER, "
                        "ordinal_no INTEGER, "
                        "minutes_all INTEGER, "
                        "seam_type TEXT, "
                        "material TEXT, "
                        "detail TEXT, "
                        "filter_seam_type BOOLEAN, "
                        "filter_material BOOLEAN, "
                        "filter_detail BOOLEAN, "
                        "filter_piece_type BOOLEAN, "
                        "comment TEXT "
                        ")";
    QSqlQuery query;

    if (!query.exec(table_def))
    {
        printf("error while setting up the steps table\n");
    }
}

int Step::getId() const
{
    return id;
}

void Step::setId(int value)
{
    id = value;
}

QString Step::getName() const
{
    return name;
}

void Step::setName(const QString& value)
{
    name = value;
    emit stepDataChanged(this);
}

int Step::getMinutesAll() const
{
    return minutesAll;
}

void Step::setMinutesAll(int value)
{
    minutesAll = value;
    emit stepDataChanged(this);
}

int Step::getCount() const
{
    return count;
}

void Step::setCount(int value)
{
    count = value;
    emit stepDataChanged(this);
}

QString Step::getSeamType() const
{
    return seamType;
}

void Step::setSeamType(const QString& value)
{
    seamType = value;
    emit stepDataChanged(this);
}

QString Step::getMaterial() const
{
    return material;
}

void Step::setMaterial(const QString& value)
{
    material = value;
    emit stepDataChanged(this);
}

QString Step::getDetail() const
{
    return detail;
}

void Step::setDetail(const QString& value)
{
    detail = value;
    emit stepDataChanged(this);
}

QString Step::getComment() const
{
    return comment;
}

void Step::setComment(const QString& value)
{
    comment = value;
    emit stepDataChanged(this);
}

bool Step::getFilterSeamType() const
{
    return filterSeamType;
}

void Step::setFilterSeamType(bool value)
{
    filterSeamType = value;
    emit stepDataChanged(this);
}

bool Step::getFilterMaterial() const
{
    return filterMaterial;
}

void Step::setFilterMaterial(bool value)
{
    filterMaterial = value;
    emit stepDataChanged(this);
}

bool Step::getFilterDetail() const
{
    return filterDetail;
}

void Step::setFilterDetail(bool value)
{
    filterDetail = value;
    emit stepDataChanged(this);
}

bool Step::getFilterPieceType() const
{
    return filterPieceType;
}

QString Step::getFilters() const
{
    QString filters = "";
    if (filterSeamType)
    {
        filters.append("N");
    }
    else
    {
        filters.append("-");
    }

    if (filterMaterial)
    {
        filters.append("M");
    }
    else
    {
        filters.append("-");
    }

    if (filterDetail)
    {
        filters.append("D");
    }
    else
    {
        filters.append("-");
    }

    if (filterPieceType)
    {
        filters.append("W");
    }
    else
    {
        filters.append("-");
    }

    return filters;
}

void Step::setFilterPieceType(bool value)
{
    filterPieceType = value;
    emit stepDataChanged(this);
}

int Step::getMax() const
{
    return max;
}

//the parameter determines if the median or the other values shall be calculated
void Step::calculateStatsPart(bool median, WorkPiece * piece)
{
    //prepare query: -------------------------------
    QSqlQuery query;
    QString q = "WITH tmp AS "
                "( SELECT step_id, count, minutes_all, "
                "minutes_all*60/count AS seconds_one "
                "FROM steps "
                "WHERE piece != (:pid) AND name = (:name) "
                "AND piece IN (SELECT piece_id FROM pieces WHERE status = 0)";
    if (filterSeamType)
    {
        q = q + " AND seam_type = (:st)";
    }
    if (filterMaterial)
    {
        q = q + " AND material = (:mat)";
    }
    if (filterDetail)
    {
        q = q + " AND detail = (:det)";
    }
    if (filterPieceType)
    {
        q = q + " AND piece IN (SELECT piece_id FROM pieces WHERE type = (:pt))";
    }
    if (!median)
    {
        //this part calculates data, min, max, and avg:
        q = q + ") "
                "SELECT COUNT(step_id), "
                "MIN(seconds_one), "
                "MAX(seconds_one), "
                "AVG(seconds_one) FROM tmp";
    }
    if (median)
    {
        //this part calculates the median:
        q = q + " ORDER BY seconds_one) "
                "SELECT AVG (seconds_one) "
                "FROM (SELECT seconds_one "
                "FROM tmp "
                "ORDER BY seconds_one "
                "LIMIT 2 - (SELECT COUNT(*) FROM tmp) % 2 "
                "OFFSET (SELECT (COUNT(*) - 1) / 2 FROM tmp))";
    }


    //printf ("%s\n", q.toStdString().c_str()); //for debugging

    query.prepare(q);
    query.bindValue(":pid", piece->getId());
    query.bindValue(":name", name);
    if (filterSeamType)
    {
        query.bindValue(":st", seamType);
    }
    if (filterMaterial)
    {
        query.bindValue(":mat", material);
    }
    if (filterDetail)
    {
        query.bindValue(":det", detail);
    }
    if (filterPieceType)
    {
        query.bindValue(":pt", piece->getType());
    }
    //execute the prepared query: -------------------
    bool s = query.exec();
    if (s == false)
    {
        printf("error while getting statistics from database:\n");
        printf("%s", query.lastError().text().toStdString().c_str());
        exit(EXIT_FAILURE);
    }
    //get the values from the first result row:
    if (query.next())
    {
        if (!median)
        {
            baseDatasets = query.value(0).toInt();
            min = (query.value(1).toInt()*count/60);
            max = (query.value(2).toInt()*count/60);
            avg = (query.value(3).toInt()*count/60);
        }
        if (median)
        {
            med = (query.value(0).toInt()*count/60);
        }
    }

}

//this gets the statistical data for this step from the db,
//given the piece to which the current step belongs as a parameter
void Step::calculateStatistics(WorkPiece* piece)
{
    //calculate other data:
    calculateStatsPart(false, piece);
    //calculate median:
    calculateStatsPart(true, piece);
    //signal that the table shall refresh the data
    emit stepDataChanged(this);
}

int Step::getAvg() const
{
    return avg;
}

int Step::getMed() const
{
    return med;
}

int Step::getMin() const
{
    return min;
}

int Step::getBaseDatasets() const
{
    return baseDatasets;
}
