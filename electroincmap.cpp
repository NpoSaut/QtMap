#include <vector>
#include <math.h>

#include <QFile>
#include <QTextCodec>

#include "masqarade.h"

#include "electroincmap.h"

#include "masqarade.h"

#include "iodrv/emapcanemitter.h"

using namespace Navigation;
#include <QDebug>

#define D_foreach(l, T, itr) for (list<T>::iterator itr=l.begin(); itr != l.end(); ++itr)
#define D_foreach_vec(l, T, itr) for (vector<T>::iterator itr=l.begin(); itr != l.end(); ++itr)

using namespace std;

ElectroincMap::ElectroincMap(QObject *parent) :
    QObject(parent),
    firstEnter(true),
    x(0), ordinate(0),
    departPost(nullptr), targetPost(nullptr)
{ }




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

    qDebug() << "loading map...";

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
    checkOrdinate();
}

void ElectroincMap::setTrackNumber(int value)
{
    if (value != trackNumber)
        trackNumber = value;
}


void ElectroincMap::checkMap(double lat, double lon)
{
    printf(" lat %7.4f lon %7.4f  x: ", lat, lon);
    CPRINTF(CL_CYAN_L, "%4.0f", x);

    l += KilometerPost::distanceBetween (lat, lon, lat1, lon1);
    lat1 = lat; lon1 = lon;

    nearPosts = getPostsInArea(lat, lon, 10000);

    if (firstEnter)
    {
        // Находим ближайший километровый столб (за исключением "столба отправления")
        KilometerPost *closestPost = nullptr;
        double closestDist = 1e20;
        foreach (KilometerPost *p, nearPosts)
        {
            if (p != departPost)
            {
                double d = p->distanceTo(lat, lon);
                if (d < closestDist)
                {
                    closestPost = p;
                    closestDist = d;
                }
            }
        }
        targetPost = closestPost;
        departPost = projectNextPost(targetPost, true);
        departX = x - departPost->distanceTo(lat, lon);

        firstEnter = false;
    }

    // Находим ближайшие столбы
    list<KilometerPost *> nextPosts = getPostsInArea(nearPosts, lat, lon, 2000);     // Находим столбы на расстоянии 3х километров - казалось бы, только к ним мы можем ехать сейчас

    nextPosts.remove(departPost);       // Удаляем оттуда столб отправления
    syncPostApproaches(nextPosts);      // синхронизируем список приближений

    // Вычисляем приближения для всех точек
    foreach(PostApproach *pa, postApproaches)
    {
        // Добавляем текущую точку приближения
        ApproachingPoint ap(x, pa->post->distanceTo(lat, lon));
        pa->pushApproaching(ap);
    }

    // Постараемся найти приближение для целевого поста
    PostApproach *targetPostApproach = nullptr;
    foreach (PostApproach *pa, postApproaches)
        if (pa->post == targetPost)
            targetPostApproach = pa;

    printf(" target: ");
    SET_COLOR(CL_YELLOW_L, CL_BLACK);
    if (targetPostApproach == nullptr) printf("null");
    else printf("%5.0f", targetPost->ordinate);
    RESET_COLOR;
    printf(" appr: ");
    CPRINTF(CL_GREEN_L, "%5.2f", targetPostApproach->approachingSpeed);
    printf("\n");

    // Смотрим, не нужно ли соскачить со столба
    if (targetPostApproach == nullptr || (targetPostApproach->approachingSpeed < 0 && targetPostApproach->minimalApproach > 150))
    {
        targetPostApproach = findBestApproach();

        //targetPost = targetPostApproach->post;
        KilometerPost *p = projectNextPost(targetPost, true);
        if (p == nullptr) departPost = findBestApproach()->post;
        else departPost = p;
    }


    // Проверяем, не достигнут ли целевой столб
    if (targetPostApproach->achived)
    {

        // Трубим

        double l0 = abs(targetPost->ordinate - departPost->ordinate);

        if (  abs(abs(x - departX) - l0) > 200)
        {
        CPRINTF(CL_WHITE_L, "FIXED TO POST: ");
        CPRINTF(CL_RED_L, "%5.3f", targetPostApproach->post->ordinate / 1000.0);
        CPRINTF(CL_WHITE, "км\tx: ");
        CPRINTF(CL_CYAN, "%6.0f", targetPostApproach->getX());
        CPRINTF(CL_WHITE_L, "  APPROACH: ");
        CPRINTF(CL_GREEN_L, "%3.0f", targetPostApproach->minimalApproach);
        CPRINTF(CL_WHITE, "м");
        CPRINTF(CL_GRAY,    "  LENGTH ");
        CPRINTF(CL_CYAN, "%4.0f", abs(x - departX) - l0);
        CPRINTF(CL_WHITE, "м");

        CPRINTF(CL_GRAY,    "  LENGTH2 ");
        CPRINTF(CL_GREEN, "%4.0f", l - l0);
        CPRINTF(CL_GRAY, "м\n");
        }
        l = 0;

        departPost = targetPostApproach->post;
        departX    = targetPostApproach->getX();
        emit onPostDetected(*departPost, departX);

        KilometerPost *p = projectNextPost(departPost);

        if (p == nullptr) targetPost = findBestApproach()->post;
        else targetPost = p;

        CPRINTF(CL_WHITE_L, "NEXT: ");
        CPRINTF(CL_YELLOW_L, "%8.0f\n", targetPost->ordinate);

        printf("\n\n\n");
    }

    checkObjects();
}

