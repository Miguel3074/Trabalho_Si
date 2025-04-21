# Comparação de Algoritmos para o Problema do Caixeiro Viajante (TSP)

Este projeto foi desenvolvido como parte da disciplina de **Sistemas Inteligentes** da UTFPR. O objetivo é aplicar e comparar técnicas heurísticas na resolução do Problema do Caixeiro Viajante (TSP), utilizando:

- **Têmpera Simulada (Simulated Annealing - SA)**
- **Algoritmo Genético (Genetic Algorithm - GA)**

---

## Objetivos

- Implementar os algoritmos SA e GA em C++.
- Resolver instâncias do TSP lidas a partir de arquivos CSV.
- Avaliar e comparar o desempenho de cada abordagem.
- Gerar visualizações gráficas das rotas e da evolução do custo.

---

## Execução

Utilize um compilador com suporte ao padrão C++11 ou superior:

Os parâmetros de cada algoritmo podem ser modificados diretamente na função main().


## Saídas Geradas
- rota_final.svg — Visualização da rota final.

- grafico_custo.svg — Evolução do custo da rota (SA).

- grafico_temperatura.svg — Evolução da temperatura (SA).

- cidades.svg — Visualização apenas dos pontos das cidades.

- resultados TS e AG, nos arquivos em formato excel. 

## Parametrização
# Têmpera Simulada (SA)
- T_inicial — Temperatura inicial

- T_final — Temperatura final

- alpha — Fator de resfriamento

# Algoritmo Genético (GA)
- G — Número de gerações

- P — Tamanho da população

- TM — Taxa de mutação

Todos os parâmetros podem ser ajustados no início da função main().
---
## Observações
Os arquivos SVG podem atingir tamanhos elevados em execuções com muitas cidades ou histórico detalhado.

Para evitar problemas ao versionar arquivos grandes, utilize o Git LFS.

Recomendamos abrir os arquivos SVG com softwares apropriados, como navegadores modernos ou ferramentas de edição vetorial.

## Autores
Miguel dos Anjos Brack — @Miguel3074

Renzo Tognella de Rosa

Licença
Uso acadêmico. Projeto desenvolvido para a disciplina de Sistemas Inteligentes — UTFPR.
