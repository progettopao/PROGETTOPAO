#include "vehiclemaintenance.h"
#include <QJsonArray>

// Costruttore AGGIORNATO: ora inizializza anche componentiDaSostituire direttamente se passata
VehicleMaintenance::VehicleMaintenance(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
                                       const QString& targa, const QString& officina, const QStringList& componenti)
    : Abstract_Activity(id, titolo, descrizione, completata), targaVeicolo(targa), officinaRiferimento(officina), componentiDaSostituire(componenti) {}

QString VehicleMaintenance::getTargaVeicolo() const { return targaVeicolo; }
void VehicleMaintenance::setTargaVeicolo(const QString& targa) { targaVeicolo = targa; }

QString VehicleMaintenance::getOfficinaRiferimento() const { return officinaRiferimento; }
void VehicleMaintenance::setOfficinaRiferimento(const QString& officina) { officinaRiferimento = officina; }

QStringList VehicleMaintenance::getComponentiDaSostituire() const { return componentiDaSostituire; }

// AGGIUNTO: implementazione del setter per la MainWindow
void VehicleMaintenance::setComponentiDaSostituire(const QStringList& componenti) {
    componentiDaSostituire = componenti;
}

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
    return QString("Veicolo (Targa/Modello): %1\nIntervento ai: %2 KM\nOfficina: %3\nComponenti da verificare: %4").arg(targaVeicolo, officinaRiferimento, listaComp);
}

bool VehicleMaintenance::isUrgente() const {
    if (isCompletata()) {
        return false;
    }
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
    json["att_type"] = "VehicleMaintenance";
    json["id"] = getId();
    json["titolo"] = getTitolo();
    json["descrizione"] = getDescrizione();
    json["completata"] = isCompletata();
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
    // Lasciamo tutto invariato, funziona perfettamente perché l'ultimo parametro ora è opzionale!
    VehicleMaintenance* newMaint = new VehicleMaintenance(
        json["id"].toString(),
        json["titolo"].toString(),
        json["descrizione"].toString(),
        json["completata"].toBool(),
        json["targaVeicolo"].toString(),
        json["officinaRiferimento"].toString()
        );

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
