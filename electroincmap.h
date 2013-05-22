#ifndef ELECTROINCMAP_H
#define ELECTROINCMAP_H

#include "emap/routesection.h"
#include <QObject>
#include "vector"

class ElectroincMap : public QObject
{
    Q_OBJECT
public:
    explicit ElectroincMap(QObject *parent = 0);
    void load(QString fileName);

signals:

public slots:
    void setMetrometer(double value);
    void checkMap(double lat, double lon);

private:
    vector<KilometerPost> allPosts;
    list<RouteSection> sections;

    vector<KilometerPost> getPostsInArea(double radius);

    double x;
};

#endif // ELECTROINCMAP_H
