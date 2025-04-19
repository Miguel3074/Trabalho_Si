#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <numeric>
#include <tuple>
#include <atomic>

using namespace std;

struct Cidade
{
    int id;
    double x, y;
};

double distancia(const Cidade &a, const Cidade &b)
{
    return hypot(b.x - a.x, b.y - a.y);
}

double custoRota(const vector<int> &rota, const vector<Cidade> &cidades)
{
    double custo = 0.0;
    int N = rota.size();
    for (int i = 0; i < N; ++i)
    {
        const Cidade &atual = cidades[rota[i]];
        const Cidade &proxima = cidades[rota[(i + 1) % N]]; // %N para voltar pro comeco dps
        custo += distancia(atual, proxima);
    }
    return custo;
}

struct Individuo
{
    vector<int> rota;
    double custo, fitness;
    Individuo(const vector<int> &r, const vector<Cidade> &c) : rota(r)
    {
        custo = custoRota(rota, c);
        fitness = 1.0 / custo;
    }
    bool operator<(const Individuo &o) const { return fitness > o.fitness; }
};

vector<Cidade> lerGrafoCSV(const string &nomeArquivo)
{
    vector<Cidade> nos;
    ifstream arquivo(nomeArquivo);
    string linha;

    if (!arquivo.is_open())
    {
        cerr << "Erro ao abrir" << endl;
        return nos;
    }

    getline(arquivo, linha); // ignora a linha 1

    while (getline(arquivo, linha))
    {
        stringstream ss(linha);
        string valor;
        Cidade cidade;

        getline(ss, valor, ',');
        if (valor.empty())
            continue;
        cidade.id = stoi(valor);

        getline(ss, valor, ',');
        cidade.x = stod(valor);

        getline(ss, valor, ',');
        cidade.y = stod(valor);

        nos.push_back(cidade);
    }

    return nos;
}

vector<int> crossoverOX(const vector<int> &pai1, const vector<int> &pai2, mt19937_64 &rng)
{
    int N = pai1.size();
    vector<int> filho(N, -1);
    unordered_set<int> genesUsados;

    uniform_int_distribution<int> dist(0, N - 1);
    int inicio = dist(rng);
    int fim = dist(rng);
    if (inicio > fim)
        swap(inicio, fim);

    // copia o segmento de pai1 para o filho
    for (int i = inicio; i <= fim; ++i)
    {
        filho[i] = pai1[i];
        genesUsados.insert(pai1[i]);
    }

    // preenche o restante do filho com pai2, respeitando a ordem e evitando duplicatas
    int posFilho = (fim + 1) % N;
    for (int i = 0; i < N; ++i)
    {
        int gene = pai2[(fim + 1 + i) % N];
        if (!genesUsados.count(gene))
        {
            filho[posFilho] = gene;
            posFilho = (posFilho + 1) % N;
        }
    }

    return filho;
}

void mutar(vector<int> &rota, mt19937_64 &rng)
{
    uniform_int_distribution<int> dist(0, rota.size() - 1);

    int inicio = dist(rng);
    int fim = dist(rng);
    if (inicio > fim)
        swap(inicio, fim);

    // inverte o trecho entre as posições selecionadas (mutação 2-opt)
    reverse(rota.begin() + inicio, rota.begin() + fim + 1);
}

vector<int> algoritmoGenetico(int G, int P, double TM, const vector<Cidade> &cidades)
{
    int N = cidades.size();
    vector<int> base(N);
    iota(base.begin(), base.end(), 0);

    random_device rd;
    mt19937_64 rng(rd());

    vector<Individuo> populacao;
    for (int i = 0; i < P; ++i)
    {
        shuffle(base.begin(), base.end(), rng);
        populacao.emplace_back(base, cidades);
    }

    double melhorCusto = populacao[0].custo;
    vector<int> melhorRota = populacao[0].rota;

    uniform_real_distribution<double> prob(0.0, 1.0);

    for (int g = 0; g < G; ++g)
    {
        sort(populacao.begin(), populacao.end());
        if (populacao[0].custo < melhorCusto)
        {
            melhorCusto = populacao[0].custo;
            melhorRota = populacao[0].rota;
        }

        if (g % 1000 == 0)
        {
            cout << "Geracao " << g << "/" << G
                 << " - Melhor custo: " << melhorCusto << endl;
        }

        vector<Individuo> nova;
        nova.reserve(P);
        nova.push_back(populacao[0]);
        nova.push_back(populacao[1]);

        while (nova.size() < P)
        {
            uniform_int_distribution<int> selecao(0, P / 2 - 1);
            int a = selecao(rng);
            int b;
            do
            {
                b = selecao(rng);
            } while (b == a);

            auto filho = crossoverOX(populacao[a].rota, populacao[b].rota, rng);
            if (prob(rng) < TM)
                mutar(filho, rng);

            nova.emplace_back(filho, cidades);
        }

        populacao.swap(nova);
    }

    return melhorRota;
}

