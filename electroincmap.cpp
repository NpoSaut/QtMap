#include "electroincmap.h"
#include "emap/kilometerpost.h"
#include "emap/routesection.h"

#include "vector"

using namespace std;

ElectroincMap::ElectroincMap(QObject *parent) :
    QObject(parent)
{
}



// Загрузка электронной карты из файла
void ElectroincMap::load(QString fileName)
{
    vector<char> data;

    // Количество столбок
    int postsCount = data[2] | data[3] << 8;

    sections.push_back(*(new RouteSection()));
    // Считываем столбы
    for (int i = 0; i < postsCount; i++)
    {
        KilometerPost currentPost = KilometerPost::loadFrom(data, 9, i);
        allPosts.push_back(currentPost);
        if (currentPost.position == kpp_start)
        {
            sections.push_back(*(new RouteSection()));
        }
        sections.end()->Posts.push_back(currentPost);
    }
}

void ElectroincMap::setMetrometer(double value)
{
    if (value != x)
        x = value;
}

void ElectroincMap::checkMap(double lat, double lon)
{

}

// Возвращает все километровые солбы в указанном радиусе
vector<KilometerPost> ElectroincMap::getPostsInArea(double radius)
{
    vector<KilometerPost> res;
    for (int i = 0; i < allPosts.size(); i++)
    {
        // Фильтр сюда!
    }
    return res;
}
