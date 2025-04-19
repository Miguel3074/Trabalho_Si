
Este projeto foi desenvolvido como parte da disciplina de Sistemas Inteligentes, com o objetivo de aplicar e comparar técnicas de otimização heurística na resolução do Problema do Caixeiro Viajante (TSP). Os algoritmos implementados foram:

- Têmpera Simulada (Simulated Annealing - SA)
- Algoritmo Genético (Genetic Algorithm - GA)

---

## Objetivos

- Implementar os algoritmos SA e GA em C++.
- Resolver instâncias do TSP utilizando grafos lidos a partir de arquivos CSV.
- Avaliar e comparar a performance de ambos os algoritmos.
- Gerar visualizações gráficas da rota e da evolução do custo durante a execução.

---

## Execução

### Compilação

Utilize um compilador C++ com suporte ao padrão C++11 ou superior:

```bash
g++ main.cpp -o tsp -std=c++11
Execução
bash
Copiar
Editar
./tsp
Os parâmetros de cada algoritmo podem ser configurados diretamente no código, na função main().

Estrutura do Projeto
main.cpp: Ponto de entrada do programa, contendo a lógica de execução principal.

simulatedAnnealing.cpp: Implementação da Têmpera Simulada.

algoritmoGenetico.cpp: Implementação do Algoritmo Genético.

utils.cpp: Funções auxiliares, como leitura de arquivos e geração de gráficos.

grafo_500.csv: Instância do problema contendo a lista de cidades com coordenadas.

Saídas Geradas
Durante a execução, o programa gera os seguintes arquivos:

rota_final.svg: Visualização da rota final gerada pelo algoritmo.

grafico_custo.svg: Gráfico da evolução do custo da solução (apenas para SA).

grafico_temperatura.svg: Gráfico da evolução da temperatura (apenas para SA).

cidades.svg: Visualização das cidades sem conexão entre elas.

Parametrização
Têmpera Simulada (SA)
T_inicial: Temperatura inicial.

T_final: Temperatura final.

alpha: Fator de resfriamento.

Algoritmo Genético (GA)
G: Número de gerações.

P: Tamanho da população.

TM: Taxa de mutação.

Esses parâmetros são definidos no início da função main() e podem ser ajustados conforme o experimento desejado.

Considerações
Os arquivos SVG podem gerar tamanhos elevados em instâncias grandes. Evite armazenar arquivos maiores que 100 MB no repositório.

Recomenda-se o uso do Git LFS caso seja necessário versionar arquivos grandes.

A visualização dos gráficos SVG pode ser feita em navegadores modernos ou ferramentas de visualização vetorial.

Autores
Miguel Dos Anjos Brack – @Miguel3074

Renzo Tognella de Rosa

Licença
Projeto desenvolvido para fins acadêmicos na Universidade Tecnológica Federal do Paraná (UTFPR), campus Curitiba.