ElectroincMap::PostApproach *ElectroincMap::findBestApproach()
{
    double targetWeigth = 1e20;
    PostApproach *targetPostApproach;

    foreach(PostApproach *pa, postApproaches)
    {
        if (pa->approachingSpeed > 0.0)
        {
            double weigth = getPostApproachWeight(pa);

            int tColor = CL_WHITE;
            int bColor = CL_BLACK;

            printf("  ");

            if (pa->post->sectionId == 6)        { tColor = CL_RED_L; }
            if (pa->post->sectionId == 4)        { tColor = CL_YELLOW_L; }
            if (pa->post->sectionId == 1)        { tColor = CL_BLUE_L; }

            SET_COLOR(tColor, bColor);

            qDebug(" post %5.0f:   %5.0f x %5.2f  >  %6.2f",
                   pa->post->ordinate,
                   pa->minimalApproach, pa->approachingSpeed,
                   weigth);

            if (weigth < targetWeigth)
            {
                targetPostApproach = pa;
                targetPost = pa->post;
                targetWeigth = weigth;
            }
            RESET_COLOR;
        }
    }

    SET_COLOR(0xe, 0);
    printf("      CHECKED to %5.0f\n", targetPostApproach->post->ordinate);
    RESET_COLOR;

    return targetPostApproach;
}


// Получает список ближайих целей
vector<EMapTarget> ElectroincMap::getNextObjects(const KilometerPost *startPost, double startPostX, int count)
{
    vector<EMapTarget> res;
    double currentPostX = startPostX;
    const KilometerPost *currentPost = startPost;

//    CPRINTF(CL_VIOLET, "  POST X: %5.0f\n", currentPostX);

    // Повторяем всё, пока не наполнится список объектов
    do
    {
        Rail *currentRail = getMyRail(currentPost);
//        CPRINTF(CL_GREEN_L, "    Проверяем КП %5.0f", currentPost->ordinate);
//        printf(" X:");
//        CPRINTF(CL_VIOLET_L, " %5.0f\n", currentPostX);
//        CPRINTF(CL_GREEN, "      Проверяем ОБЪЕКТЫ:");
        // Делаем дела для каждого объекта на текущем пути текущего километрового столба
        foreach (RailObject *o, currentRail->getObjects())
        {
            // Вычисляем координату X объекта
            int objectX = (int)(currentPostX + currentRail->direction * currentPost->direction * (o->getOrdinate() - currentPost->ordinate));
//            CPRINTF(CL_GREEN, " %5d [%4d]", o->getOrdinate(), objectX);
            if (objectX >= x)       // Добавляем объект в список только если его координата X больше текущей
            {
//                CPRINTF(CL_GREEN, "*");
                EMapTarget target(o, objectX);
                res.push_back(target);
            } // else printf(" ");
            if (res.size() >= count) break;     // Прерываем цикл, если набрали достаточное количество целей
        }
//        printf("\n");
        KilometerPost *nextPost = projectNextPost(currentPost);
        if (nextPost == nullptr) break;
        currentPostX += abs(nextPost->ordinate - currentPost->ordinate);
        currentPost = nextPost;
    } while (res.size() < count);

//    CPRINTF(CL_GREEN_L, "    Нашли %2d целей\n", res.size());
    return res;
}

Rail *ElectroincMap::getMyRail(const KilometerPost *post)
{
    if (post->rails.find(trackNumber) != post->rails.end()) return post->rails.at(trackNumber);
    else return nullptr;
}

KilometerPost *ElectroincMap::getCurrentKilometer()
{
    if (Rail::getDirectoin(trackNumber) == departPost->direction) return departPost;
    else return targetPost;
}

int ElectroincMap::getDirection(int trackNumber, KilometerPost *kilometer)
{
    return kilometer->direction * Rail::getDirectoin (trackNumber);
}

