///POGAR DENISA NATALIA 151 CYK

#include <bits/stdc++.h>
using namespace std;
ifstream f("date.in");
ofstream g("date.out");
vector<vector<set<string>>> cyk_dp;
map<string,set<string>> reverse_gram;
string line, simbol, aux, litera, simbol_start, cuv;
set<string> terminale, neterminale;
int nr_prod, n;
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
        reverse_gram[aux].insert(simbol);
    }
    f>>simbol_start;
    f>>cuv;
}
void afisare() {
    if (cyk_dp[0][n-1].contains(simbol_start)) g << "DA\n";
    else g << "NU\n";

    g<<"TABELA CYK: \n";
    for (int i=0;i<n;i++,g<<'\n') {
        for (int j=0;j<n;j++) {
            if (j<i)
                g<<" - ";
            else if (cyk_dp[i][j].empty())
                g<<" - ";
            else {
                g<<"( ";
                for (const auto& simb:cyk_dp[i][j])
                    g<<simb<<" ";
                g<<") ";
            }
        }
    }
}

int main() {

    citire();
    n=cuv.size();
    cyk_dp.resize(n+3);
    for(auto& line: cyk_dp) {
        line.resize(n+3);
    }

    ///initializare diagonala principala
    for (int i=0;i<n;i++) {
        string lit_curenta=string(1,cuv[i]);
        if (reverse_gram.contains(lit_curenta)) {
            for (const auto& simb:reverse_gram[lit_curenta])
                cyk_dp[i][i].insert(simb);
        }
    }

    for (int l=1;l<n;l++) {
        for (int i=0; i<n-l;i++) {
            int j=i+l;

            for (int k=i;k<=j-1;k++) {
                set<string> prefix=cyk_dp[i][k];
                set<string> sufix=cyk_dp[k+1][j];

                for (const auto& s1:prefix) {
                    for (const auto& s2:sufix) {
                        string pereche=s1+s2;
                        if (reverse_gram.contains(pereche)) {
                            for (const auto& neterminal:reverse_gram[pereche])
                                cyk_dp[i][j].insert(neterminal);
                        }
                    }
                }
            }
        }
    }

    afisare();

    return 0;
}