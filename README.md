
#  CLIENT WEB. COMUNICATIE CU REST API

# REGISTER

- Am utilizat biblioteca de parsare JSON, deoarece este mai simplu de
inteles si de scris. De asemenea, pentru afisarea informatiile despre
carti, acest format ajuta la urmarirea usoara a campurilor.
- Pentru inregistrarea unui nou utilizator, am citit de la tastatura
un username si o parola. In functia auxiliara, "function_register",
am creat un obiect JSON cu cele doua caracteristici ale unui utilizator.
- Am utilizat functia "compute_post_request", prin intermediul careia am creat
o cerere HTTP de tip POST. Aceasta cerere este trimisa catre server si are ca
scop adaugarea de resurse. Se seteaza headerele necesare si payload-ul.
- Dupa trimiterea cererii catre server, se interogheaza raspunsul primit.
In cazul in care raspunsul primit de la server contine cuvantul "error",
inseamna ca utilizatorul nu a putut fi inregistrat. Utilizatorul mai fusese
inregistrat inainte, astfel vom afisa un mesaj de EROARE.
- Daca raspunsul primit de la server contine  "201 Created" sau "200 OK",
inseamna ca utilizatorul dorit a putut fi inregistrat si afisam un mesaj de
SUCCES.

# LOGIN

- Pentru a realiza autentificarea unui utilizator, avem nevoie de username si
de parola pe care le vom citi de la tastatura. Vom crea obiectul JSON si cu
ajutorul functiei "compute_post_request" vom crea o cerere HTTP de tip POST
pe care o trimitem catre server. Vom analiza raspunsul primit. Pot aparea
doua mesaje de eroare, unul pentru credentiale: daca username-ul sau parola
au fost introduse gresit, "Credentials are not good!" si unul pentru cazul in
care se doreste autentificarea unui utilizator care nu a fost inregistrat
inainte: "No account with this username!".
- Daca autentificarea a avut loc cu succes, serverul va trimite un cookie de
sesiune. Din raspunsul primit, vom extrage cookie care reprezinta dovada ca
utilizatorul este autentificat si il vom salva.

# ENTER_LIBRARY

- Pentru a face o cerere de acces in biblioteca, trebuie sa verificam daca
utilizatorul este autentificat. Aceasta verificarea se face cu ajutorul unui
cookie pe care il salvam din raspunsul primit de la server atunci cand un
utilizator se autentifica. Daca acest cookie de sesiune nu este NULL, 
vom crea o cerere HTTP de tip GET catre server. Acesta cerere va include si
cookie-ul de sesiune care dovedeste ca utilizatorul este autentificat.
- Serverul va trimite un raspuns care va include un token de autentificare, pe
care il vom salva si il vom folosi mai departe pentru a dovedi faptul ca
utilizatorul are acces la resursele bibliotecii.

# GET_BOOKS

- Pentru a vizualiza informatiile despre toate cartile aflate in biblioteca,
trebuie sa avem acces la resurse. Ne vom folosi de cookie pentru a dovedi
prima data ca suntem autentificati si apoi vom face verificarea daca token-ul
de autentificare este unul nevid.
- Daca utilizatorul este autentificat, se creeaza o cerere de tip GET prin
care se vor trimite si token-ul si cookie-ul de sesiune pentru a fi verificate.
- Din raspsunul primit de la server, vom extrage informatiile despre carti.
Am creat functia ajutatoare print_books pentru a afisa cartile intr-un format
JSON usor de inteles. Am folosit functia "json_value_get_array" pentru a accesa
array-ul de tip JSON care contine cartile din biblioteca. Pentru fiecare carte
in parte, vom extrage informatiile legate despre id si nume si le vom afisa.

# GET_BOOK

- Asemenea functiei precedente, am facut verificarile necesare pentru a vedea
daca utilizatorul are acces la resurse. In cazul in care token-ul de
autentificare este unul valid, vom crea o cerere de tip GET care va include
si cookie-ul de sesiune si token-ul. Aceasta cerere are rolul de a mentine
integritatea datelor. Serverul va trimite un raspuns. Se verifica daca cartea
cu id-ul dorit se afla in biblioteca, in caz contrar serverul va intoarce un
mesaj de EROARE de tipul "No book was found!".
- Daca in biblioteca se gaseste cartea cu id-ul dorit, atunci vom afisa toate
informatiile despre aceasta intr-un format JSON, usor de urmarit. Extragem
obiectul JSON din raspuns si vom determina valorile corespunzatoare fiecarui
camp, cu ajutorul functiei "json_object_get_string".

# ADD_BOOK

- Pentru a adauga o carte in biblioteca, verificam daca token-ul de
autentificare este unul nevid, mai exact daca avem acces la biblioteca.
Atunci cand se citesc informatiile necesare pentru carte: titlul, autorul,
genul, editura si numarul de pagini, vom face verificare pentru fiecare in
parte daca a fost introdus corespunzator. Niciunul dintre aceste caracteristici
ale carti nu trebuie sa lipseasca, in caz contrar, afisam un mesaj de EROARE.
In cazul numarului de carti, am creat functia ajutatoare "is_number" care
verifica daca pentru acest camp s-a introdus un numar intreg.
- In functia "function_add_book" am creat un obiect JSON cu caracteristicile
necesare cartii, pe care il vom trimite catre server pentru a stoca datele.
Vom crea o cerere HTTP de tip POST prin intermediul careia vom trimite
informatiile noii carti.
- Serverul va face verificarea daca toate campurile sunt valide, iar daca
verificarea are loc cu succes, atunci vor fi retinute datele. Se va trimite un
mesaj corespunzator fie in caz de eroare, fie in caz de succes.

# DELETE_BOOK

- Pentru stergerea unui carti, se va face din nou verificarea daca utilizatorul
are acces la biblioteca. Mai departe, se va crea o functie HTTP de tip
DELETE care va fi trimisa catre server. Serverul va face toate verificarile
necesare stergerii cartii, inclusiv daca acea carte cu id-ul respectiv exista.
In caz contrar, va returna un mesaj de eroare "No book was deleted!".
- Daca toate verificarile au fost realizate, cartea va fi stearsa din
biblioteca. Vom afisa un mesaj de SUCCES.

# LOGOUT

- Se verifica mai intai daca utilizatorul a fost logat inainte. Mai departe
se va crea o cerere de tip GET prin care i se va comunica serverului faptul
ca utilizatorul doreste sa de delogheze. Serverul va face verficarile necesare
si va trimite un raspuns.
- Se afiseaza un mesaj de SUCCES, daca utilizatorul s-a delogat.

# EXIT

- Se efectueaza iesirea din program
- La final se inchide conexiunea.

