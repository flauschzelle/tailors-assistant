#include "step.h"

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
