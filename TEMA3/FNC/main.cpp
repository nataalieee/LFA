///POGAR DENISA NATALIA 151 FNC

#include <bits/stdc++.h>
using namespace std;
ifstream f("date.in");
ofstream g("date.out");
map<string,set<string>> grammar,reverse_gram;
map<string,set<string>> adj;
unordered_map<string, bool> found;
vector<pair<int,string>> level_simbol;
string line,simbol,aux, simbol_start, litera;
set<string> terminale, neterminale;
unordered_set<string> neterminale_nule,new_neterminale_nule;
map<string, set<vector<string>>> grammar_fnc;
int contor_x=0;
int contor_y=0;
int nr_prod;
bool lambda_in_limbaj;
void citire() {
    getline(f, line); ///neterminalele
    if (!line.empty()){
        stringstream ss(line);
        while (ss>>simbol) {
            neterminale.insert(simbol);
        }
    }
    getline(f, line); ///terminalele
    if (!line.empty()){
        stringstream ss(line);
        while (ss>>litera) {
            terminale.insert(litera);
        }
    }
    f>>nr_prod;
    for (int i=1;i<=nr_prod;i++) {
        f>>simbol>>aux;
        grammar[simbol].insert(aux);
        if (aux!="lambda") {
            reverse_gram[aux].insert(simbol);
        }
    }
    f>>simbol_start;
}
void adauga_noul_start() {
    string vechiul_start =simbol_start;
    simbol_start=vechiul_start+"1";
    neterminale.insert(simbol_start);
    grammar[simbol_start].insert(vechiul_start);
}
void elimina_lambda_productii() {

    bool gata=0;
    while (!gata) {
        int old_size = neterminale_nule.size();
        for (const auto& nt : neterminale) {
            ///daca gebereaza lambda
            if (grammar[nt].contains("lambda")) {
                neterminale_nule.insert(nt);
                continue;
            }

            //verificam daca are vreo productie formata doar din neterminale "nule"
            for (const string& prod: grammar[nt]) {
                bool toate_nule=true;
                for (char c: prod) {
                    string s(1, c);
                    if (neterminale_nule.contains(s)) {
                        toate_nule=0; break;
                    }
                }
                if (toate_nule) {
                    neterminale_nule.insert(nt);
                    break;///am gasit deja o productie care il face nul
                }
            }
        }

        if (neterminale_nule.size()==old_size) {
            gata=1;
        }
    }
    if (neterminale_nule.contains(simbol_start)) {
        lambda_in_limbaj=1;
    }
    map<string, set<string>> new_grammar = grammar;

    for (const auto& [stanga, productii] : grammar) {
        for (const string& prod : productii) {
            if (prod == "lambda") continue;///ignoram lambda

           ///gasim pozitiile neterminalelor nule
            vector<int> pozitii_nule;
            for (int i=0; i<prod.length(); i++) {
                string litera_curenta(1, prod[i]);
                if (neterminale_nule.contains(litera_curenta)) {
                    pozitii_nule.push_back(i);
                }
            }

            ///daca nu avem neterminale nule o lasam asa
            if (pozitii_nule.empty()) {
                new_grammar[stanga].insert(prod);
                continue;
            }

            ///generam toate combinatiile posibile
            int total_combinatii = 1<<pozitii_nule.size();
            for (int mask=0; mask<total_combinatii; mask++) {
                string noua_prod="";
                for (int i=0; i<prod.length(); i++) {

                    auto it = find(pozitii_nule.begin(), pozitii_nule.end(), i);
                    if (it!=pozitii_nule.end()) {
                        int index_in_lista=distance(pozitii_nule.begin(), it);
                        ///daca bitul curent e 0 => il pastram
                        if ((mask & (1<<index_in_lista))==0) {
                            noua_prod+=prod[i];
                        }
                    } else {
                        ///nu e terminal nul
                        noua_prod+=prod[i];
                    }
                }

                ///adaugam productia dupa ce s-a golit complet
                if (!noua_prod.empty()) {
                    new_grammar[stanga].insert(noua_prod);
                }
            }
        }
    }

    ///eliminam productiile lambda "pure"
    for (auto& [stanga, productii]: new_grammar) {
        productii.erase("lambda");
    }

    grammar = new_grammar;
}
void dfs(string start, int niv) {
    found[start]=true;
    level_simbol.push_back({niv,start});
    for (const auto& simbol:grammar[start]) {
        if (neterminale.contains(simbol) ) {
            if (!found[simbol]) dfs(simbol, niv+1);
        }
    }
}
void elimina_productii_unitare() {

    map<string,set<string>> new_grammar;

    for (const auto& simbol:neterminale) {
        set<string> inchidere;
        ///gasim inchiderea folosind o coada (tip bfs) pt productiile de tip A -> B
        queue<string> q;

        inchidere.insert(simbol);
        q.push(simbol);

        while (q.size()) {
            string curent=q.front();
            q.pop();

            for (const string& dest:grammar[curent]) {
                if (neterminale.contains(dest) && !inchidere.contains(dest)) {
                    inchidere.insert(dest);
                    q.push(dest);
                }
            }
        }
        for (const auto& s: inchidere) {
            for (const auto& dest:grammar[s]) {
                bool e_unitara=(dest.length()==1 && neterminale.contains(dest));
                if (!e_unitara && dest!="lambda") {
                    new_grammar[simbol].insert(dest);
                }
            }
        }

    }
    grammar=new_grammar;
}
void stergere_simboluri_neproductive() {
    unordered_map<string,bool> found_productiv;
    set<string> productive;
    ///gasim de jos in sus ce neterminale genereaza ceva concret
    bool gata=0;
    while (!gata) {
        int old_size=productive.size();
        for (const auto& [neterminal, productii]:grammar) {
            ///e deja gasit
            if (productive.contains(neterminal)) continue;

            for (const auto& dest: productii) {
                bool e_productiva=1;

                for (char c:dest) {
                    string simbol(1,c);

                    if (neterminale.contains(simbol) && !productive.contains(simbol)) {
                        e_productiva = false;
                        break;
                    }
                }
                if (e_productiva) {
                    productive.insert(neterminal); break;
                }
            }
        }
        if (productive.size()==old_size) {
            gata=true;
        }
    }

    map<string,set<string>> new_grammar;

    for (const auto& [simb, productii]:grammar) {
        if (!productive.contains(simb)) continue;

        for (const auto& prod:productii) {
            ///verificam daca productia e inca valabila (nu contine simboluri sterse)
            bool pastram=1;

            for (char c:prod) {
                string lit_curenta=string(1,c);
                if (neterminale.contains(lit_curenta) && !productive.contains(lit_curenta)) {
                    pastram=0; break;
                }
            }
            if (pastram) {
                new_grammar[simb].insert(prod);
            }

        }
    }

    grammar=new_grammar;

}
void stergere_simboluri_inaccesibile() {
    set<string> accesibile;
    queue<string> q;

    accesibile.insert(simbol_start);
    q.push(simbol_start);

    ///pornim din start si vedem in ce neterminale putem ajunge
    while (!q.empty()) {
        string curent = q.front();
        q.pop();
        for (const string& prod : grammar[curent]) {

            for (char c : prod) {
                string simbol(1, c);

                if (neterminale.contains(simbol) && !accesibile.contains(simbol)) {
                    accesibile.insert(simbol);
                    q.push(simbol);
                }
            }
        }
    }


    map<string, set<string>> new_grammar;
    for (const auto& [neterminal, productii]: grammar) {
        if (accesibile.contains(neterminal)) {
            new_grammar[neterminal]=productii;
        }
    }

    grammar=new_grammar;

    set<string> new_neterminale;
    for (const string& nt: neterminale) {
        if (accesibile.contains(nt)) {
            new_neterminale.insert(nt);
        }
    }
    neterminale=new_neterminale;
}
void transformare_forma_simpla() {
    ///spargem in productii de forma A -> a sau A -> BC
    map<string, string> dict_x; /// pentru maparea terminalelor

    for (const auto& [stanga, productii]: grammar) {
        for (const auto& dreapta: productii) {

           ///cand merge intr-un singur terminal
            if (dreapta.length()==1 && terminale.contains(dreapta)) {
                grammar_fnc[stanga].insert({dreapta});
                continue;
            }

            ///prodcite de lungime>=2
            vector<string> simboluri_curente;

            for (char c: dreapta) {
                string simbol(1, c);

                if (terminale.contains(simbol)) {
                    //daca e terminal, verificam daca are deja un X asociat
                    if (dict_x.find(simbol)==dict_x.end()) {
                        string nou_x="X"+to_string(contor_x++);
                        dict_x[simbol]=nou_x;
                        grammar_fnc[nou_x].insert({simbol}); ///adaugam Xi -> lit
                    }
                    simboluri_curente.push_back(dict_x[simbol]);
                } else {
                    ///daca e neterminal
                    simboluri_curente.push_back(simbol);
                }
            }

            ///daca avem 2 neterminale doar
            if (simboluri_curente.size()==2) {
                 grammar_fnc[stanga].insert(simboluri_curente);
            }
            //daca avem mai mult de 2 neterminale
            else if (simboluri_curente.size()>2) {
                 string curent_stanga =stanga;

                 for (int i=0; i<simboluri_curente.size()-2; i++) {
                     string nou_y="Y" + to_string(contor_y++);

                     grammar_fnc[curent_stanga].insert({simboluri_curente[i], nou_y});
                     curent_stanga=nou_y;
                 }
                 grammar_fnc[curent_stanga].insert({
                     simboluri_curente[simboluri_curente.size()-2],
                     simboluri_curente[simboluri_curente.size()-1]
                 });
            }
        }
    }
    if (lambda_in_limbaj) {
        grammar_fnc[simbol_start].insert({"lambda"});
    }
}
void afisare_fnc() {
    g<<"---------- GRAMATICA FNC ------------\n";
    g<<"\n";
    g<<"Multime neterminali: ";
    for (const auto& [neterminal, productie]: grammar_fnc) {
        g <<neterminal<<" ";
    }
    g<<"\n";
    g<<"Multime terminali: ";
    for (const auto& term: terminale) {
        g<<term<<" ";
    }
    g<<"\n";
    g<<"Productii: \n";
    for (const auto& [stanga, productii] : grammar_fnc) {
        g<<stanga<<" -> ";
        bool prima_prod=1;
        for (const auto& prod : productii) {
            if (!prima_prod) g<<" | ";
            for (const auto& simb: prod) {
                g<<simb;
            }
            prima_prod=0;
        }
        g<<"\n";
    }
    g<<"Simbol start: "<<simbol_start;
}
int main() {
    citire();
    adauga_noul_start();
    elimina_lambda_productii();
    elimina_productii_unitare();
    stergere_simboluri_neproductive();
    stergere_simboluri_inaccesibile();
    transformare_forma_simpla();
    afisare_fnc();
    return 0;
}