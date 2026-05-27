#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <QVector>
#include <QString>
#include "abstract_activity.h"
#include "jsonpersistencemanager.h"
#include "xmlpersistencemanager.h"

class ActivityManager {
private:
    QVector<Abstract_Activity*> activities; // Il contenitore polimorfo principale
    JsonPersistenceManager jsonManager;     // Manager per il salvataggio JSON
    XmlPersistenceManager xmlManager;       // Manager per il salvataggio XML

    void clearActivities();

public:
    ActivityManager();
    ~ActivityManager();

    // CRUD e gestione della collezione
    void addActivity(Abstract_Activity* activity);
    bool removeActivity(const QString& id);
    QVector<Abstract_Activity*> getActivities() const;
    Abstract_Activity* getActivityById(const QString& id) const;

    // Filtri polimorfi per la GUI
    QVector<Abstract_Activity*> getUrgentActivities() const;

    template <typename T>
    QVector<Abstract_Activity*> getActivitiesByType() const {
        QVector<Abstract_Activity*> filteredList;
        for (Abstract_Activity* activity : activities) {
            if (activity && dynamic_cast<T*>(activity)) {
                filteredList.append(activity);
            }
        }
        return filteredList;
    }

    // Persistenza Dati
    bool saveToFile(const QString& filePath);
    bool loadFromFile(const QString& filePath);
};

#endif // ACTIVITYMANAGER_H