void gerarSVG(const vector<Cidade> &cidades, const vector<int> &rota, const string &nomeArquivo) // funcao pra vizualizacao da melhor rota SVG, gpt q fez, entendi como funciona mt bem
{
    ofstream svg(nomeArquivo);
    if (!svg.is_open())
    {
        cerr << "Erro ao criar SVG\n";
        return;
    }

    int width = 1000, height = 1000;
    svg << "<svg xmlns='http://www.w3.org/2000/svg' width='" << width << "' height='" << height << "'>\n";

    double maxX = 0, maxY = 0;
    for (const auto &c : cidades)
    {
        if (c.x > maxX)
            maxX = c.x;
        if (c.y > maxY)
            maxY = c.y;
    }

    auto escalar = [](double valor, double max, double limite)
    {
        return (valor / max) * (limite - 40) + 20;
    };

    for (size_t i = 0; i < rota.size(); ++i)
    {
        const Cidade &a = cidades[rota[i]];
        const Cidade &b = cidades[rota[(i + 1) % rota.size()]];

        double x1 = escalar(a.x, maxX, width);
        double y1 = escalar(a.y, maxY, height);
        double x2 = escalar(b.x, maxX, width);
        double y2 = escalar(b.y, maxY, height);

        svg << "<line x1='" << x1 << "' y1='" << y1
            << "' x2='" << x2 << "' y2='" << y2
            << "' stroke='blue' stroke-width='1'/>\n";
    }

    for (const auto &c : cidades)
    {
        double x = escalar(c.x, maxX, width);
        double y = escalar(c.y, maxY, height);
        svg << "<circle cx='" << x << "' cy='" << y << "' r='3' fill='red'/>\n";
        svg << "<text x='" << x + 4 << "' y='" << y - 4 << "' font-size='8' fill='black'>" << c.id << "</text>\n";
    }

    svg << "</svg>\n";
    svg.close();
}

void gerarSVGcidades(const vector<Cidade> &cidades, const string &nomeArquivo)
{
    ofstream svg(nomeArquivo);
    if (!svg.is_open())
    {
        cerr << "Erro ao criar SVG\n";
        return;
    }

    int width = 1000, height = 1000;
    svg << "<svg xmlns='http://www.w3.org/2000/svg' width='" << width << "' height='" << height << "'>\n";

    double maxX = 0, maxY = 0;
    for (const auto &c : cidades)
    {
        if (c.x > maxX) maxX = c.x;
        if (c.y > maxY) maxY = c.y;
    }

    auto escalar = [](double valor, double max, double limite)
    {
        return (valor / max) * (limite - 40) + 20; // margens de 20 px
    };

    for (const auto &c : cidades)
    {
        double x = escalar(c.x, maxX, width);
        double y = escalar(c.y, maxY, height);

        svg << "<circle cx='" << x << "' cy='" << y << "' r='3' fill='red'/>\n";
        svg << "<text x='" << x + 4 << "' y='" << y - 4 << "' font-size='8' fill='black'>" << c.id << "</text>\n";
    }

    svg << "</svg>\n";
    svg.close();
}


int main()
{
    srand(time(0));

    vector<Cidade> cidades = lerGrafoCSV("grafo_500.csv");
    int N = cidades.size();

    gerarSVGcidades(cidades, "cidades.svg");

    vector<int> rota(N);
    for (int i = 0; i < N; ++i)
        rota[i] = i;

    // aleatoriedade
    random_device rd;
    mt19937 g(rd());
    shuffle(rota.begin(), rota.end(), g);

    // Atributos do Algoritmo Genético
    double G = 100000; // número de gerações
    double P = 15;   // tamanho da população
    double TM = 0.2; // probabilidade de mutação

    vector<int> melhorRota = algoritmoGenetico(G, P, TM, cidades);

    cout << "\nCusto da melhor rota: " << custoRota(melhorRota, cidades) << "\n";

    gerarSVG(cidades, melhorRota, "rota_final.svg");

    return 0;
}
