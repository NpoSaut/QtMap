#include "electroincmap.h"
#include "emap/kilometerpost.h"
#include "emap/routesection.h"
#include "QFile"

#define D_foreach(l, T, itr) for (list<T>::iterator itr=l.begin(); itr != l.end(); ++itr)

#include "vector"

using namespace std;

ElectroincMap::ElectroincMap(QObject *parent) :
    QObject(parent)
{
    minimalApproach = 1e20;
}



// Загрузка электронной карты из файла
void ElectroincMap::load(QString fileName)
{
    QFile mapFile (fileName);
    mapFile.open(QIODevice::ReadOnly);
    QByteArray data = mapFile.readAll ();

    // Количество столбов
    int postsCount = data[2] | data[3] << 8;

    RouteSection section;
    // Считываем столбы
    for (int i = 0; i < postsCount; i++)
    {
        KilometerPost currentPost = KilometerPost::loadFrom(data, 9, i);
        allPosts.push_back(currentPost);
        section.Posts.push_back(currentPost);

        if (currentPost.position == kpp_end)
        {
            sections.push_back(section);
            RouteSection empty;
            section = empty;
        }
    }
}

void ElectroincMap::setMetrometer(double value)
{
    if (value != x)
        x = value;
}

const double approachIncreaseLimit = 30; // м

void ElectroincMap::checkMap(double lat, double lon)
{
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

    // Если ближайший столб равен целевому столбу (т.е. цель движения не изменилась)
    if (targetPost == closestPost)
    {
        // Удаляем все лишние точки
        while (aPoints.size() >= 20)
            aPoints.pop_front();

        // Добавляем текущую точку приближения
        ApproachingPoint ap;
        ap.x = x;   ap.r = targetPost.distanceTo(lat, lon);
        aPoints.push_back(ap);

        // Проверяем, ближе ли эта точка к столбу чем другие
        if (ap.r < minimalApproach)
        {
            minimalApproach = ap.r;
        }
        // А иначе - проверяем, не удалились ли мы от минимума достаточно хорошо
        else if (ap.r - minimalApproach >= approachIncreaseLimit)
        {
            // Заходим сюда если обнаружили приближение/отдаление от столба!

            // Получаем массив трёх важных точек
            vector<ApproachingPoint> approaches = getExtremumApproaches();
            // Параболическим приближением находим хороший X
            double postX = parabolizeX(approaches);

            // Подчищаем хвосты
            aPoints.clear();
            minimalApproach = 1e20;

            // Трубим
            emit onPostDetected(targetPost, x);

            departPost = targetPost;
        }

    }
    else
    {
        targetPost = closestPost;
    }
}

// Возвращает все километровые солбы в указанном радиусе
list<KilometerPost> ElectroincMap::getPostsInArea(double lat, double lon, double radius)
{
    list<KilometerPost> res;

    double radiusEstimation = KilometerPost::metersToEstimation(radius);
    for (int i = 0; i < allPosts.size(); i++)
    {
        KilometerPost p = allPosts[i];
        //if (p.estimateDistanceTo(lat, lon) <= radiusEstimation)
        if (p.distanceTo (lat, lon) <= radius)
        {
            res.push_back(p);
        }
    }
    return res;
}


vector<ElectroincMap::ApproachingPoint> ElectroincMap::getExtremumApproaches()
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

double ElectroincMap::parabolizeX(vector<ElectroincMap::ApproachingPoint> aprs)
{
    double a = (aprs[2].r-(aprs[2].x*(aprs[1].r-aprs[0].r) + aprs[1].x*aprs[0].r -aprs[0].x*aprs[1].r)/(aprs[1].x-aprs[0].x))/(aprs[2].x*(aprs[2].x-aprs[0].x-aprs[1].x) + aprs[0].x*aprs[1].x);
    double b = (aprs[1].r-aprs[0].r)/(aprs[1].x-aprs[0].x) - a*(aprs[0].x+aprs[1].x);
    return - b / (2 * a);
}
