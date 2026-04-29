#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <bitset>
using namespace std;

ifstream f("date.in");
ofstream g("date.out");

///variabile/structuri pentru lambda-nfa
map<int,map<string, set<int>>> nfa_tranz; ///tranzitii nfa_tranz[stare_src][lit]=stare_dest
unordered_map<string, int> nfa_stare_to_id; ///fiecarei stari i-am atribuit un id ca sa pot lucra mai usor
unordered_map<int, string> id_to_nfa_stare; ///decodificarea id-ului in numele starii
set <string> alfabet; ///alfabetul
set <int> stari_finale; ///stari finale
string aux;
string stare_init; ///stare initiala
int nr_stari;
int cnt=1; ///contor pentru normalizarea starilor nfa
int nr_lit; ///numar litere din alfabet
int nr_tranzitii; ///numar tranzitii
int cod_stare_init; ///id ul starii initiale
int nr_stari_finale; ///numar stari finale

///variabile/structuri pentru dfa
map<set<int>, int> dfa_stare_to_id; ///fiecarei stari i-am atribuit un id ca sa pot lucra mai usor
map<int, set<int>> id_to_dfa_stare; ///decodificarea id-ului in numele starii
map<int, map<string, int>> dfa_tranz; ///tranzitii dfa_tranz[stare_src][lit]=stare_dest
map<set<int>, int> gasit; ///mapa de verificat daca am mai gasit submultimea de stari ale nfa-ului (acestea vor fi noile stari in dfs)
vector<set<int>> inchidere; ///lambda inchiderea fiecarei stari
queue<set<int>> stari_de_procesat; ///coada cu stari de procesat -> submultime de stari dn vechiul nfa
set<int> dfa_stari_finale; ///stari finale
int dfa_nr_stari; ///numar stari finale dfa
int dfa_cnt=1; ///contor pentru normalizarea starilor dfa

///variabile/structuri pentru dfa minimizat
int nr_stari_M; ///numari stari
int stare_init_M; ///id_stare_initiala
set<int> stari_finale_M; ///stari finale
vector<int> id_grup; ///id ul grupul fiecarei stari -> pentru algoritm
vector<set<int>> stari_ale_grupului; ///satrile din fiecare grupa -> pentru afisare

void lambda_inchidere(int nod)
{
    bitset <100000> viz;
    queue <int>q;
    inchidere[nod].insert(nod);
    viz[nod]=1;
    q.push(nod);

    while(q.size())
    {
        int x=q.front();
        q.pop();
        for(auto it: nfa_tranz[x]["lambda"])
            if(!viz[it])
            {
                inchidere[nod].insert(it);
                q.push(it); viz[it]=1;
            }
    }
}

