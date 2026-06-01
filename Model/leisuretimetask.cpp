#include "leisuretimetask.h"

LeisureTimeTask::LeisureTimeTask(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
                                 const QString& categoria, int relax, const QString& compagnia)
    : Abstract_Activity(id, titolo, descrizione, completata), categoriaSvago(categoria), livelloRelax(relax), compagnia(compagnia) {}

QString LeisureTimeTask::getCategoriaSvago() const { return categoriaSvago; }
void LeisureTimeTask::setCategoriaSvago(const QString& c) { categoriaSvago = c; }

int LeisureTimeTask::getLivelloRelax() const { return livelloRelax; }
void LeisureTimeTask::setLivelloRelax(int r) { livelloRelax = r; }

QString LeisureTimeTask::getCompagnia() const { return compagnia; }
void LeisureTimeTask::setCompagnia(const QString& comp) { compagnia = comp; }

bool LeisureTimeTask::richiedeCompagnia() const {
    return !compagnia.isEmpty() && compagnia.toLower() != "da solo" && compagnia.toLower() != "da sola";
}

QString LeisureTimeTask::getDettagliSpecifici() const {
    QString social = richiedeCompagnia() ? QString("In compagnia di: %1").arg(compagnia) : "Attività solitaria";
    return QString("Categoria: %1\nIndice Rigenerazione/Relax: %2/5\nOrganizzazione: %3")
        .arg(categoriaSvago).arg(livelloRelax).arg(social);
}

bool LeisureTimeTask::isUrgente() const {
    if (isCompletata()) {
        return false;
    }
    return livelloRelax >= 5;
}

QJsonObject LeisureTimeTask::toJsonObject() const {
    QJsonObject json;
    json["att_type"] = "LeisureTimeTask";
    json["id"] = getId();
    json["titolo"] = getTitolo();
    json["descrizione"] = getDescrizione();
    json["completata"] = isCompletata();
    // Campi specifici
    json["categoriaSvago"] = categoriaSvago;
    json["livelloRelax"] = livelloRelax;
    json["compagnia"] = compagnia;
    return json;
}

Abstract_Activity* LeisureTimeTask::cloneFromJson(const QJsonObject& json) const {
    return new LeisureTimeTask(
        json["id"].toString(),
        json["titolo"].toString(),
        json["descrizione"].toString(),
        json["completata"].toBool(),
        json["categoriaSvago"].toString(),
        json["livelloRelax"].toInt(),
        json["compagnia"].toString()
    );
}

void LeisureTimeTask::writeToXml(QXmlStreamWriter& writer) const {
    writer.writeStartElement("Activity");
    writer.writeAttribute("type", "LeisureTimeTask");
    writer.writeTextElement("id", getId());
    writer.writeTextElement("titolo", getTitolo());
    writer.writeTextElement("descrizione", getDescrizione());
    writer.writeTextElement("completata", isCompletata() ? "true" : "false");
    writer.writeTextElement("categoriaSvago", categoriaSvago);
    writer.writeTextElement("livelloRelax", QString::number(livelloRelax));
    writer.writeTextElement("compagnia", compagnia);
    writer.writeEndElement();
}
