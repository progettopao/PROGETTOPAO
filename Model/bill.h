#ifndef BILL_H
#define BILL_H

#include "abstract_activity.h"
#include <QDate>

class Bill : public Abstract_Activity {
private:
    double importo;
    QString enteErogatore;
    QDate dataScadenza;

public:
    Bill(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
         double importo, const QString& ente, const QDate& scadenza);

    // Getter e Setter
    double getImporto() const;
    void setImporto(double imp);
    QString getEnteErogatore() const;
    void setEnteErogatore(const QString& ente);
    QDate getDataScadenza() const;
    void setDataScadenza(const QDate& scadenza);

    int giorniAllaScadenza() const;

    // Override
    QString getDettagliSpecifici() const override;
    bool isUrgente() const override;
    
    QJsonObject toJsonObject() const override;
    Abstract_Activity* cloneFromJson(const QJsonObject& json) const override;
    void writeToXml(QXmlStreamWriter& writer) const override;
};

#endif // BILL_H
