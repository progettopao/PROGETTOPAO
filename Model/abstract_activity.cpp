#include "abstract_activity.h"
#include "hometask.h"
#include "bill.h"
#include "vehiclemaintenance.h"
#include "shoppingtask.h"
#include "leisuretimetask.h"

Abstract_Activity::Abstract_Activity(const QString &id, const QString &titolo, const QString &descrizione, bool completata)
    : id(id), titolo(titolo), descrizione(descrizione), completata(completata) {
}
// Getters
QString Abstract_Activity::getId() const { return id; }
QString Abstract_Activity::getTitolo() const { return titolo; }
QString Abstract_Activity::getDescrizione() const { return descrizione; }
bool Abstract_Activity::isCompletata() const { return completata; }

// Setters
void Abstract_Activity::setTitolo(const QString &titolo) { this->titolo = titolo; }
void Abstract_Activity::setDescrizione(const QString &descrizione) { this->descrizione = descrizione; }
void Abstract_Activity::setCompletata(bool stato) { this->completata = stato; }
void Abstract_Activity::completaAttivita() { this->completata = true; }

// NOTA: I metodi 'toJsonObject()', 'cloneFromJson()' e 'writeToXml()' 
// sono virtuali puri (= 0) nell'header, quindi NON devono essere implementati qui.


