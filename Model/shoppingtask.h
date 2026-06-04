#ifndef SHOPPINGTASK_H
#define SHOPPINGTASK_H

#include "abstract_activity.h"
#include <QStringList>

class ShoppingTask : public Abstract_Activity {
private:
    QString supermercatoConsigliato;
    double budgetMassimo;
    QStringList listaProdotti;

public:
    // Costruttore AGGIORNATO: l'ultimo parametro ora è opzionale per non rompere cloneFromJson
    ShoppingTask(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
                 const QString& negozio, double budget, const QStringList& lista = QStringList());

    // Getter e Setter
    QString getSupermercatoConsigliato() const;
    void setSupermercatoConsigliato(const QString& negozio);
    double getBudgetMassimo() const;
    void setBudgetMassimo(double budget);
    QStringList getListaProdotti() const;
    void setListaProdotti(const QStringList& lista); // AGGIUNTO: richiesto da MainWindow

    void aggiungiArticolo(const QString& articolo);
    void spuntaArticolo(const QString& articolo);

    // Override Polimorfismo
    QString getDettagliSpecifici() const override;
    bool isUrgente() const override;
    void completaAttivita();

    // Override Persistenza
    QJsonObject toJsonObject() const override;
    Abstract_Activity* cloneFromJson(const QJsonObject& json) const override;
    void writeToXml(QXmlStreamWriter& writer) const override;
};

#endif // SHOPPINGTASK_H
