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
    bool firstEntery;
    vector<KilometerPost *> allPosts;
    list<RouteSection *> sections;

    list<KilometerPost *> nearPosts;
    KilometerPost *targetPost;
    KilometerPost *departPost;
    double departX;

    list<KilometerPost *> getPostsInArea(double lat, double lon, double radius);
    list<KilometerPost *> getPostsInArea(vector<KilometerPost *> &source, double lat, double lon, double radius);

    double x;
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

    /**
     * @brief getMyRail Находит путь, на котором ты окажешься
     * @param post      Столб, для которого ты хочешь найти путь
     * @return          Ссылку на путь, по которому поедешь, если он есть. Иначе null
     */
    Rail *getMyRail(const KilometerPost *post);

public:
    explicit ElectroincMap(QObject *parent = 0);
    void load(QString fileName);

signals:
    void onPostDetected(KilometerPost post, double x);
    void onUpcomingTargets(vector<EMapTarget>);

public slots:
    void setMetrometer(double value);
    void setTrackNumber(int value);
    void checkMap(double lat, double lon);
};

}

#endif // ELECTROINCMAP_H
