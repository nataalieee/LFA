///NFA - Pogar Denisa Natalia 151

#include <bits/stdc++.h>
using namespace std;
ifstream f("date.in");
ofstream g("date.out");

int nr_stari,nr_tran,stare_init,nr_finale,nr_cuv;
string aux; ///aveam nev ca sa citesc stari auxiliare
vector<int> stari; ///memorez starile intr-un vector
vector<string> cuv,tranzitii; ///cuvintele de verificat + tranzitiile
vector<vector<pair<int, char>>> adj;
set<char> alfabet; ///alfabetul
bitset<100000> e_finala; ///am marcat starile finale cu 1
bool test(int sc, string cuv, int poz)
{
    if(poz==cuv.size())
    {
        ///daca am terminat literele si suntem pe o pozitie finala e acceptat
        if(e_finala[sc]) return 1;
        return 0;
    }

    for(int j=0; j<adj[sc].size(); j++)
    {
        if(adj[sc][j].second==cuv[poz])
            ///adj[sc][j].first = starea destinatie din sc cu litera cuv[poz]
            if(test(adj[sc][j].first, cuv, poz+1))
            {
                string de_pus="q"+to_string(sc)+" -> q"+to_string(adj[sc][j].first)+" cu litera "+cuv[poz];
                tranzitii.push_back(de_pus);
                return 1;
            }
    }
    return 0;
}
void afisare_alfabet()
{
    g<<"Alfabetul este: ";
    for(auto lit:alfabet) g<<lit<<' ';
    g<<'\n';
}
int to_number(string s)
{
    int cnt=0;
    for(int i=1;i<s.size();i++)
        cnt=cnt*10+(s[i]-'0');
    return cnt;
}
int main()
{
    ///citiri
    f>>nr_stari;
    adj.resize(nr_stari+5);
    for(int i=0; i<nr_stari; i++)
    {
        f>>aux;
        stari.push_back(to_number(aux));
    }
    f>>nr_tran;

    for(int i=0; i<nr_tran; i++)
    {
        string x,y;
        char lit;
        f>>x>>y>>lit;
        alfabet.insert(lit);
        int start=to_number(x);
        int endd=to_number(y);
        adj[start].push_back({endd, lit});
    }
    f>>aux;
    stare_init=to_number(aux);
    f>>nr_finale;
    for(int i=0; i<nr_finale; i++)
    {
        f>>aux;
        e_finala[to_number(aux)]=1;
    }
    f>>nr_cuv;

    for(int i=0; i<nr_cuv; i++)
    {
        f>>aux;
        cuv.push_back(aux);
    }
    ///for(int i=0;i<nr_cuv;i++) cout<<cuv[i]<<' ';


    for(int i=0; i<nr_cuv; i++)
    {
        tranzitii.clear();
        if(test(stare_init,cuv[i],0)==1)
            {
                g<<"DA\n";
                for(int i=tranzitii.size()-1; i>=0; i--)
                g<<tranzitii[i]<<'\n';
            }
        else g<<"NU\n";
    }
    afisare_alfabet();

    return 0;
}
