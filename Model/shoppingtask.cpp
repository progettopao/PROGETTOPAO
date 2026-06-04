#include "shoppingtask.h"
#include <QJsonArray>

// Costruttore AGGIORNATO: accetta e assegna la lista passata in blocco
ShoppingTask::ShoppingTask(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
                           const QString& negozio, double budget, const QStringList& lista)
    : Abstract_Activity(id, titolo, descrizione, completata),
    supermercatoConsigliato(negozio),
    budgetMassimo(budget),
    listaProdotti(lista) {}

QString ShoppingTask::getSupermercatoConsigliato() const { return supermercatoConsigliato; }
void ShoppingTask::setSupermercatoConsigliato(const QString& negozio) { supermercatoConsigliato = negozio; }

double ShoppingTask::getBudgetMassimo() const { return budgetMassimo; }
void ShoppingTask::setBudgetMassimo(double budget) { budgetMassimo = budget; }

QStringList ShoppingTask::getListaProdotti() const { return listaProdotti; }

// AGGIUNTO: implementazione del setter per consentire le modifiche da MainWindow
void ShoppingTask::setListaProdotti(const QStringList& lista) {
    listaProdotti = lista;
}

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
    return listaProdotti.size() >= 5;
}

QJsonObject ShoppingTask::toJsonObject() const {
    QJsonObject json;
    json["att_type"] = "ShoppingTask";
    json["id"] = getId();
    json["titolo"] = getTitolo();
    json["descrizione"] = getDescrizione();
    json["completata"] = isCompletata();
    json["supermercatoConsigliato"] = supermercatoConsigliato;
    json["budgetMassimo"] = budgetMassimo;

    QJsonArray arrayProd;
    for (const QString& p : listaProdotti) {
        arrayProd.append(p);
    }
    json["listaProdotti"] = arrayProd;
    return json;
}

Abstract_Activity* ShoppingTask::cloneFromJson(const QJsonObject& json) const {
    // Lasciamo inalterato: l'assenza del settimo parametro userà automaticamente la lista vuota di default
    ShoppingTask* newTask = new ShoppingTask(
        json["id"].toString(),
        json["titolo"].toString(),
        json["descrizione"].toString(),
        json["completata"].toBool(),
        json["supermercatoConsigliato"].toString(),
        json["budgetMassimo"].toDouble()
        );

    if (json.contains("listaProdotti") && json["listaProdotti"].isArray()) {
        QJsonArray arrayProd = json["listaProdotti"].toArray();
        for (int i = 0; i < arrayProd.size(); ++i) {
            newTask->aggiungiArticolo(arrayProd[i].toString());
        }
    }

    return newTask;
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
    writer.writeEndElement();
    writer.writeEndElement();
}