void nfa_to_dfa() {
    ///punem starea initiala + lambda-inchiderea ei
    stari_de_procesat.push(inchidere[cod_stare_init]);

    ///o marcam ca gasita
    gasit[inchidere[cod_stare_init]]=1;

    ///normalizam starile dfa-ului -> folsoim acel map<set<int>,int> si map<int, set<int>>
    dfa_stare_to_id[inchidere[cod_stare_init]]=dfa_cnt;
    id_to_dfa_stare[dfa_cnt]=inchidere[cod_stare_init];
    dfa_cnt++;

    while (stari_de_procesat.size()) {

        set<int> stari_curente=stari_de_procesat.front();
        stari_de_procesat.pop();

        int id_stare_curenta=dfa_stare_to_id[stari_curente];

        ///pentru fiecare set de stari cuente, parcurgem literele pe rand
        for (auto &lit: alfabet) {
            if (lit=="lambda") continue;///in caz ca apare in alfabet

            ///vedem ce stari destinatie avem
            set<int> stari_dest,stari_dest_after_lambda;
            for (auto &s : stari_curente) {
                for (auto &s_d: nfa_tranz[s][lit])
                    stari_dest.insert(s_d);
            }

            if (stari_dest.empty()) continue;///daca nu avem stari destinatie

            ///includem si lambda-tranzitiile
            for (auto &s_d: stari_dest) {
                for (auto &s:inchidere[s_d])
                    stari_dest_after_lambda.insert(s);
            }


            ///daca nu am mai gasit aceasta submultime de star pana acum
            if (gasit[stari_dest_after_lambda]!=1) {
                gasit[stari_dest_after_lambda]=1;

                ///ii atribuim id-ul
                dfa_stare_to_id[stari_dest_after_lambda]=dfa_cnt;
                id_to_dfa_stare[dfa_cnt]=stari_dest_after_lambda;
                dfa_cnt++;

                ///o adaugam in coada
                stari_de_procesat.push(stari_dest_after_lambda);
            }

            ///salvam tranzitiile dfa-ului
            int id_stare_dest=dfa_stare_to_id[stari_dest_after_lambda];
            dfa_tranz[id_stare_curenta][lit]=id_stare_dest;
        }
    }

}
bool check_finala(set<int>&s_curente ) {
    for (auto &s : s_curente) {
        for (auto &s2: stari_finale) {
            if (s==s2) return true; ///gasim o stare finala inclusa in submultimea starilor curente
        }
    }
    return false;
}
void dfa_afis() {
    g<<" ---------------------------------------------------------------------------\n";
    g<<"                            DFA ECHIVALENT                                       \n";
    g<<" ---------------------------------------------------------------------------\n";
    g<<"Multimea starilor noului dfa este: ";
    for (int i=1;i<=dfa_nr_stari;i++) {
        g<<"q"<<i<<" care a fost formata din vechile: ";
        for (auto &s: id_to_dfa_stare[i]) {
            g<<s<<' ';
        }
        g<<'\n';
    }

    g<<"Alfabetul este: ";
    for (auto &lit: alfabet) {
        if (lit!="lambda") g<<lit<<' ';
    }
    g<<'\n';

    g<<"Functiile de tranzitie sunt: \n";
    for (int i=0;i<=dfa_nr_stari;i++) {
        for (auto &lit: alfabet) {
            if (lit=="lambda") continue;

            if (dfa_tranz[i][lit]) {
                int dest=dfa_tranz[i][lit];
                g<<"q"<<i<<" --> q"<<dest<<' '<<lit<<'\n';
            }
        }
    }
    g<<"Starea initiala este q"<<dfa_stare_to_id[inchidere[cod_stare_init]]<<'\n';
    g<<"Starile finale sunt: ";
    for (auto &s: dfa_stari_finale) {
        g<<"q"<<s<<' ';
    }
    g<<'\n';
    g<<'\n';
    g<<'\n';

}
void dfa_minimizare() {
    id_grup.resize(dfa_nr_stari+5);

    ///marcam starile finale ca sa ne ajute la prima partitie
    bitset<100000> e_finala;
    for (auto &s: dfa_stari_finale) {
        e_finala[s]=1;
    }

    ///impartim pe finale/non-finale
    for (int i=1;i<=dfa_nr_stari; i++) {
        if (e_finala[i]) id_grup[i]=1;
        else id_grup[i]=2;
    }
    int id=1;
    bool gata=0;
    while (!gata) {
        map<vector<int>,int> id_config; ///id configuratiei = idul vectorului signatura
        ///adica in ce grup_id ne duce o stare impreuna cu toate literele

        vector<int> new_id_grup(dfa_nr_stari+5);
        int new_id=1; /// va numara noile grupe formate la acest pas

        for (int i=1;i<=dfa_nr_stari;i++) {
            vector<int> sgn; ///vector signatura = [stare_src], [id grup stare_dest cu lit 1], [id grup stare_dest cu lit 2], ..., [id grup stare_dest cu lit n]
            sgn.push_back(id_grup[i]); ///pe prima pozitie va fi codificat grupul din care plecam

            ///parcurgem literele
            for (auto &lit : alfabet) {
                int stare_dest=dfa_tranz[i][lit];
                sgn.push_back(id_grup[stare_dest]);
            }

            /// daca n-am mai gasit configuratia pan acum ii atribuim id
            if (!id_config[sgn]) {
                id_config[sgn]=new_id;
                new_id++;
            }

            ///salvam noua grupa a starilor
            new_id_grup[i] = id_config[sgn];
        }
        ///daca numarul grupelor e acelasi cu cel de la psul anterior ne putem opri
        if (id==new_id) {
            gata=1;
        }
        else {
            id=new_id;
            id_grup=new_id_grup;
        }
    }

    nr_stari_M=id-1;///nr stari noul dfa minimizat

    ///actualizam starea initala, cele finale
    stare_init_M=id_grup[dfa_stare_to_id[inchidere[cod_stare_init]]];

    for (auto &s: dfa_stari_finale) {
        stari_finale_M.insert(id_grup[s]);
    }

    ///facem asta ca sa ne ajute la afisare
    stari_ale_grupului.resize(nr_stari_M+5);
    for (int i=0;i<=dfa_nr_stari;i++) {
        stari_ale_grupului[id_grup[i]].insert(i);
    }
}
void dfa_minimizat_afis() {

    g<<" ---------------------------------------------------------------------------\n";
    g<<"                             DFA MINIMIZAT                                       \n";
    g<<" ---------------------------------------------------------------------------\n";
    g<<"Multimea starilor noului dfa minimizat este: ";
    for (int i=1;i<=nr_stari_M;i++) {
        g<<"M"<<i<<" care a fost formata din vechile: ";
        for (int j=1;j<=dfa_nr_stari;j++)
            if (id_grup[j]==i){
                g<<"q"<<j<<' ';
            }
        g<<'\n';
    }

    g<<"Alfabetul este: ";
    for (auto &lit: alfabet) {
        if (lit!="lambda") g<<lit<<' ';
    }
    g<<'\n';

    g<<"Functiile de tranzitie sunt: \n";
    for (int i=1;i<=nr_stari_M;i++) {
        for (auto &lit: alfabet) {
            if (lit=="lambda") continue;

            ///luam prima "vechea stare" din grupul cu id ul "i"
            int stare_src=*stari_ale_grupului[i].begin();
            if (dfa_tranz[stare_src][lit]) {
                int dest=id_grup[dfa_tranz[stare_src][lit]];
                g<<"M"<<i<<" --> M"<<dest<<' '<<lit<<'\n';
            }
        }
    }
    g<<"Starea initiala este M"<<stare_init_M<<'\n';
    g<<"Starile finale sunt: ";
    for (auto &s: stari_finale_M) {
        g<<"M"<<s<<' ';
    }
    g<<'\n';

}
int main() {
    ///citire + normalizare stari -> atribuim cate un numar de la 1 la n
    f>>nr_stari;
    inchidere.resize(nr_stari+5);
    for (int i=0; i<nr_stari; i++) {
        f>>aux;
        nfa_stare_to_id[aux]=cnt;
        id_to_nfa_stare[cnt]=aux;
        cnt++;
    }

    f>>nr_lit;
    for (int i=0; i<nr_lit; i++){
        f>>aux;
        alfabet.insert(aux);
    }
    f>>nr_tranzitii;
    for (int i=0; i<nr_tranzitii; i++) {
        string s1,s2,lit;
        f>>s1>>s2>>lit;
        nfa_tranz[nfa_stare_to_id[s1]][lit].insert(nfa_stare_to_id[s2]);
    }
    f>>stare_init;
    cod_stare_init=nfa_stare_to_id[stare_init];

    f>>nr_stari_finale;
    for (int i=0;i<nr_stari_finale; i++) {
        f>>aux;
        stari_finale.insert(nfa_stare_to_id[aux]);
    }
    ///calculam lambda-inchiderile pentru fiecare nod
    for (int i=1;i<=nr_stari;i++) {
        lambda_inchidere(i);
    }

    ///transformare
    nfa_to_dfa();

    dfa_nr_stari=dfa_cnt-1;
    ///gasirea noilor stari finale
    for (int i=1;i<=dfa_nr_stari;i++) {
        set<int> stari_curente=id_to_dfa_stare[i];
        if (check_finala(stari_curente)) {
            dfa_stari_finale.insert(i);
        }
    }

    ///afisare DFA
    dfa_afis();

    ///minimizare dfs
    dfa_minimizare();

    ///afisare dfa_minimizat
    dfa_minimizat_afis();
    return 0;
}