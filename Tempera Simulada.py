import json
import math
import random

with open("grafo_500.json", "r") as f:
    cidades = json.load(f)

N = len(cidades)

def distancia(a, b):
    x1, y1 = cidades[a]["x"], cidades[a]["y"]
    x2, y2 = cidades[b]["x"], cidades[b]["y"]
    return math.hypot(x2 - x1, y2 - y1)

def custo_rota(rota):
    return sum(distancia(rota[i], rota[(i + 1) % N]) for i in range(N))

def vizinho(rota):
    nova_rota = rota[:]
    i, j = random.sample(range(N), 2)
    nova_rota[i], nova_rota[j] = nova_rota[j], nova_rota[i]
    return nova_rota

#### TESTE Distancias
distancias = [[distancia(i, j) for j in range(N)] for i in range(N)]

for cidade in cidades[:5]:
    print(cidade)

print(f"\nTotal {N}")
print(distancias[30][410])
print("teste", distancias[0][0])
####

#### TESTE Rota
N = 10
rota_teste = list(range(N))
random.shuffle(rota_teste)

def vizinho(rota):
    nova_rota = rota[:]
    i, j = random.sample(range(N), 2)
    nova_rota[i], nova_rota[j] = nova_rota[j], nova_rota[i]
    return nova_rota

print("Rota", rota_teste)
print("\ncusto", custo_rota(rota_teste))
rota_vizinha = vizinho(rota_teste)
print("Vizinho ", rota_vizinha)
print("\ncusto", custo_rota(rota_vizinha))
####