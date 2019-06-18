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

    static void setupTable();

    int getId() const;
    void setId(int value);

    QString getName() const;
    void setName(const QString& value);

    int getMinutesAll() const;
    void setMinutesAll(int value);

    int getCount() const;
    void setCount(int value);

    QString getSeamType() const;
    void setSeamType(const QString& value);

    QString getMaterial() const;
    void setMaterial(const QString& value);

    QString getDetail() const;
    void setDetail(const QString& value);

    QString getComment() const;
    void setComment(const QString& value);

    bool getFilterSeamType() const;
    void setFilterSeamType(bool value);

    bool getFilterMaterial() const;
    void setFilterMaterial(bool value);

    bool getFilterDetail() const;
    void setFilterDetail(bool value);

    bool getFilterPieceType() const;
    void setFilterPieceType(bool value);

signals:

public slots:

private:
    int id;
    QString name;
    int minutesAll;
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
