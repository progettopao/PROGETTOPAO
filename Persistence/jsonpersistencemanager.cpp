#include "jsonpersistencemanager.h"
#include "hometask.h"
#include "bill.h"
#include "vehiclemaintenance.h"
#include "shoppingtask.h"
#include "leisuretimetask.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QDate>

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
        if (activity) {
            array.append(activity->toJsonObject());
        }
    }

    return array;
}

QVector<Abstract_Activity*> JsonPersistenceManager::jsonArrayToActivityList(const QJsonArray &array) {
    QVector<Abstract_Activity*> activityList;

    for (const QJsonValue &val : array) {
        if (val.isObject()) {
            QJsonObject obj = val.toObject();

            // Leggiamo la stringa salvata nel file JSON sotto la chiave "att_type"
            QString type = obj["att_type"].toString();
            Abstract_Activity* activity = nullptr;

            // RISOLUZIONE COMPILAZIONE: Usiamo dei prototipi locali fornendo argomenti di default
            if (type == "Bill") {
                Bill proto("", "", "", false, 0.0, "", QDate::currentDate());
                activity = proto.cloneFromJson(obj);
            }
            else if (type == "HomeTask") {
                HomeTask proto("", "", "", false, "", 1);
                activity = proto.cloneFromJson(obj);
            }
            else if (type == "VehicleMaintenance") {
                VehicleMaintenance proto("", "", "", false, "", "");
                activity = proto.cloneFromJson(obj);
            }
            else if (type == "ShoppingTask") {
                ShoppingTask proto("", "", "", false, "", 0.0);
                activity = proto.cloneFromJson(obj);
            }
            else if (type == "LeisureTimeTask") {
                LeisureTimeTask proto("", "", "", false, "", 0, "");
                activity = proto.cloneFromJson(obj);
            }

            if (activity) {
                activityList.append(activity);
            }
        }
    }

    return activityList;
}
