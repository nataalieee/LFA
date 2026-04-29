#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <sstream>
#include <queue>
#include <stack>
#include <bitset>
using namespace std;
ifstream f("date.in");
ofstream g("date.out");

typedef pair<int, vector<string>> piv;
map<int, map<string, map<string, set<piv>>>> tranz_pda;///tranz_pda[satre_src][lit_curenta][simbol_varf_stiva][vector de litere de adaugat pe stiva]=[stare_dest]
map <string,int> pda_stare_to_id; ///fiecarei stari i-am atribuit un id ca sa pot lucra mai usor
map<int,string> id_to_pda_stare; ///decodificarea id-ului in numele starii
set <string> alfabet; ///alfabetul cuvantului
set <string>alfabet_stiva; ///alfabetul de pe stiva
set <int> stari_finale; ///starile finale
bitset<100000> e_finala;
string aux;
string cuvant_de_testat;
string simbol_stiva; ///simbolul stivei
string stare_init; ///stare initiala
int nr_stari; ///nuamr stari
int cnt=1; ///contor pentru normalizat starile
int nr_lit,nr_lit_stiva; ///nuamrul literelor din alfabet, de pe stiva
int nr_tranzitii; ///nuamr tranzitii
int cod_stare_init;
int nr_stari_finale;
///obiect = configuratie pentru coada
struct obiect{
    int stare_curenta;
    string cuv_ramas;
    vector<string> stiva;

    ///am avut nevoie sa cream o ordine in map pentru a-l putea folosi
    bool operator<(const obiect& other) const {
        if (stare_curenta!=other.stare_curenta)
            return stare_curenta<other.stare_curenta;
        if (cuv_ramas!=other.cuv_ramas)
            return cuv_ramas<other.cuv_ramas;
        return stiva<other.stiva;
    }
};

bool test(string& cuv){
    ///coada pentru BFS care contine obiecte formate din stare_curenta, cuvantul_ramas_de_parcurs, stiva_in_momentul_curent
    queue<obiect> q;

    ///initializare coada
    obiect start;
    start.stare_curenta=cod_stare_init;
    start.cuv_ramas=cuv;
    start.stiva.push_back(simbol_stiva);

    q.push(start);
    map<obiect,int> vizitat; ///util pentru a evita repetarea aceleasi configuratii la nesfarsit

    while(q.size()){
        obiect curent=q.front();
        q.pop();

        ///evitam buclele cu lambda care ne pot provoca loopuri infinite
        if (vizitat[curent]==1) {
            continue;
        }
        vizitat[curent]=1;///marcam configuratia vizitata

        ///am terminat de procesat cuvantul si am ajuns in stare finala
        if(curent.cuv_ramas=="" && e_finala[curent.stare_curenta]){
            return true;
        }

        ///daca am terminat stiva ne blocam
        if (curent.stiva.empty()) continue;

        string simbol_curent=curent.stiva.back();
        int stare_actuala=curent.stare_curenta;


        ///daca mai avem litere in cuvant
        if (!curent.cuv_ramas.empty()) {
            char lit_curenta=curent.cuv_ramas[0];
            string lit_c=string(1,lit_curenta);

            ///aici parcurgem "taind" o litera

            ///parcurgem starile ca sa vedem unde putem ajunge
            for (auto &it: tranz_pda[stare_actuala][lit_c][simbol_curent]){
                int stare_dest=it.first;
                string new_cuvant=curent.cuv_ramas.substr(1);
                vector <string>simb_de_pus=it.second;

                ///cream noua configuratie de pun in coada
                obiect new_config;
                new_config.stare_curenta=stare_dest;
                new_config.cuv_ramas=new_cuvant;
                new_config.stiva=curent.stiva;
                new_config.stiva.pop_back();

                ///ne asiguram ca nu punem lambda pe stiva
                for (int i = simb_de_pus.size()-1; i>=0; i--) {
                    if (simb_de_pus[i]!="lambda") {
                        new_config.stiva.push_back(simb_de_pus[i]);
                    }
                }
                ///punem in coada noua configuratie
                q.push(new_config);
            }
        }

        ///aici testam muchiile lambda care nu taie nicio litera dar ne duc intr-o noua stare
        for (auto &it: tranz_pda[stare_actuala]["lambda"][simbol_curent]){
            int stare_dest=it.first;
            vector <string>simb_de_pus=it.second;

            obiect new_config;
            new_config.stare_curenta=stare_dest;
            new_config.cuv_ramas=curent.cuv_ramas;
            new_config.stiva=curent.stiva;
            new_config.stiva.pop_back();

            for (int i = simb_de_pus.size()-1; i>=0; i--) {
                if (simb_de_pus[i]!="lambda") {
                    new_config.stiva.push_back(simb_de_pus[i]);
                }
            }
            q.push(new_config);
        }

    }
    return false;
}
int main(){
    ///citire + normalizare stari -> atribuim cate un numar de la 1 la n
    f>>nr_stari;
    for (int i=0; i<nr_stari; i++) {
        f>>aux;
        pda_stare_to_id[aux]=cnt;
        id_to_pda_stare[cnt]=aux;
        cnt++;
    }

    f>>nr_lit;
    for (int i=0; i<nr_lit; i++){
        f>>aux;
        alfabet.insert(aux);
    }
    f>>nr_lit_stiva;
    for(int i=0;i<nr_lit_stiva;i++)
    {
        f>>aux;
        alfabet_stiva.insert(aux);
    }
    f>>nr_tranzitii;

    // Ramanea un '\n' pe linie in buffer si asta ne citea ca o linie in plus
    string dummy;
    getline(f, dummy);

    for (int i=0; i<nr_tranzitii; i++) {
        string linie;
        getline(f,linie);

        stringstream ss(linie);
        string cuvant;
        vector<string> v;

        ///CA SA MEARGA ASTA TREBUIE CA LITERELE DE PUS PE STIVA SA FIE DATE CU SPATIU
        while (ss >> cuvant) {
             v.push_back(cuvant);
        }
        string s1,s2,lit_cuv,lit_stiva;
        s1=v[0];
        lit_cuv=v[1];
        lit_stiva=v[2];
        s2=v[3];
        vector<string>curent;
        for(int j=4;j<v.size();j++)
        {
            curent.push_back(v[j]);
        }
        tranz_pda[pda_stare_to_id[s1]][lit_cuv][lit_stiva].insert({pda_stare_to_id[s2],curent});
        curent.clear();
        v.clear();
    }
    f>>stare_init;
    cod_stare_init=pda_stare_to_id[stare_init];

    f>>simbol_stiva;

    f>>nr_stari_finale;
    for (int i=0;i<nr_stari_finale; i++) {
        f>>aux;
        stari_finale.insert(pda_stare_to_id[aux]);
        e_finala[pda_stare_to_id[aux]]=1;
    }
    f>>cuvant_de_testat;

    if(test(cuvant_de_testat))
        cout<<"ACCEPTAT \n";
    else cout<<"NEACCEPTAT \n";

    return 0;
}