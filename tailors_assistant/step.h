#ifndef STEP_H
#define STEP_H

#include <QObject>
#include <QString>

class Step : public QObject
{
    Q_OBJECT
public:
    explicit Step(QObject *parent = nullptr);

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
    //add more properties here if necessary
};

#endif // STEP_H
