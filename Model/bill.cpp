#include "bill.h"

Bill::Bill(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
           double importo, const QString& ente, const QDate& scadenza)
    : Abstract_Activity(id, titolo, descrizione, completata), importo(importo), enteErogatore(ente), dataScadenza(scadenza) {}

double Bill::getImporto() const { return importo; }
void Bill::setImporto(double imp) { importo = imp; }

QString Bill::getEnteErogatore() const { return enteErogatore; }
void Bill::setEnteErogatore(const QString& ente) { enteErogatore = ente; }

QDate Bill::getDataScadenza() const { return dataScadenza; }
void Bill::setDataScadenza(const QDate& scadenza) { dataScadenza = scadenza; }

int Bill::giorniAllaScadenza() const {
    return QDate::currentDate().daysTo(dataScadenza);
}

QString Bill::getDettagliSpecifici() const {
    int giorni = giorniAllaScadenza();
    QString statoScadenza = (giorni < 0) ? QString("SCADUTA da %1 giorni").arg(qAbs(giorni)): QString("Mancano %1 giorni alla scadenza").arg(giorni);
    return QString("Importo: %1 €\nEnte: %2\nScadenza: %3 (%4)").arg(QString::number(importo, 'f', 2),enteErogatore,dataScadenza.toString("dd/MM/yyyy"),
             statoScadenza);
}


bool Bill::isUrgente() const {
    return (giorniAllaScadenza() <= 3) && !isCompletata();
}

QJsonObject Bill::toJsonObject() const {
    QJsonObject json = Abstract_Activity::toJsonObject();
    json["tipo"] = "Bill";
    json["importo"] = importo;
    json["enteErogatore"] = enteErogatore;
    json["dataScadenza"] = dataScadenza.toString(Qt::ISODate);
    return json;
}

void Bill::fromJsonObject(const QJsonObject& json) {
    Abstract_Activity::fromJsonObject(json);
    if (json.contains("importo") && json["importo"].isDouble()) importo = json["importo"].toDouble();
    if (json.contains("enteErogatore") && json["enteErogatore"].isString()) enteErogatore = json["enteErogatore"].toString();
    if (json.contains("dataScadenza") && json["dataScadenza"].isString()) {
        dataScadenza = QDate::fromString(json["dataScadenza"].toString(), Qt::ISODate);
    }
}

void Bill::writeToXml(QXmlStreamWriter& writer) const {
    writer.writeStartElement("Activity");
    writer.writeAttribute("type", "Bill");
    writer.writeTextElement("id", getId());
    writer.writeTextElement("titolo", getTitolo());
    writer.writeTextElement("descrizione", getDescrizione());
    writer.writeTextElement("completata", isCompletata() ? "true" : "false");
    writer.writeTextElement("importo", QString::number(importo, 'f', 2));
    writer.writeTextElement("enteErogatore", enteErogatore);
    writer.writeTextElement("dataScadenza", dataScadenza.toString(Qt::ISODate));
    writer.writeEndElement();
}
