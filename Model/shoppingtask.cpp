#include "shoppingtask.h"
#include <QJsonArray>

ShoppingTask::ShoppingTask(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
                           const QString& negozio, double budget)
    : Abstract_Activity(id, titolo, descrizione, completata), supermercatoConsigliato(negozio), budgetMassimo(budget) {}

QString ShoppingTask::getSupermercatoConsigliato() const { return supermercatoConsigliato; }
void ShoppingTask::setSupermercatoConsigliato(const QString& negozio) { supermercatoConsigliato = negozio; }

double ShoppingTask::getBudgetMassimo() const { return budgetMassimo; }
void ShoppingTask::setBudgetMassimo(double budget) { budgetMassimo = budget; }

QStringList ShoppingTask::getListaProdotti() const { return listaProdotti; }

void ShoppingTask::aggiungiArticolo(const QString& articolo) {
    if (!articolo.isEmpty() && !listaProdotti.contains(articolo)) {
        listaProdotti.append(articolo);
    }
}

void ShoppingTask::spuntaArticolo(const QString& articolo) {
    listaProdotti.removeAll(articolo);
}

void ShoppingTask::completaAttivita() {
    if (listaProdotti.isEmpty()) {
        Abstract_Activity::completaAttivita();
    }
}

QString ShoppingTask::getDettagliSpecifici() const {
    QString articoli = listaProdotti.isEmpty() ? "Nessun articolo rimasto" : listaProdotti.join(", ");
    return QString("Negozio: %1\nBudget Fissato: %2 €\nArticoli da Acquistare (%3):\n%4")
        .arg(supermercatoConsigliato).arg(budgetMassimo, 0, 'f', 2).arg(listaProdotti.size()).arg(articoli);
}

bool ShoppingTask::isUrgente() const {
    if (isCompletata()) {
        return false;
    }
    // Diventa urgente se ci sono più di 5 prodotti da comprare
    return listaProdotti.size() >= 5;
}

QJsonObject ShoppingTask::toJsonObject() const {
    QJsonObject json = Abstract_Activity::toJsonObject();
    json["tipo"] = "ShoppingTask";
    json["supermercatoConsigliato"] = supermercatoConsigliato;
    json["budgetMassimo"] = budgetMassimo;

    QJsonArray arrayProd;
    for (const QString& p : listaProdotti) {
        arrayProd.append(p);
    }
    json["listaProdotti"] = arrayProd;
    return json;
}

void ShoppingTask::fromJsonObject(const QJsonObject& json) {
    Abstract_Activity::fromJsonObject(json);
    if (json.contains("supermercatoConsigliato") && json["supermercatoConsigliato"].isString()) supermercatoConsigliato = json["supermercatoConsigliato"].toString();
    if (json.contains("budgetMassimo") && json["budgetMassimo"].isDouble()) budgetMassimo = json["budgetMassimo"].toDouble();

    listaProdotti.clear();
    if (json.contains("listaProdotti") && json["listaProdotti"].isArray()) {
        QJsonArray arrayProd = json["listaProdotti"].toArray();
        for (int i = 0; i < arrayProd.size(); ++i) {
            listaProdotti.append(arrayProd[i].toString());
        }
    }
}

void ShoppingTask::writeToXml(QXmlStreamWriter& writer) const {
    writer.writeStartElement("Activity");
    writer.writeAttribute("type", "ShoppingTask");
    writer.writeTextElement("id", getId());
    writer.writeTextElement("titolo", getTitolo());
    writer.writeTextElement("descrizione", getDescrizione());
    writer.writeTextElement("completata", isCompletata() ? "true" : "false");
    writer.writeTextElement("supermercatoConsigliato", supermercatoConsigliato);
    writer.writeTextElement("budgetMassimo", QString::number(budgetMassimo, 'f', 2));

    writer.writeStartElement("Prodotti");
    for (const QString& p : listaProdotti) {
        writer.writeTextElement("Prodotto", p);
    }
    writer.writeEndElement(); // Fine Prodotti
    writer.writeEndElement(); // Fine Activity
}
