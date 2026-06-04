#include "hometask.h"

HomeTask::HomeTask(const QString &id, const QString &titolo, const QString &descrizione, bool completata,
                   const QString &stanza, int urgenza)
    : Abstract_Activity(id, titolo, descrizione, completata), stanza(stanza), urgenza(urgenza) {
}

QString HomeTask::getStanza() const {
    return stanza;
}

int HomeTask::getUrgenza() const {
    return urgenza;
}

void HomeTask::setStanza(const QString &stanza) {
    this->stanza = stanza;
}

void HomeTask::setUrgenza(int urgenza) {
    this->urgenza = urgenza;
}

QString HomeTask::getDettagliSpecifici() const {
    return QString("Ambiente: %1\nGrado Urgenza: %2/5").arg(stanza).arg(urgenza);
}

bool HomeTask::isUrgente() const {
    return (urgenza >= 4) && !isCompletata();
}

QJsonObject HomeTask::toJsonObject() const {
    QJsonObject json;
    json["att_type"] = "HomeTask";
    json["id"] = getId();
    json["titolo"] = getTitolo();
    json["descrizione"] = getDescrizione();
    json["completata"] = isCompletata();
    // Campi specifici
    json["stanza"] = stanza;
    json["urgenza"] = urgenza;
    return json;
}

// Corretto l'errore di sintassi: ora il metodo è un blocco unico, valido e sicuro
Abstract_Activity* HomeTask::cloneFromJson(const QJsonObject &json) const {
    QString id = json.contains("id") ? json["id"].toString() : "";
    QString titolo = json.contains("titolo") ? json["titolo"].toString() : "";
    QString descrizione = json.contains("descrizione") ? json["descrizione"].toString() : "";
    bool completata = json.contains("completata") ? json["completata"].toBool() : false;
    QString stanza = json.contains("stanza") ? json["stanza"].toString() : "Generica";
    int urgenza = json.contains("urgenza") ? json["urgenza"].toInt() : 1;

    return new HomeTask(id, titolo, descrizione, completata, stanza, urgenza);
}

void HomeTask::writeToXml(QXmlStreamWriter &writer) const {
    writer.writeStartElement("Activity");
    writer.writeAttribute("type", "HomeTask");
    writer.writeTextElement("id", getId());
    writer.writeTextElement("titolo", getTitolo());
    writer.writeTextElement("descrizione", getDescrizione());
    writer.writeTextElement("completata", isCompletata() ? "true" : "false");
    writer.writeTextElement("stanza", stanza);
    writer.writeTextElement("urgenza", QString::number(urgenza));
    writer.writeEndElement(); // Fine Activity
}
