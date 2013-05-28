#ifndef ELECTROINCMAP_H
#define ELECTROINCMAP_H

#include <vector>

#include <QObject>
#include <QString>
#include <QByteArray>

#include "emap/almanac.h"
#include "emap/routesection.h"
#include "emap/kilometerpost.h"

namespace Navigation
{

class ElectroincMap : public QObject
{
    Q_OBJECT


private:
    vector<KilometerPost *> allPosts;
    list<RouteSection *> sections;

    list<KilometerPost *> nearPosts;
    KilometerPost *targetPost;
    KilometerPost *departPost;

    list<KilometerPost *> getPostsInArea(double lat, double lon, double radius);
    list<KilometerPost *> getPostsInArea(vector<KilometerPost *> &source, double lat, double lon, double radius);

    double x;
    int trackNumber;

    class ApproachingPoint
    {
    public:
        double x;
        double r;
    };

    class PostApproach
    {
    public:
        PostApproach();
        KilometerPost *post;
        vector<ApproachingPoint> getExtremumApproaches();
        double parabolizeX(vector<ApproachingPoint> aprs);
        bool pushApproaching(ApproachingPoint p);
        bool achived;
        double getX();
        double minimalApproach;
        list<ApproachingPoint> aPoints;
    private:
    };
    list<ElectroincMap::PostApproach> postApproaches;
    void syncPostApproaches(list<KilometerPost *> posts);
    KilometerPost &projectNextPost(KilometerPost forPost);

public:
    explicit ElectroincMap(QObject *parent = 0);
    void load(QString fileName);

signals:
    void onPostDetected(KilometerPost post, double x);

public slots:
    void setMetrometer(double value);
    void setTrackNumber(int value);
    void checkMap(double lat, double lon);
};

}

#endif // ELECTROINCMAP_H
