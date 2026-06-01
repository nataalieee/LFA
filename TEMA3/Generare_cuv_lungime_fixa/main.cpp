///POGAR DENISA NATALIA 151 GENERARE_CUVINTE
#include <bits/stdc++.h>
using namespace std;
ifstream f("date.in");
ofstream g("date.out");
map<string, set<pair<string,int>>> grammar;
set<string> neterminale;
set<string> terminale;
set<string> cuv_gasite;
struct config {
    string cuv_actual;
    int nr_terminale, pas;

   /* bool operator<(const config& other) const {
        if (cuv_actual!=other.cuv_actual) return cuv_actual<other.cuv_actual;
        if (nr_terminale!=other.nr_terminale) return nr_terminale < other.nr_terminale;
        return pas<other.pas;
    }*/
};

queue<config> q;
unordered_set<string> gasit_conf;
string line,simbol,litera,aux,simbol_start,string_neterminale;
int nr_prod,lung_max;
void citire() {
    getline(f, line); ///neterminalele
    if (!line.empty()){
        stringstream ss(line);
        while (ss>>simbol) {
            neterminale.insert(simbol);
            string_neterminale+=simbol;
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
        int nr_lit=0;
        if (aux=="lambda")aux="";
        for (int j=0;j<aux.size();j++)
            if (terminale.contains(string(1,aux[j])) ) nr_lit++;
        grammar[simbol].insert({aux,nr_lit});
    }
    f>>simbol_start;
    f>>lung_max;
}
void gen_cuv(int lung) {
    config start;
    start.cuv_actual=simbol_start;
    start.nr_terminale=0;
    start.pas=1;

    gasit_conf.insert(start.cuv_actual);
    q.push(start);

    while (q.size()) {
        config curent=q.front();
        q.pop();

        ///gasit pozitia primului terminal
        unsigned int poz=curent.cuv_actual.find_first_of(string_neterminale);


        ///daca avem doar terminale
        if (poz==string::npos) {
            if (curent.cuv_actual.length()==lung) {
                cuv_gasite.insert(curent.cuv_actual);
            }
            continue;
        }

        ///daca e deja prea lung
        if (curent.nr_terminale>lung) {
            continue;
        }

        ///daca am facut deja numarul_productiilor+50 pasi prob avem o bucla
        if (curent.pas>lung+nr_prod+50) {
            continue;
        }

        ///daca nu e finalul cuvantului

        string neterminal_curent = string(1, curent.cuv_actual[poz]);
        for (const auto& dest: grammar[neterminal_curent]) {
            config new_pas;
            new_pas.cuv_actual=curent.cuv_actual;
            new_pas.cuv_actual=new_pas.cuv_actual.replace(poz,1,dest.first);
            new_pas.nr_terminale=curent.nr_terminale+dest.second;
            new_pas.pas=curent.pas+1;


            ///daca n-am mai trecut prin configuratia asta
            if (new_pas.nr_terminale<=lung && gasit_conf.contains(new_pas.cuv_actual)) {
                q.push(new_pas);
                gasit_conf.insert(new_pas.cuv_actual);
            }
        }

    }
}
int main() {
    citire();
    gen_cuv(lung_max);
    if (cuv_gasite.empty())
        g<<"nu exista";
    else {
        for (const auto& cuv:cuv_gasite) {
            g<<cuv<<"\n";
        }
    }

}