///Lambda-NFA - Pogar Denisa Natalia 151

#include <bits/stdc++.h>
using namespace std;
ifstream f("date.in");
ofstream g("date.out");

int nr_stari,nr_tran,stare_init,nr_finale,nr_cuv;
string aux; ///aveam nev ca sa citesc stari auxiliare
vector<int> stari; ///memorez starile intr-un vector
vector<string> cuv; ///cuvintele de verificat
vector<set<int>> inc; ///lambda-inchiderile pentru fiecare stare de la 0 la n
map<int,map<string, set<int>>> adj; ///am tinut minte tranzitiile, de data asta intr-un map
set<string> alfabet;///alfabetul
bitset<100000> e_final;///am marcat starile finale cu 1

bool test(int start, string cuv)
{
    ///am luat 2 unordered_sets pentru ca fiecare element sa se insereze o singura data
    unordered_set<int> stari_curente,stari2;

    ///am facut lambda-inchiderea pt starea initiala
    for(auto it:inc[start])
        stari_curente.insert(it);

    ///parcurg cuvantul litera cu litera
    for(int i=0; i<cuv.size(); i++)
    {
        string lit_crt(1, cuv[i]); ///convertesc charul la string ca sa mearga adj
        ///parcurg starile curente
        for(auto stare_c:stari_curente)
            ///parcurg starile "destinatie" in care pot ajunge din starea mea cu litera actuala
            for(auto dest:adj[stare_c][lit_crt])
                ///parcurg lambda-inchiderea starii ca sa le adaug direct pe toate in stari2
                for(auto lambda_dest:inc[dest])
                    stari2.insert(lambda_dest);

        stari_curente=stari2;
        stari2.clear();

    }
    ///verific daca am ajuns intr-o stare finala
    for(auto it: stari_curente)
        if(e_final[it]) return 1;
    return 0;
}
void afisare_alfabet()
{
    g<<"Alfabetul este: ";
    for(auto lit:alfabet) g<<lit<<' ';
    g<<'\n';
}
void l_inchidere(int nod)
{
    bitset <100000> viz;
    queue <int>q;
    inc[nod].insert(nod);
    viz[nod]=1;
    q.push(nod);

    while(q.size())
    {
        int x=q.front();
        q.pop();
        for(auto it: adj[x]["lambda"])
            if(!viz[it])
            {
                inc[nod].insert(it);
                q.push(it); viz[it]=1;
            }
    }
}
bool are_vecin_final(int nod)
{
    for(auto it:inc[nod])
        if(e_final[it]) return 1;
    return 0;
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
    inc.resize(nr_stari+5);
    for(int i=0; i<nr_stari; i++)
    {
        f>>aux;
        stari.push_back(to_number(aux));
        ///am luat starile drept numere naturale de la 0 la n
    }
    f>>nr_tran;

    for(int i=0; i<nr_tran; i++)
    {
        string x,y,lit;
        f>>x>>y>>lit;
        alfabet.insert(lit);
        int start=to_number(x);
        int endd=to_number(y);
        adj[start][lit].insert(endd);
    }
    f>>aux;
    stare_init=to_number(aux);
    f>>nr_finale;
    for(int i=0; i<nr_finale; i++)
    {
        f>>aux;
        e_final[to_number(aux)]=1;
    }
    f>>nr_cuv;
    for(int i=0; i<nr_cuv; i++)
    {
        f>>aux;
        cuv.push_back(aux);
    }

    ///facem lambda-inchiderile
    for(int i=0; i<nr_stari; i++)
        l_inchidere(i);

    ///facem toate starile finale
    for(int i=0; i<nr_stari; i++)
        if(e_final[i]==0 && are_vecin_final(i)==1)
            e_final[i]=1;

    ///testam
    for(int i=0; i<nr_cuv; i++)
    {
        ///tranzitii.clear();
        if(test(stare_init,cuv[i])==1) g<<"DA\n";
        else g<<"NU\n";
    }

    afisare_alfabet();

    return 0;
}
