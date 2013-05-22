#ifndef ELECTROINCMAP_H
#define ELECTROINCMAP_H

#include "emap/routesection.h"
#include <QObject>
#include "vector"

class ElectroincMap : public QObject
{
    Q_OBJECT


private:
    vector<KilometerPost> allPosts;
    list<RouteSection> sections;

    list<KilometerPost> nearPosts;
    KilometerPost targetPost;
    KilometerPost departPost;

    list<KilometerPost> getPostsInArea(double lat, double lon, double radius);

    double x;

    class ApproachingPoint
    {
    public:
        double x;
        double r;
    };
    list<ApproachingPoint> aPoints;
    double minimalApproach;

    vector<ApproachingPoint> getExtremumApproaches();

    double parabolizeX(vector<ApproachingPoint> aprs);

public:
    explicit ElectroincMap(QObject *parent = 0);
    void load(QString fileName);

signals:
    void onPostDetected(KilometerPost post, double x);

public slots:
    void setMetrometer(double value);
    void checkMap(double lat, double lon);
};

#endif // ELECTROINCMAP_H
