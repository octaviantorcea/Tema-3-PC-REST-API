# Tema 3 PC - REST API
Torcea Octavian 324CA

* Pentru parsarea campurilor de tip JSON din mesaje folosesc biblioteca
`parson`, deoarece tema este implementata in limbajul C si aceasta biblioteca a
fost recomandata in enuntul temei.

* `constants.h`: contine toate stringurile constante pe care le folosesc in
implementarea clientului.

* `library_client.c`: implementarea propiu-zisa a clientului

## Implementare
Intr-o bucla infinita, clientul asteapta sa primeasca de la
tastatura o comanda. Pentru a ma asigura ca exista mereu o conexiune intre
client si server, pentru fiecare comanda noua primita se va deschide un nou
socket TCP pentru a comunica cu serverul, iar la sfarsitul comenzii, acel socket
se va inchide. Mesajele se vor trimite catre acelasi host mereu,
iar ruta de acces se va modifica in functie de comanda primita.

## Descrierea comenzilor
* register: clientul asteapta inputul utilizatorului pentru username si
password (acestea pot contine maxim 64 de caractere). Dupa primirea lor,
construiesc un JSONObject pentru a pune informatiile in el si apoi trimit
mesajul catre server. Astept apoi raspunusul de la server si verific daca a
aparut vreo eroare. In cazul aparitiei, unei erori, se va afisa mesajul de
eroare primit de la server. In caz contrar, se va afisa un mesaj de succes de
tipul: `***SUCCESS***: You have registered successfully!`

* login: la fel ca la register, se asteapta primirea username-ului si a
parolei si se va trimite mesajul catre server. In cazul unei erori, se va afisa
mesajul de eroare primit de la server. In caz contrar, extrag din mesajul primit
cookie-ul de sesiune si il memorez, apoi se va afisa un mesaj de succes.

* enter_library: trimit un get request cu cookie-ul primit in urma comenzii
de login. Daca acel cookie nu exista, serverul va trimite un mesaj de eroare. In
cazul in care totul a functionat corect, extrag din mesajul primit tokenul jwt
si il memorez.

* get_books: trimit un get request cu cookie-ul si cu tokenul jwt primit
anterior. In cazul in care utilizatorul nu a dat prima data comanda de
`enter_library`, serverul va intoarce un mesaj de eroare. In caz contrar, se va
primi de la server un mesaj ce va contine un JSON_Array cu toate cartile
din biblioteca. Daca nu exista nicio carte, se va afisa `No books!`. In cazul in
care exista carti, voi parsa array-ul folosind functia `json_array_get_object` si
voi afisa pe rand cartile

* get_book: se asteapta prima data inputul utilizatorului pentru a primi un
id. Apoi, la ruta de acces folosita la punctul anterior, concatenez id-ul si
apoi trimit un get request folosind cookie-ul si tokenul jwt. Daca id-ul nu
exista sau utilizatorul nu are acces la biblioteca, serverul va intoarce un
mesaj de eroare, pe care il voi afisa. Daca nu este un mesaj de eroare, atunci
inseamna ca am primit un mesaj in care se vor afla informatii despre cartea cu
id-ul dat ca input. Urmez aceeasi pasi ca la punctul anterior pentru a extrage
informatiile din JSON_Array, doar ca de data asta in array va fi doar un singur
obiect.

* add_book: clientul asteapta inputul utilizatorului pentru toate campurile
necesare unei carti. In cazul in care acesta nu introduce nicio informatie sau
o informatie invalida, va primi un mesaj de tipul
`Please insert a valid <field>.` si va trebui sa reintroduca acel camp. Ordinea
prompturilor este fix cea din cerinta temei. Construiesc un JSONObject si voi
trimite un mesaj de tip `POST` (impreuna cu cookie-ul de sesiune si tokenul jwt)
catre server. In cazul in care utilizatorul nu are acces la biblioteca, serverul
va intoarce un mesaj de eroare; altfel se va afisa un mesaj de succes.

* delete_book: identic ca la comanda `get_book`, doar ca in loc de get
request se va trimite un mesaj de tipul delete request.

* logout: setez cookie-ul si tokenul jwt la 0.

* exit: programul iese din bucla infinita si se va inchide.

* orice alta comanda: se va afisa mesajul `Unknown command.` .
