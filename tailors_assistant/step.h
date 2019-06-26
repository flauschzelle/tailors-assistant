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
    int getMinutesAll() const;
    int getCount() const;
    QString getSeamType() const;
    QString getMaterial() const;
    QString getDetail() const;
    QString getComment() const;
    bool getFilterSeamType() const;
    bool getFilterMaterial() const;
    bool getFilterDetail() const;
    bool getFilterPieceType() const;
    QString getFilters() const;

    int getBaseDatasets() const;
    int getMin() const;
    int getMed() const;
    int getAvg() const;
    int getMax() const;

signals:
    void stepDataChanged(Step * step);

public slots:
    void setName(const QString& value);
    void setMinutesAll(int value);
    void setCount(int value);
    void setSeamType(const QString& value);
    void setMaterial(const QString& value);
    void setDetail(const QString& value);
    void setComment(const QString& value);
    void setFilterSeamType(bool value);
    void setFilterMaterial(bool value);
    void setFilterDetail(bool value);
    void setFilterPieceType(bool value);

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
