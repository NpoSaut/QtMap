#include "electroincmap.h"
#include "emap/kilometerpost.h"
#include "emap/routesection.h"
#include "QFile"
#include <QDebug>

#define D_foreach(l, T, itr) for (list<T>::iterator itr=l.begin(); itr != l.end(); ++itr)
#define D_foreach_vec(l, T, itr) for (vector<T>::iterator itr=l.begin(); itr != l.end(); ++itr)

#include "vector"

using namespace std;

ElectroincMap::ElectroincMap(QObject *parent) :
    QObject(parent)
{
}



// Загрузка электронной карты из файла
void ElectroincMap::load(QString fileName)
{
    QFile mapFile (fileName);
    mapFile.open(QIODevice::ReadOnly);
    QByteArray data = mapFile.readAll ();

    // Количество столбов
    int postsCount = data[2] | data[3] << 8;

    int sectionId = 1;
    RouteSection section(sectionId);
    // Считываем столбы
    for (int i = 0; i < postsCount; i++)
    {
        KilometerPost currentPost = KilometerPost::loadFrom(data, 9, i);
        allPosts.push_back(currentPost);
        section.Posts.push_back(currentPost);
        currentPost.sectionId = sectionId;

        if (currentPost.position == kpp_end)
        {
            sections.push_back(section);
            RouteSection empty(sectionId);
            section = empty;
        }
    }
}

void ElectroincMap::setMetrometer(double value)
{
    if (value != x)
        x = value;
}

void ElectroincMap::setTrackNumber(int value)
{
    if (value != trackNumber)
        trackNumber = value;
}


void ElectroincMap::checkMap(double lat, double lon)
{
    qDebug() << " lat " << lat << "  lon " << lon << "    x " << x;

    nearPosts = getPostsInArea(lat, lon, 10000);

    // Находим ближайший километровый столб (за исключением "столба отправления")
    KilometerPost closestPost;
    double closestDist = 1e20;
    foreach (KilometerPost p, nearPosts)
    {
        if (p != departPost)
        {
            double d = p.distanceTo(lat, lon);
            if (d < closestDist)
            {
                closestPost = p;
                closestDist = d;
            }
        }
    }

    // Находим ближайшие столбы
    list<KilometerPost> nextPosts = getPostsInArea(lat, lon, 3000);     // Находим столбы на расстоянии 3х километров - казалось бы, только к ним мы можем ехать сейчас

    nextPosts.remove(departPost);       // Удаляем оттуда столб отправления
    syncPostApproaches(nextPosts);      // И синхронизируем список приближений

    // Вычисляем приближения для всех точек
    //foreach (PostApproach pa, postApproaches) {
    D_foreach(postApproaches, PostApproach, pait)
    {
        PostApproach &pa = *pait;
        // Добавляем текущую точку приближения
        ApproachingPoint ap;
        ap.x = x;   ap.r = pa.post.distanceTo(lat, lon);
        bool ready = pa.pushApproaching(ap);
        qDebug() << "   post " << pa.post.ordinate << "\tdist " << pa.minimalApproach << "\tfrom " << pa.aPoints.size() << " points";
        if (pa.achived && pa.post == closestPost)
        {
            // Трубим
            emit onPostDetected(pa.post, pa.getX());

            qDebug() << "FIXED TO POST: " << pa.post.ordinate << "m   x: " << pa.getX();

            departPost = targetPost;

            KilometerPost ttt = projectNextPost(departPost);
            qDebug() << "NEXT: " << ttt.ordinate << endl << endl;
            break;
        }
    }

    // Если ближайший столб равен целевому столбу (т.е. цель движения не изменилась)
    if (targetPost == closestPost)
    {
    }
    else
    {
        targetPost = closestPost;
    }
}

// Возвращает все километровые солбы в указанном радиусе
list<KilometerPost> ElectroincMap::getPostsInArea(double lat, double lon, double radius)
{
    return getPostsInArea(allPosts, lat, lon, radius);
}
list<KilometerPost> ElectroincMap::getPostsInArea(vector<KilometerPost> source, double lat, double lon, double radius)
{
    list<KilometerPost> res;

    double radiusEstimation = KilometerPost::metersToEstimation(radius);
    foreach (KilometerPost p, source) {
        //if (p.estimateDistanceTo(lat, lon) <= radiusEstimation)
        if (p.distanceTo (lat, lon) <= radius)
        {
            res.push_back(p);
        }
    }
    return res;
}

void ElectroincMap::syncPostApproaches(list<KilometerPost> posts)
{
    list<PostApproach> newApproaches;
    foreach (KilometerPost p, posts) {
        PostApproach pa;
        pa.post = p;
        foreach (PostApproach ipa, postApproaches) {
            if (ipa.post == p)
            {
                pa = ipa;
                break;
            }
        }
        newApproaches.push_back(pa);
    }
    postApproaches = newApproaches;
}

KilometerPost &ElectroincMap::projectNextPost(KilometerPost forPost)
{
    int direction = forPost.direction;
    KilometerPost &res = KilometerPost();
    double shiftToRes = 1e20;
    D_foreach(nearPosts, KilometerPost, kpi)
    {
        KilometerPost &kp = *kpi;
        if (kp.sectionId == forPost.sectionId)
        {
            double ordinateShift = direction * (kp.ordinate - forPost.ordinate);
            if (ordinateShift > 0 && ordinateShift < shiftToRes)
            {
                shiftToRes = ordinateShift;
                res = kp;
            }
        }
    }
    return res;
}


ElectroincMap::PostApproach::PostApproach()
{
    achived = false;
    minimalApproach = 1e20;
}

vector<ElectroincMap::ApproachingPoint> ElectroincMap::PostApproach::getExtremumApproaches()
{
    vector<ApproachingPoint> res;

    ApproachingPoint prev_ap;
    D_foreach(aPoints, ApproachingPoint, itr)
    {
        ApproachingPoint ap = ApproachingPoint(*itr);
        if (ap.r == minimalApproach)
        {
            res.push_back(prev_ap);
            res.push_back(ap);
            res.push_back(*(++itr));
        }
        prev_ap = ap;
    }

    return res;
}

double ElectroincMap::PostApproach::parabolizeX(vector<ElectroincMap::ApproachingPoint> aprs)
{
    double a = (aprs[2].r-(aprs[2].x*(aprs[1].r-aprs[0].r) + aprs[1].x*aprs[0].r -aprs[0].x*aprs[1].r)/(aprs[1].x-aprs[0].x))/(aprs[2].x*(aprs[2].x-aprs[0].x-aprs[1].x) + aprs[0].x*aprs[1].x);
    double b = (aprs[1].r-aprs[0].r)/(aprs[1].x-aprs[0].x) - a*(aprs[0].x+aprs[1].x);
    return - b / (2 * a);
}



const double approachIncreaseLimit = 30; // м
bool ElectroincMap::PostApproach::pushApproaching(ElectroincMap::ApproachingPoint ap)
{
    // Удаляем все лишние точки
    while (aPoints.size() >= 20)
        aPoints.pop_front();

    aPoints.push_back(ap);

    // Проверяем, ближе ли эта точка к столбу чем другие
    if (ap.r < minimalApproach)
    {
        minimalApproach = ap.r;
    }
    // Если мы не приближаемся к столбу - проверяем, не отдалились ли мы от его а "достоверное" расстояние
    else
    {
        achived = ap.r - minimalApproach >= approachIncreaseLimit;
    }
    return achived;
}

double ElectroincMap::PostApproach::getX()
{
    return parabolizeX(getExtremumApproaches());
}
