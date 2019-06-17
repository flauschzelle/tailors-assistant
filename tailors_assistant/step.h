#ifndef STEP_H
#define STEP_H

#include <QObject>
#include <QString>
#include <QSql>
#include <QSqlQuery>

class Step : public QObject
{
    Q_OBJECT
public:
    explicit Step(QObject *parent = nullptr);

    static QString table_def;

    static void setupTable();

signals:

public slots:

private:
    int id;
    QString name;
    int minutesAll;
    int secondsOne;
    int count;
    QString seamType;
    QString material;
    QString detail;
    QString comment;
    bool filterSeamType;
    bool filterMaterial;
    bool filterDetail;
    bool filterPieceType;
    int min;
    int med;
    int avg;
    int max;
    int baseDatasets;
    //add more properties here if necessary
};

#endif // STEP_H
