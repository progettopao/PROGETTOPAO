#ifndef JSONPERSISTENCEMANAGER_H
#define JSONPERSISTENCEMANAGER_H

#include <QString>
#include <QVector>
#include <QJsonArray>
#include "abstract_activity.h"

class JsonPersistenceManager {
public:
    JsonPersistenceManager();

    bool saveActivities(const QString &filePath, const QVector<Abstract_Activity*> &activityList);
    QVector<Abstract_Activity*> loadActivities(const QString &filePath);

private:
    QJsonArray activityListToJsonArray(const QVector<Abstract_Activity*> &activityList);
    QVector<Abstract_Activity*> jsonArrayToActivityList(const QJsonArray &array);
};

#endif // JSONPERSISTENCEMANAGER_H
