#include "xmlpersistencemanager.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QDate>

#include "hometask.h"
#include "bill.h"
#include "vehiclemaintenance.h"
#include "shoppingtask.h"
#include "leisuretimetask.h"

XmlPersistenceManager::XmlPersistenceManager() {}

// SALVATAGGIO
bool XmlPersistenceManager::saveActivities(const QString &filePath, const QVector<Abstract_Activity*> &activityList) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true); // Rende l'XML leggibile a occhio umano (va a capo e indenta)
    writer.writeStartDocument();

    writer.writeStartElement("Activities"); // Tag radice del documento

    // Sfruttiamo il polimorfismo: ogni attività sa come scriversi nel flusso
    for (const Abstract_Activity *activity : activityList) {
        if (activity) {
            activity->writeToXml(writer);
        }
    }

    writer.writeEndElement(); // Chiude "Activities"
    writer.writeEndDocument();
    file.close();

    return true;
}

// CARICAMENTO
QVector<Abstract_Activity*> XmlPersistenceManager::loadActivities(const QString &filePath) {
    QVector<Abstract_Activity*> activityList;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return activityList;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement && xml.name().toString() == "Activity"){
            Abstract_Activity* activity = parseActivity(xml);
            if (activity) {
                activityList.append(activity);
            }
        }
    }

    if (xml.hasError()) {
        qDebug() << "Errore nel parsing XML:" << xml.errorString();
    }

    file.close();
    return activityList;
}

//HELPER DI PARSING
Abstract_Activity* XmlPersistenceManager::parseActivity(QXmlStreamReader &xml) {
    QString type = xml.attributes().value("type").toString();
    QString id, titolo, descrizione, stanza, targa, officina, ente, categoria, compagnia, supermercatoConsigliato;
    bool completata = false;
    int urgenza = 1, kmScadenza = 0, relax = 1;
    double importo = 0.0, budgetMassimo = 0.0;
    QDate scadenza = QDate::currentDate();
    QStringList componenti, prodotti;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name().toString() == "Activity")) {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            QString tagName = xml.name().toString();

            // Campi comuni della classe base
            if (tagName == "id") id = xml.readElementText();
            else if (tagName == "titolo") titolo = xml.readElementText();
            else if (tagName == "descrizione") descrizione = xml.readElementText();
            else if (tagName == "completata") completata = (xml.readElementText() == "true");

            // Campi specifici di HomeTask
            else if (tagName == "stanza") stanza = xml.readElementText();
            else if (tagName == "urgenza") urgenza = xml.readElementText().toInt();

            // Campi specifici di Bill
            else if (tagName == "importo") importo = xml.readElementText().toDouble();
            else if (tagName == "enteErogatore") ente = xml.readElementText();
            else if (tagName == "dataScadenza") scadenza = QDate::fromString(xml.readElementText(), Qt::ISODate);

            // Campi specifici di VehicleMaintenance
            else if (tagName == "targaVeicolo") targa = xml.readElementText();
            else if (tagName == "chilometraggioScadenza") kmScadenza = xml.readElementText().toInt();
            else if (tagName == "officinaRiferimento") officina = xml.readElementText();
            else if (tagName == "Componente") componenti.append(xml.readElementText());

            // Campi specifici di ShoppingTask
            else if (tagName == "supermercatoConsigliato") supermercatoConsigliato = xml.readElementText();
            else if (tagName == "budgetMassimo") budgetMassimo = xml.readElementText().toDouble();
            else if (tagName == "Prodotto") prodotti.append(xml.readElementText());

            // Campi specifici di LeisureTimeTask
            else if (tagName == "categoriaSvago") categoria = xml.readElementText();
            else if (tagName == "livelloRelax") relax = xml.readElementText().toInt();
            else if (tagName == "compagnia") compagnia = xml.readElementText();
        }
    }

    if (type == "HomeTask") {
        return new HomeTask(id, titolo, descrizione, completata, stanza, urgenza);
    }
    else if (type == "Bill") {
        return new Bill(id, titolo, descrizione, completata, importo, ente, scadenza);
    }
    else if (type == "VehicleMaintenance") {
        VehicleMaintenance* vm = new VehicleMaintenance(id, titolo, descrizione, completata, targa, kmScadenza, officina);
        for (const QString& c : componenti) {
            vm->aggiungiComponente(c); //
        }
        return vm;
    }
    else if (type == "ShoppingTask") {
        ShoppingTask* st = new ShoppingTask(id, titolo, descrizione, completata, supermercatoConsigliato, budgetMassimo);
        for (const QString& p : prodotti) {
            st->aggiungiArticolo(p);
        }
        return st;
    }
    else if (type == "LeisureTimeTask") {
        return new LeisureTimeTask(id, titolo, descrizione, completata, categoria, relax, compagnia);
    }

    return nullptr;
}
