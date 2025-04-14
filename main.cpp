#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
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

int i = 0;

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

vector<int> gerarVizinho(const vector<int> &rota)
{
    vector<int> nova = rota;
    int i = rand() % rota.size();
    int j = rand() % rota.size();
    if (i > j)
        swap(i, j);                              // garante i < j
    reverse(nova.begin() + i, nova.begin() + j); // gera um vizinho invertendo um trecho aleatório da rota 2-opt
    return nova;
}

vector<int> simulatedAnnealing(vector<int> rotaInicial, const vector<Cidade> &cidades, double T_inicial, double T_final, double alpha)
{
    vector<int> melhorRota = rotaInicial;
    double T = T_inicial;
    vector<int> rotaAtual = rotaInicial;

    while (T > T_final)
    {
        vector<int> vizinho = gerarVizinho(rotaAtual);
        double custoAtual = custoRota(rotaAtual, cidades);
        double custoVizinho = custoRota(vizinho, cidades);

        double random = (double)rand() / RAND_MAX; // valor aleatorio entre 0 e 1

        if (custoVizinho < custoAtual || exp((custoAtual - custoVizinho) / T) > random)
        {
            rotaAtual = vizinho;
        }

        if (custoRota(rotaAtual, cidades) < custoRota(melhorRota, cidades))
        {
            melhorRota = rotaAtual;
        }

        //cout << "\nmelhor Custo: " << custoRota(melhorRota, cidades)<< "      Custo: " << custoRota(rotaAtual, cidades) <<"    T   "<< T;

        T *= alpha;
        i++;
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

int main()
{
    srand(time(0));
    vector<Cidade> cidades = lerGrafoCSV("grafo_500.csv");
    int N = cidades.size();

    // //TESTES
    // cout << "Total de cidades: " << N << "\n\n";
    // for (int i = 0; i < 5 && i < N; ++i)
    // {
    //     cout << "Cidade " << cidades[i].id << ": x=" << cidades[i].x << ", y=" << cidades[i].y << "\n";
    // }
    // vector<vector<double>> distancias(N, vector<double>(N));
    // for (int i = 0; i < N; ++i)
    // {
    //     for (int j = 0; j < N; ++j)
    //     {
    //         distancias[i][j] = distancia(cidades[i], cidades[j]);
    //     }
    // }
    // cout << "\nDistancia entre cidade 2 e 3: " << distancias[2][3] << "\n";
    // cout << "Distancia cidade 0 para ela mesma: " << distancias[0][0] << "\n\n";

    vector<int> rota(N);
    for (int i = 0; i < N; ++i)
        rota[i] = i;

    // aleatoriedade
    random_device rd;
    mt19937 g(rd());
    shuffle(rota.begin(), rota.end(), g);

    // atributos
    double T_inicial = 10000; // temperatura inicial  para cada 0 tbm aumenta o tempo de processamento mas n tanto quanto o alpha
    double T_final = 0.00001;    // temperatura final   n entendi muito bem como funciona ainda, esse numero foi o gpt que deu quando perguntei sobre os atributos
    double alpha = 0.9999;     // taxa de resfriamento (0,00001 eu acho q ta)  PARA CADA 9 NO FINAL AUMENTA MUITO O TEMPO DE PROCESSAMENTO MAS DA UMA RESPOSTA MELHOR, com 0.99999; consegui 18790.5 mas demorou uns 5-10 min



    // double T_inicial = 10000; 
    // double T_final = 0.00001;
    // double alpha = 0.999999;  deixei um tempao rodando com esses e deu 18242.2


    vector<int> melhorRota = simulatedAnnealing(rota, cidades, T_inicial, T_final, alpha);

    cout << "\nMelhor Rota Encontrada: ";
    for (int i : melhorRota)
    {
        cout << i << " ";
    }
    cout << "\nCusto da Rota: " << custoRota(melhorRota, cidades) << "\n";

    gerarSVG(cidades, melhorRota, "rota_final.svg");

    cout << "i: " << i;

    return 0;
}
