#include "activitymanager.h"
#include <QFileInfo>

// Includiamo le classi figlie nel .cpp per far funzionare il dynamic_cast nella GUI se serve
#include "hometask.h"
#include "bill.h"
#include "vehiclemaintenance.h"
#include "shoppingtask.h"
#include "leisuretimetask.h"

ActivityManager::ActivityManager() {}

ActivityManager::~ActivityManager() {
    clearActivities();
}

void ActivityManager::clearActivities() {
    for (Abstract_Activity* activity : activities) {
        delete activity;
    }
    activities.clear();
}

void ActivityManager::addActivity(Abstract_Activity* activity) {
    if (activity) {
        activities.append(activity);
    }
}

bool ActivityManager::removeActivity(const QString& id) {
    for (int i = 0; i < activities.size(); ++i) {
        if (activities[i]->getId() == id) {
            delete activities[i];
            activities.removeAt(i);
            return true;
        }
    }
    return false;
}

QVector<Abstract_Activity*> ActivityManager::getActivities() const {
    return activities;
}

Abstract_Activity* ActivityManager::getActivityById(const QString& id) const {
    for (Abstract_Activity* activity : activities) {
        if (activity->getId() == id) {
            return activity;
        }
    }
    return nullptr;
}

// FILTRO POLIMORFO PURO (Vincolo 8)
QVector<Abstract_Activity*> ActivityManager::getUrgentActivities() const {
    QVector<Abstract_Activity*> urgentList;
    for (Abstract_Activity* activity : activities) {
        if (activity && activity->isUrgente()) {
            urgentList.append(activity);
        }
    }
    return urgentList;
}

// PERSISTENZA INTELLIGENTE
bool ActivityManager::saveToFile(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();

    if (ext == "json") {
        return jsonManager.saveActivities(filePath, activities);
    }
    else if (ext == "xml") {
        return xmlManager.saveActivities(filePath, activities);
    }
    return false;
}

bool ActivityManager::loadFromFile(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();
    QVector<Abstract_Activity*> loadedActivities;

    if (ext == "json") {
        loadedActivities = jsonManager.loadActivities(filePath);
    }
    else if (ext == "xml") {
        loadedActivities = xmlManager.loadActivities(filePath);
    }
    else {
        return false;
    }

    // ROBUSTEZZA: Aggiorna la memoria SOLO se il file conteneva effettivamente qualcosa o era un file valido vuoto
    // Evita di piallare i dati utente se si seleziona un file corrotto.
    if (loadedActivities.isEmpty() && !QFile::exists(filePath)) {
        return false;
    }

    clearActivities();
    activities = loadedActivities;
    return true;
}
