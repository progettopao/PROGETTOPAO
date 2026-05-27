#include "jsonpersistencemanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>

JsonPersistenceManager::JsonPersistenceManager() {
}

bool JsonPersistenceManager::saveActivities(const QString &filePath, const QVector<Abstract_Activity*> &activityList) {
    QJsonObject root;
    QJsonArray activityArray = activityListToJsonArray(activityList);
    root["activities"] = activityArray;

    QJsonDocument doc(root);
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << doc.toJson();
    file.close();

    return true;
}

QVector<Abstract_Activity*> JsonPersistenceManager::loadActivities(const QString &filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QVector<Abstract_Activity*>();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        return QVector<Abstract_Activity*>();
    }

    QJsonObject root = doc.object();
    QJsonArray activityArray = root["activities"].toArray();

    return jsonArrayToActivityList(activityArray);
}

QJsonArray JsonPersistenceManager::activityListToJsonArray(const QVector<Abstract_Activity*> &activityList) {
    QJsonArray array;

    for (const Abstract_Activity *activity : activityList) {
        array.append(activity->toJsonObject());
    }

    return array;
}

QVector<Abstract_Activity*> JsonPersistenceManager::jsonArrayToActivityList(const QJsonArray &array) {
    QVector<Abstract_Activity*> activityList;

    for (const QJsonValue &val : array) {
        if (val.isObject()) {
            Abstract_Activity *activity = Abstract_Activity::fromJson(val.toObject());
            if (activity) {
                activityList.append(activity);
            }
        }
    }

    return activityList;
}
