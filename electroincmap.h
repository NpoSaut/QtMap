#ifndef ELECTROINCMAP_H
#define ELECTROINCMAP_H

#include <vector>

#include <QObject>
#include <QString>
#include <QByteArray>

#include "emap/almanac.h"
#include "emap/routesection.h"
#include "emap/kilometerpost.h"
#include "emap/emaptarget.h"

namespace Navigation
{

class ElectroincMap : public QObject
{
    Q_OBJECT


private:
    bool mapLoaded;
    bool xReceived;

    bool isLocated;
    void setIsLocated(bool val);

    bool firstEnter;
    list<KilometerPost *> allPosts;
    list<RouteSection *> sections;

    list<KilometerPost *> nearPosts;
    KilometerPost *targetPost;
    KilometerPost *departPost;
    double departX;

    list<KilometerPost *> getPostsInArea(double lat, double lon, double radius);
    list<KilometerPost *> getPostsInArea(list<KilometerPost *> &source, double lat, double lon, double radius);

    double x;
    double l;

    double lat1, lon1;

    int trackNumber;

    class ApproachingPoint
    {
    public:
        ApproachingPoint(double x, double r)
            : x(x), r(r)
        { }

        double x;
        double r;
    };

    class PostApproach
    {
    public:
        PostApproach(KilometerPost *post)
            : post(post),
              achived(false),
              minimalApproach(1e20)
        { }

        KilometerPost *post;
        bool pushApproaching(ApproachingPoint p);
        bool achived;
        double getX();
        double minimalApproach;
        double approachingSpeed;
        list<ApproachingPoint> aPoints;
    private:
        double parabolizeX(vector<ApproachingPoint> aprs);
        vector<ApproachingPoint> getExtremumApproaches();
        double estimateApproaching();
    };


    list<ElectroincMap::PostApproach *> postApproaches;
    void syncPostApproaches(list<KilometerPost *> posts);
    KilometerPost *projectNextPost(const KilometerPost *forPost, bool goBack = false);
    double getPostApproachWeight(const PostApproach *pa);

    PostApproach *findBestApproach();

    vector<EMapTarget> getNextObjects(const KilometerPost *startPost, double startPostX, int count = 10);

    Rail *getMyRail(const KilometerPost *post);

    KilometerPost *getCurrentKilometer();

    int myDirection() { return getDirection (trackNumber, getCurrentKilometer()); }
    static int getDirection(int trackNumber, KilometerPost *kilometer);

    void checkOrdinate();

    int closestObjectX;
    void checkObjects();

public:
    explicit ElectroincMap(QObject *parent = 0);
    void load(QString fileName);

    double ordinate;

signals:
    void onPostDetected(KilometerPost post, double x);
    void onUpcomingTargets(std::vector<EMapTarget>);
    void isLocatedChanged();

public slots:
    void setMetrometer(int value);
    void setTrackNumber(int value);
    void checkMap(double lat, double lon);
};

}

#endif // ELECTROINCMAP_H
