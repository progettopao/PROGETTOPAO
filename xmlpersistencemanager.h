#ifndef XMLPERSISTENCEMANAGER_H
#define XMLPERSISTENCEMANAGER_H

#include <QString>
#include <QVector>
#include <QXmlStreamReader>
#include "abstract_activity.h"

class XmlPersistenceManager {
public:
    XmlPersistenceManager();

    // Metodi principali analoghi a quelli JSON
    bool saveActivities(const QString &filePath, const QVector<Abstract_Activity*> &activityList);
    QVector<Abstract_Activity*> loadActivities(const QString &filePath);

private:
    Abstract_Activity* parseActivity(QXmlStreamReader &xml);
};

#endif
