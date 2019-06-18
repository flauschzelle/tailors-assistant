#include "step.h"

Step::Step(QObject *parent) : QObject(parent)
{

}

void Step::setupTable()
{
    QString table_def = "CREATE TABLE IF NOT EXISTS steps "
                        "(step_id INTEGER PRIMARY KEY NOT NULL, "
                        "piece INTEGER NOT NULL REFERENCES pieces(piece_id), "
                        "name TEXT, "
                        "count INTEGER, "
                        "ordinal_no INTEGER,"
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
}

int Step::getMinutesAll() const
{
    return minutesAll;
}

void Step::setMinutesAll(int value)
{
    minutesAll = value;
}

int Step::getCount() const
{
    return count;
}

void Step::setCount(int value)
{
    count = value;
}

QString Step::getSeamType() const
{
    return seamType;
}

void Step::setSeamType(const QString& value)
{
    seamType = value;
}

QString Step::getMaterial() const
{
    return material;
}

void Step::setMaterial(const QString& value)
{
    material = value;
}

QString Step::getDetail() const
{
    return detail;
}

void Step::setDetail(const QString& value)
{
    detail = value;
}

QString Step::getComment() const
{
    return comment;
}

void Step::setComment(const QString& value)
{
    comment = value;
}

bool Step::getFilterSeamType() const
{
    return filterSeamType;
}

void Step::setFilterSeamType(bool value)
{
    filterSeamType = value;
}

bool Step::getFilterMaterial() const
{
    return filterMaterial;
}

void Step::setFilterMaterial(bool value)
{
    filterMaterial = value;
}

bool Step::getFilterDetail() const
{
    return filterDetail;
}

void Step::setFilterDetail(bool value)
{
    filterDetail = value;
}

bool Step::getFilterPieceType() const
{
    return filterPieceType;
}

void Step::setFilterPieceType(bool value)
{
    filterPieceType = value;
}
