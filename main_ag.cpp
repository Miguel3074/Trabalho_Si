#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <sstream>
#include <string>

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
    double fitness;
    double custo;

    Individuo(const vector<int> &r, const vector<Cidade> &cidades)
    {
        rota = r;
        fitness = 1.0 / (custoRota(rota, cidades)); // inverso do custo
        custo = custoRota(rota, cidades);
    }

    bool operator<(const Individuo &outro) const
    {
        return fitness > outro.fitness; // para ordenar do melhor pro pior
    }
};

int iteracoes = 0;
vector<double> historicoG;
vector<double> historicoCusto;

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

vector<int> crossoverOX(const vector<int>& pai1, const vector<int>& pai2) {
    int n = pai1.size();
    vector<int> filho(n, -1);
    unordered_set<int> cidadesNoFilho;

    int inicio = rand() % n;
    int fim = inicio + rand() % (n - inicio);

    // Copiar segmento do pai1
    for (int i = inicio; i <= fim; ++i) {
        filho[i] = pai1[i];
        cidadesNoFilho.insert(pai1[i]);
    }

    // Preencher o restante com cidades do pai2, na ordem que aparecerem
    int pos = (fim + 1) % n;
    for (int i = 0; i < n; ++i) {
        int cidade = pai2[(fim + 1 + i) % n];
        if (cidadesNoFilho.count(cidade) == 0) {
            filho[pos] = cidade;
            cidadesNoFilho.insert(cidade);
            pos = (pos + 1) % n;
        }
    }

    return filho;
}


void mutar(vector<int> &rota)
{
    int i = rand() % rota.size();
    int j = rand() % rota.size();
    swap(rota[i], rota[j]);
}

vector<int> algoritmoGenetico(const vector<Cidade> &cidades, int geracoes = 50000, int tamanhoPop = 24, double taxaMutacao = 0.10)
{
    vector<Individuo> populacao;
    vector<int> base(cidades.size());
    for (int i = 0; i < cidades.size(); ++i)
        base[i] = i;

    for (int i = 0; i < tamanhoPop; ++i)
    {
        shuffle(base.begin(), base.end(), mt19937(random_device{}()));
        populacao.emplace_back(base, cidades);
    }

    vector<int> melhorRota = populacao[0].rota;
    double melhorCusto = custoRota(melhorRota, cidades);

    for (int g = 0; g < geracoes; ++g)
    {
        sort(populacao.begin(), populacao.end());

        if (1.0 / populacao[0].fitness < melhorCusto)
        {
            melhorCusto = 1.0 / populacao[0].fitness;
            melhorRota = populacao[0].rota;
        }

        historicoCusto.push_back(1.0 / populacao[0].fitness);
        historicoG.push_back(g);

        vector<Individuo> novaPop;
        while (novaPop.size() < tamanhoPop)
        {
            int a = rand() % (tamanhoPop / 2);
            int b = rand() % (tamanhoPop / 2);

            vector<int> filho = crossoverOX(populacao[a].rota, populacao[b].rota);

            if ((rand() / double(RAND_MAX)) < taxaMutacao)
                mutar(filho);

            novaPop.emplace_back(filho, cidades);
        }

        iteracoes++;
        cout << "\nCusto da Rota: " << melhorCusto << "  Iteracoes:  " << iteracoes << "\n";

        populacao = novaPop;
    }

    return melhorRota;
}

void gerarGraficoSVG(const vector<double> &dados, const string &nomeArquivo, const string &titulo, const string &eixoY)
{
    ofstream svg(nomeArquivo);
    if (!svg.is_open())
    {
        cerr << "Erro ao criar SVG\n";
        return;
    }

    int width = 1000, height = 400;
    svg << "<svg xmlns='http://www.w3.org/2000/svg' width='" << width << "' height='" << height << "'>\n";
    svg << "<style> .eixo { stroke: black; stroke-width: 1; } .linha { stroke: blue; stroke-width: 1; fill: none; } </style>\n";

    svg << "<line x1='40' y1='" << height - 30 << "' x2='" << width - 10 << "' y2='" << height - 30 << "' class='eixo'/>\n"; // eixo x
    svg << "<line x1='40' y1='10' x2='40' y2='" << height - 30 << "' class='eixo'/>\n";                                      // eixo y

    svg << "<text x='" << width / 2 - 50 << "' y='20' font-size='14'>" << titulo << "</text>\n";
    svg << "<text x='" << width / 2 << "' y='" << height - 5 << "' font-size='12'>tempo</text>\n";

    double maxValor = *max_element(dados.begin(), dados.end());
    double escalaY = (height - 40.0) / maxValor;
    double passoX = (width - 60.0) / dados.size();

    svg << "<polyline class='linha' points='";
    for (size_t i = 0; i < dados.size(); ++i)
    {
        double x = 40 + i * passoX;
        double y = height - 30 - dados[i] * escalaY;
        svg << x << "," << y << " ";
    }
    svg << "'/>\n";

    svg << "<text x='5' y='" << height / 2 << "' font-size='12' transform='rotate(-90, 15," << height / 2 << ")'>" << eixoY << "</text>\n";

    svg << "</svg>\n";
    svg.close();
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

int main()
{
    srand(time(0));
    vector<Cidade> cidades = lerGrafoCSV("grafo_500.csv");

    vector<int> melhorRota = algoritmoGenetico(cidades);

    cout << "\nCusto da Rota: " << custoRota(melhorRota, cidades) << "\n";

    gerarSVG(cidades, melhorRota, "rota_final.svg");
    // gerarGraficoSVG(historicoG, "grafico_temperatura.svg", "Temperatura ao longo do tempo", "Temperatura");
    // gerarGraficoSVG(historicoCusto, "grafico_custo.svg", "Custo da rota ao longo do tempo", "Custo");

    cout << "i: " << iteracoes << "\n ";

    return 0;
}
