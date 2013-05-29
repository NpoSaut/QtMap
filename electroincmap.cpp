#include <vector>
#include <math.h>

#include <QFile>

#include "electroincmap.h"

using namespace Navigation;
#include <QDebug>

#define D_foreach(l, T, itr) for (list<T>::iterator itr=l.begin(); itr != l.end(); ++itr)
#define D_foreach_vec(l, T, itr) for (vector<T>::iterator itr=l.begin(); itr != l.end(); ++itr)


using namespace std;

#ifdef WIN32

#include <Windows.h>
#define SET_COLOR(fg, bg) SetConsoleTextAttribute(ch, fg + 16*bg)
#define RESET_COLOR SET_COLOR(7,0)

#define CL_BLACK    0
#define CL_BLUE     1
#define CL_GREEN    2
#define CL_CYAN     3
#define CL_RED      4
#define CL_VIOLET   5
#define CL_YELLOW   6
#define CL_WHITE    7

#define CL_LIGHT(color) color + 8
#define CL_GRAY     CL_LIGHT(CL_BLACK)
#define CL_BLUE_L   CL_LIGHT(CL_BLUE)
#define CL_GREEN_L  CL_LIGHT(CL_GREEN)
#define CL_CYAN_L   CL_LIGHT(CL_CYAN)
#define CL_RED_L    CL_LIGHT(CL_RED)
#define CL_VIOLET_L CL_LIGHT(CL_VIOLET)
#define CL_YELLOW_L CL_LIGHT(CL_YELLOW)
#define CL_WHITE_L  CL_LIGHT(CL_WHITE)

HANDLE ch;

#endif

ElectroincMap::ElectroincMap(QObject *parent) :
    QObject(parent),
    firstEntery(true)
{
#ifdef WIN32
    ch = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}




