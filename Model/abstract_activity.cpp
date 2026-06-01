#include "abstract_activity.h"
#include "hometask.h"
#include "bill.h"
#include "vehiclemaintenance.h"
#include "shoppingtask.h"
#include "leisuretimetask.h"

Abstract_Activity::Abstract_Activity(const QString &id, const QString &titolo, const QString &descrizione, bool completata)
    : id(id), titolo(titolo), descrizione(descrizione), completata(completata) {
}
// Getters
QString Abstract_Activity::getId() const { return id; }
QString Abstract_Activity::getTitolo() const { return titolo; }
QString Abstract_Activity::getDescrizione() const { return descrizione; }
bool Abstract_Activity::isCompletata() const { return completata; }

// Setters
void Abstract_Activity::setTitolo(const QString &titolo) { this->titolo = titolo; }
void Abstract_Activity::setDescrizione(const QString &descrizione) { this->descrizione = descrizione; }
void Abstract_Activity::setCompletata(bool stato) { this->completata = stato; }
void Abstract_Activity::completaAttivita() { this->completata = true; }

//Persistenza

QJsonObject Abstract_Activity::toJsonObject() const {
    QJsonObject json;
    json["tipo"] = getTipoClasse();
    json["id"] = id;
    json["titolo"] = titolo;
    json["descrizione"] = descrizione;
    json["completata"] = completata;
    return json;
}

// Factory di Caricamento
Abstract_Activity* Abstract_Activity::fromJson(const QJsonObject &json) {
    if (!json.contains("tipo") || !json["tipo"].isString()) {
        return nullptr;
    }

    QString type = json["tipo"].toString();

    // Delega la creazione e il riempimento dei dati specifici alle classi figlie
    if (type == "HomeTask") {
        return HomeTask::fromJson(json);
    } else if (type == "Bill") {
        return Bill::fromJson(json);
    } else if (type == "VehicleMaintenance") {
        return VehicleMaintenance::fromJson(json);
    } else if (type == "ShoppingTask") {
        return ShoppingTask::fromJson(json);
    } else if (type == "LeisureTimeTask") {
        return LeisureTimeTask::fromJson(json); 
    }

    return nullptr;
}
