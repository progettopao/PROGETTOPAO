#ifndef LEISURETIMETASK_H
#define LEISURETIMETASK_H

#include "abstract_activity.h"

class LeisureTimeTask : public Abstract_Activity {
private:
    QString categoriaSvago;
    int livelloRelax;
    QString compagnia;

public:
    LeisureTimeTask(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
                    const QString& categoria, int relax, const QString& compagnia);

    // Getter e Setter
    QString getCategoriaSvago() const;
    void setCategoriaSvago(const QString& c);
    int getLivelloRelax() const;
    void setLivelloRelax(int r);
    QString getCompagnia() const;
    void setCompagnia(const QString& comp);

    bool richiedeCompagnia() const;

    // Override dei metodi polimorfi della classe base
    QString getDettagliSpecifici() const override;
    bool isUrgente() const override;

    // Override per la persistenza dati
    QJsonObject toJsonObject() const override;
    void fromJsonObject(const QJsonObject& json) override;
    void writeToXml(QXmlStreamWriter& writer) const override;
};

#endif // LEISURETIMETASK_H
