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

    double _prewX;
    double x;
    double l;

    double lat1, lon1;

    int trackNumber;

    class ApproachingPoint
    {
    public:
        ApproachingPoint()
            : x(0), r(1e20)
        { }
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
              minimalApproach(1e20),
              aPoints(),
              minPoints(),
              minCandidateActualCount(0)
        { }

        KilometerPost *post;
        bool pushApproaching(ApproachingPoint p);
        bool achived;
        double getX();
        double minimalApproach;
        double approachingSpeed;
        double lastApproach;
        list<ApproachingPoint> aPoints;
        int minCandidateActualCount;
        ApproachingPoint minCandidate[3];
        vector<ApproachingPoint> minPoints;
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

    int customDirection = +1;
    int myDirection() { return trackNumber != 0 ? getDirection (trackNumber, getCurrentKilometer()) : customDirection ; }
    static int getDirection(int trackNumber, KilometerPost *kilometer);

    void checkOrdinate(int delta);

    int closestObjectX;
    void checkObjects();

    double ordinate;

    int trainLength;

public:
    explicit ElectroincMap(QObject *parent = 0);
    void load(QString fileName);
    bool getIsLocated();
    double getOrdinate();

signals:
    void onPostDetected(KilometerPost post, double x);
    void onUpcomingTargets(std::vector<EMapTarget>);
    void isLocatedChanged(bool value);
    void ordinateUpdated(int value);
    void activityChanged(bool active);

public slots:
    // Устанавливает текущее значение проиденного пути
    void setMetrometer(int value);
    void resetMetrometer();

    // Устанавливает текущий номер пути
    void setTrackNumber(int value);

    // Задаёт текущие координаты и запускает процесс привязки
    void checkMap(double lat, double lon);

    // Задаёт длину состава (в метрах)
    void setTrainLength(int value);

    // Задаёт начальную ординату в кастомном режиме
    void setCustomOrdinate(int value);
    // Задаёт направление в кастомном режиме
    void setCustomDirection(int value);
};

}

#endif // ELECTROINCMAP_H
