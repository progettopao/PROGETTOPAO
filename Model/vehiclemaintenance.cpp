#include "vehiclemaintenance.h"
#include <QJsonArray>

VehicleMaintenance::VehicleMaintenance(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
                                       const QString& targa, const QString& officina)
    : Abstract_Activity(id, titolo, descrizione, completata), targaVeicolo(targa), officinaRiferimento(officina) {}

QString VehicleMaintenance::getTargaVeicolo() const { return targaVeicolo; }
void VehicleMaintenance::setTargaVeicolo(const QString& targa) { targaVeicolo = targa; }

QString VehicleMaintenance::getOfficinaRiferimento() const { return officinaRiferimento; }
void VehicleMaintenance::setOfficinaRiferimento(const QString& officina) { officinaRiferimento = officina; }

QStringList VehicleMaintenance::getComponentiDaSostituire() const { return componentiDaSostituire; }

void VehicleMaintenance::aggiungiComponente(const QString& componente) {
    if (!componentiDaSostituire.contains(componente)) {
        componentiDaSostituire.append(componente);
    }
}

void VehicleMaintenance::rimuoviComponente(const QString& componente) {
    componentiDaSostituire.removeAll(componente);
}

QString VehicleMaintenance::getDettagliSpecifici() const {
    QString listaComp = componentiDaSostituire.isEmpty() ? "Nessuno" : componentiDaSostituire.join(", ");
    return QString("Veicolo (Targa/Modello): %1\nIntervento ai: %2 KM\nOfficina: %3\nComponenti da verificare: %4").arg(targaVeicolo,
             QString::number(chilometraggioScadenza), officinaRiferimento, listaComp);
}

bool VehicleMaintenance::isUrgente() const {
    if (isCompletata()) {
        return false;
    }
    // Se ci sono componenti legati alla sicurezza, l'attività diventa urgente automaticamente
    for (const QString& componente : componentiDaSostituire) {
        QString compLower = componente.toLower();
        if (compLower.contains("fren") || compLower.contains("cinghi") || compLower.contains("motor") || compLower.contains("gomp")) {
            return true;
        }
    }

    return false;
}

QJsonObject VehicleMaintenance::toJsonObject() const {
    QJsonObject json;
    json["att_type"] = "VehicleMaintenance"; // Tag per il registro dei prototipi
    json["id"] = getId();
    json["titolo"] = getTitolo();
    json["descrizione"] = getDescrizione();
    json["completata"] = isCompletata();
    // Campi specifici
    json["targaVeicolo"] = targaVeicolo;
    json["officinaRiferimento"] = officinaRiferimento;

    QJsonArray arrayComp;
    for (const QString& c : componentiDaSostituire) {
        arrayComp.append(c);
    }
    json["componentiDaSostituire"] = arrayComp;
    return json;
}
Abstract_Activity* VehicleMaintenance::cloneFromJson(const QJsonObject& json) const {
    VehicleMaintenance* newMaint = new VehicleMaintenance(
        json["id"].toString(),
        json["titolo"].toString(),
        json["descrizione"].toString(),
        json["completata"].toBool(),
        json["targaVeicolo"].toString(),
        json["officinaRiferimento"].toString()
    );

    // Ripristino della QStringList interna di componenti
    if (json.contains("componentiDaSostituire") && json["componentiDaSostituire"].isArray()) {
        QJsonArray arrayComp = json["componentiDaSostituire"].toArray();
        for (int i = 0; i < arrayComp.size(); ++i) {
            newMaint->aggiungiComponente(arrayComp[i].toString());
        }
    }

    return newMaint;
}

void VehicleMaintenance::writeToXml(QXmlStreamWriter& writer) const {
    writer.writeStartElement("Activity");
    writer.writeAttribute("type", "VehicleMaintenance");
    writer.writeTextElement("id", getId());
    writer.writeTextElement("titolo", getTitolo());
    writer.writeTextElement("descrizione", getDescrizione());
    writer.writeTextElement("completata", isCompletata() ? "true" : "false");
    writer.writeTextElement("targaVeicolo", targaVeicolo);
    writer.writeTextElement("officinaRiferimento", officinaRiferimento);

    writer.writeStartElement("Componenti");
    for (const QString& c : componentiDaSostituire) {
        writer.writeTextElement("Componente", c);
    }
    writer.writeEndElement();
    writer.writeEndElement();
}
