#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <regex>

using namespace std;

ifstream f("date.in");
ofstream g("date.out");

///variabile/structuri pentru lambda-nfa
map<int,map<string, set<int>>> nfa_tranz; ///tranzitii nfa_tranz[stare_src][lit]=stare_dest
unordered_map<string, int> nfa_stare_to_id; ///fiecarei stari i-am atribuit un id ca sa pot lucra mai usor
unordered_map<int, string> id_to_nfa_stare; ///decodificarea id-ului in numele starii
set <string> alfabet; ///alfabet
set <int> stari_finale; ///multimea starilor finale vechi
string aux;
string stare_init; ///stare initala
int nr_stari; ///numar stari
int cnt=1; ///contor pentru normalizarea starilor nfa
int nr_lit; ///numar litere din alfabet
int nr_tranzitii; ///numar tranzitii
int cod_stare_init,new_stare_init; ///id-ul starii initiale si a starii noi adaugate initiala
int nr_stari_finale; ///nr stari finale
int new_stare_finala; ///id noua stare finala adaugata

///structura pentru RegExNFA
map<int, map<int, string>> regex_tranz;

string concatenate(string s1, string s2) {
    if (s1=="lambda") return s2;
    if (s2=="lambda") return s1;
    if (s1=="" || s2=="") return s1+s2;

    return "("+s1+"."+s2+")";
}

void elimina(int nod) {
    ///ne ocupam de bucla
    string bucla;

    if (regex_tranz[nod][nod].size()==1 || regex_tranz[nod][nod].size()==0) {
        bucla=regex_tranz[nod][nod];
    }
    else {
        bucla='('+regex_tranz[nod][nod]+')';
    }

    regex_tranz[nod][nod]="";
    for (int i=0;i<=cnt;i++)
        for (int j=0;j<=cnt;j++) {
            if ( i==nod || j==nod) continue; ///vrem predecesor si succesor distincti de nodul curent

			///daca gasim muchii i->nod si nod->j
            if (regex_tranz[i][nod].size() && regex_tranz[nod][j].size()) {

				///daca e goala
                if (regex_tranz[i][j].empty()) {
                    if (bucla.size()) {
                        string bucla_cu_steluta=bucla+"*";
                        regex_tranz[i][j]=concatenate(regex_tranz[i][nod], concatenate(bucla_cu_steluta, regex_tranz[nod][j]));
                    }
                    else {
                        regex_tranz[i][j]=concatenate(regex_tranz[i][nod], regex_tranz[nod][j]);
                    }
                }
				///daca era deja o expresie pe muchie
                else {
                    if (bucla.size()) {
                        string bucla_cu_steluta=bucla+"*";
                        regex_tranz[i][j]="("+regex_tranz[i][j]+"+"+(concatenate(regex_tranz[i][nod], concatenate(bucla_cu_steluta, regex_tranz[nod][j])))+")";
                    }
                    else {
                       regex_tranz[i][j]="("+regex_tranz[i][j]+"+"+concatenate(regex_tranz[i][nod], regex_tranz[nod][j])+")";
                    }
                }
            }
        }

	///dupa ce am inserat muchiile i->j putem elimina nodul curent de tot stergandu-i tranzitiile
    for (int k=0; k<=cnt; k++) {
        regex_tranz[k][nod]="";
        regex_tranz[nod][k]="";
    }
}

int main() {
    ///citire + normalizare stari -> atribuim cate un numar de la 1 la n, pastram 0 pentru noua stare initiala
    f>>nr_stari;
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

    ///adaugam stare initiala noua
    new_stare_init=0;
    id_to_nfa_stare[new_stare_init]="q_init";
    nfa_stare_to_id["q_init"]=0;

    nfa_tranz[0]["lambda"].insert(nfa_stare_to_id[stare_init]);///adaugam tranzitia lambda catre vechea stare finala

    ///adaugam stare finala
    new_stare_finala=cnt;
    id_to_nfa_stare[cnt]="q_final";
    nfa_stare_to_id["q_final"]=cnt;
    for (auto &s: stari_finale) {
        nfa_tranz[s]["lambda"].insert(new_stare_finala); ///adaugam tranzitiile cu lambda din vechile stari finale in noua stare finala
    }

    ///incepem sa formam nfa-ul pentru RegEx prin a uni muchiile dintre doua noduri
    for (auto &[start, tranz] : nfa_tranz) {
        for (auto &[lit, destinatii ] : tranz) {
            for (int dest : destinatii) {
                if (regex_tranz[start][dest]=="") {
                    regex_tranz[start][dest]=lit;
                }
                else {
                    regex_tranz[start][dest]="("+regex_tranz[start][dest]+"+"+lit+")";
                }
            }
        }
    }

    ///eliminam nodurile pe rand
    for (int i=1;i<cnt;i++) {
        elimina(i);
    }

    ///afisam expresia regulata care se gaseste intre noua stare initiala si noua stare finala
    g<<regex_tranz[0][cnt]<<endl;
    return 0;
}