string getProgressbar(double val, double maxVal, QChar filler = '=', int length = 6)
{
    int activeSegmentsCount = int(0.5 + length * val / maxVal);
    QString res = "";
    for (int i = 0; i < length; i++)
    {
        if (i < activeSegmentsCount) res += filler;
        else res += ' ';
    }
    return res.toStdString();
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
    RouteSection *section = new RouteSection(sectionId);
    // Считываем столбы
    for (int i = 0; i < postsCount; i++)
    {
        KilometerPost *currentPost = KilometerPost::loadFrom(data, 9, i);
        allPosts.push_back(currentPost);
        section->posts.push_back(currentPost);

        currentPost->sectionId = sectionId;

        if (currentPost->position == kpp_end)
        {
            sections.push_back(section);
            section = new RouteSection(sectionId);
            sectionId++;
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
    printf(" lat %7.4f lon %7.4f x %4.0f", lat, lon, x);

    nearPosts = getPostsInArea(lat, lon, 10000);

    if (firstEntery)
    {
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
        targetPost = closestPost;
        firstEntery = false;
    }

    // Находим ближайшие столбы
    list<KilometerPost> nextPosts = getPostsInArea(lat, lon, 2000);     // Находим столбы на расстоянии 3х километров - казалось бы, только к ним мы можем ехать сейчас

    nextPosts.remove(departPost);       // Удаляем оттуда столб отправления
    syncPostApproaches(nextPosts);      // И синхронизируем список приближений

    // Вычисляем приближения для всех точек
    D_foreach(postApproaches, PostApproach, pait)
    {
        PostApproach &pa = *pait;
        // Добавляем текущую точку приближения
        ApproachingPoint ap;
        ap.x = x;   ap.r = pa.post.distanceTo(lat, lon);
        pa.pushApproaching(ap);
    }

    // Постараемся найти приближение для целевого поста
    PostApproach *targetPostApproach = nullptr;
    D_foreach(postApproaches, PostApproach, pait)
    {
        PostApproach &pa = *pait;
        if (pa.post == targetPost)
        {
            targetPostApproach = &pa;
        }
    }

    printf(" target: ");
    SET_COLOR(0xe,0);
    if (targetPostApproach == nullptr) printf("null");
    else printf("%5.0f", targetPost.ordinate);
    RESET_COLOR;
    printf(" appr: ");
    SET_COLOR(2,0);
    printf("%5.2f", targetPostApproach->approachingSpeed);
    RESET_COLOR;
    printf("\n");

    // Смотрим, не нужно ли соскачить со столба
    if (targetPostApproach == nullptr || (targetPostApproach->approachingSpeed < 0 && targetPostApproach->minimalApproach > 150))
    {
        targetPostApproach = findBestApproach();
    }


    // Проверяем, не достигнут ли целевой столб
    if (targetPostApproach->achived)
    {
        SET_COLOR(0xF,0);

        PostApproach &pa = *targetPostApproach;
        // Трубим
        emit onPostDetected(pa.post, pa.getX());

        qDebug() << "FIXED TO POST: " << pa.post.ordinate << "m   x: " << pa.getX();

        departPost = targetPost;

        KilometerPost *p = projectNextPost(departPost);

        if (p == nullptr) targetPost = findBestApproach()->post;
        else targetPost = *p;

        qDebug() << "NEXT: " << targetPost.ordinate << endl << endl << endl;

        SET_COLOR(7,0);
    }
}

ElectroincMap::PostApproach *ElectroincMap::findBestApproach()
{
    double targetWeigth = 1e20;
    PostApproach *targetPostApproach;

    D_foreach(postApproaches, PostApproach, pait)
    {
        PostApproach &pa = *pait;
        if (pa.approachingSpeed > 0.0)
        {
            double weigth = getPostApproachWeight(pa);

            byte tColor = 7;
            byte bColor = 0;

            printf("  ");

            if (pa.post.sectionId == 6)        { tColor = 0x2; }
            if (pa.post.sectionId == 4)        { tColor = 0x4; }
            if (pa.post.sectionId == 1)        { tColor = 0x1; }

            SET_COLOR(CL_LIGHT(tColor), bColor);

            qDebug(" post %5.0f:   %5.0f x %5.2f  >  %6.2f",
                   pa.post.ordinate,
                   pa.minimalApproach, pa.approachingSpeed,
                   weigth);

            if (weigth < targetWeigth)
            {
                targetPostApproach = &pa;
                targetPost = pa.post;
                targetWeigth = weigth;
            }
            RESET_COLOR;
        }
    }

    SET_COLOR(0xe, 0);
    printf("      CHECKED to %5.0f\n", targetPostApproach->post.ordinate);
    RESET_COLOR;

    return targetPostApproach;
}

// Рассчитывает вес для точки приближения
double ElectroincMap::getPostApproachWeight(ElectroincMap::PostApproach &pa)
{
//    double priblizhenie = pa.approachingSpeed;

//    double zaSectsiyu = pa.post == targetPost ? 1 : 0;
//    double zaDisstance = 1 - pa.minimalApproach / 1000.0;


//    return zaDisstance + (1 + zaSectsiyu) * priblizhenie;

    return pa.minimalApproach / pa.approachingSpeed;
}

// Возвращает все километровые солбы в указанном радиусе
list<KilometerPost *> ElectroincMap::getPostsInArea(double lat, double lon, double radius)
{
    return getPostsInArea(allPosts, lat, lon, radius);
}
list<KilometerPost *> ElectroincMap::getPostsInArea(vector<KilometerPost *> &source, double lat, double lon, double radius)
{
    list<KilometerPost *> res;

    double radiusEstimation = KilometerPost::metersToEstimation(radius);
    foreach (KilometerPost *p, source) {
        //if (p.estimateDistanceTo(lat, lon) <= radiusEstimation)
        if (p->distanceTo (lat, lon) <= radius)
        {
            res.push_back(p);
        }
    }
    return res;
}

void ElectroincMap::syncPostApproaches(list<KilometerPost *> posts)
{
    list<PostApproach> newApproaches;
    foreach (KilometerPost *p, posts) {
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

KilometerPost *ElectroincMap::projectNextPost(KilometerPost forPost)
{
    int direction = forPost.direction * (trackNumber % 2 == 0 ? 1 : -1);
    KilometerPost *res = nullptr;
    double shiftToRes = 1e20;
    D_foreach(nearPosts, KilometerPost *, kpi)
    {
        KilometerPost &kp = **kpi;
        if (kp.sectionId == forPost.sectionId)
        {
            double ordinateShift = direction * (kp.ordinate - forPost.ordinate);
            if (ordinateShift > 0 && ordinateShift < shiftToRes)
            {
                shiftToRes = ordinateShift;
                res = &kp;
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

    approachingSpeed = estimateApproaching();

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

// Оценивает среднюю скорость приближения к точке
double ElectroincMap::PostApproach::estimateApproaching()
{
    double sumSlope = 0;
    ApproachingPoint prewPoint;
    bool isFirstPoint = true;
    foreach (ApproachingPoint ap, aPoints) {
        if (!isFirstPoint)
        {
            double dr = ap.r - prewPoint.r;
            double dx = ap.x - prewPoint.x;
            sumSlope = sumSlope * 0.7 + (-dr/dx) * 0.3;
        }
        prewPoint = ap;
        isFirstPoint = false;
    }
    //sumSlope /= aPoints.size();
    return sumSlope;
}
