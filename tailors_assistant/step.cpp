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
                        "minutes_all INTEGER, "
                        "seconds_per INTEGER, "
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