void ElectroincMap::checkOrdinate()
{
    if (departPost != nullptr)
    {
        ordinate = departPost->ordinate + myDirection() * (x - departX);
    }
}

void ElectroincMap::checkObjects()
{
    KilometerPost *currentKilometer = getCurrentKilometer ();

//    CPRINTF(CL_CYAN, "   [%5.0f --> %5.0f] by KP ", departPost->ordinate, targetPost->ordinate);
//    CPRINTF(CL_CYAN_L, "%5.0f", currentKilometer->ordinate);
//    CPRINTF(CL_RED_L, "     d = %.0f м\n", departPost->ordinate - currentKilometer->ordinate)
    vector<EMapTarget> targets = getNextObjects(currentKilometer, departX + (departPost->ordinate - currentKilometer->ordinate));
    foreach (EMapTarget t, targets) {
        CPRINTF(CL_GREEN , "     TARGET");
        CPRINTF(CL_YELLOW, "%2d", t.object->getType());
        printf(" x: ");
        CPRINTF(CL_YELLOW_L, "%6d", (unsigned short)(t.x));
        printf(" ordinate: ");
        CPRINTF(CL_CYAN, "%8d", t.object->getOrdinate());

        EMapCanEmitter::CanMessageData ima= EMapCanEmitter::encodeEMapTarget (t);
        unsigned char *msg = (unsigned char *) &ima;
        CPRINTF(CL_GRAY, "     %02x %02x %02x %02x", msg[4], msg[5], msg[6], msg[7]);

        CPRINTF(CL_VIOLET, "   %s", t.object->getName ().toStdString ().c_str ());
        printf("\n");
    }
}

// Рассчитывает вес для точки приближения
double ElectroincMap::getPostApproachWeight(const ElectroincMap::PostApproach *pa)
{
    return pa->minimalApproach / pa->approachingSpeed;
}

// Возвращает все километровые солбы в указанном радиусе
list<KilometerPost *> ElectroincMap::getPostsInArea(double lat, double lon, double radius)
{
    return getPostsInArea(allPosts, lat, lon, radius);
}
list<KilometerPost *> ElectroincMap::getPostsInArea(list<KilometerPost *> &source, double lat, double lon, double radius)
{
    list<KilometerPost *> res;

//    double radiusEstimation = KilometerPost::metersToEstimation(radius);
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
    list<PostApproach *> newApproaches;
    foreach (KilometerPost *p, posts) {
        PostApproach *pa = nullptr;
        foreach (PostApproach *ipa, postApproaches) {
            if (ipa->post == p)
            {
                pa = ipa;
                postApproaches.remove (ipa);
                break;
            }
        }
        if (pa == nullptr)
        {
            pa = new PostApproach(p);
        }
        newApproaches.push_back(pa);
    }
    foreach (PostApproach *deadPa, postApproaches) {
        delete deadPa;
    }
    postApproaches = newApproaches;
}

KilometerPost *ElectroincMap::projectNextPost(const KilometerPost *forPost, bool goBack)
{
    int direction = forPost->direction * getMyRail(forPost)->direction * (goBack ? -1 : 1);
    KilometerPost *res = nullptr;
    double shiftToRes = 1e20;
    foreach(KilometerPost *kp, nearPosts)
    {
        if (kp->sectionId == forPost->sectionId)
        {
            double ordinateShift = direction * (kp->ordinate - forPost->ordinate);
            if (ordinateShift > 0 && ordinateShift < shiftToRes)
            {
                shiftToRes = ordinateShift;
                res = kp;
            }
        }
    }

    return res;
}

vector<ElectroincMap::ApproachingPoint> ElectroincMap::PostApproach::getExtremumApproaches()
{
    vector<ElectroincMap::ApproachingPoint> res;

    ElectroincMap::ApproachingPoint prev_ap(1e20, 1e20);
    D_foreach(aPoints, ElectroincMap::ApproachingPoint, itr)
    {
        ElectroincMap::ApproachingPoint ap = *itr;
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
    ApproachingPoint prewPoint(1e20, 1e20);
    bool isFirstPoint = true;
    foreach (ApproachingPoint ap, aPoints) {
        if (!isFirstPoint)
        {
            double dr = ap.r - prewPoint.r;
            double dx = ap.x - prewPoint.x;
            if (dx != 0)
                sumSlope = sumSlope * 0.7 + (-dr/dx) * 0.3;
        }
        prewPoint = ap;
        isFirstPoint = false;
    }
    //sumSlope /= aPoints.size();
    return sumSlope;
}
