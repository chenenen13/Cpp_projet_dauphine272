# Trading Platform C++17

## Présentation du projet

Ce projet implémente une plateforme de trading simulée en C++17 couvrant le cycle complet d’un trade :

Signal → Exécution → Analyse post-trade

Modules :
- MarketDataFeed
- OrderBook
- MatchingEngine
- StrategyEngine
- Portfolio / Risk Manager
- Reporting

## Architecture

- Le `MarketDataFeed` lit un fichier CSV et émet des événements `OrderAdd` et `OrderRemove`
- L’`OrderBook` maintient l’état du carnet et génère les snapshots `MarketData`
- Le `StrategyEngine` reçoit les snapshots et produit des signaux BUY / SELL / HOLD
- Le `MatchingEngine` tente d’exécuter les ordres de stratégie
- Le `Portfolio` gère les positions, le cash, le P&L et le contrôle pré-trade
- Le `Reporting` affiche les métriques finales

## Événements

### Feed
- `OrderAdd(timestamp, order_id, side, price, quantity)`
- `OrderRemove(timestamp, order_id)`

### MarketData
Généré par l’OrderBook :
- `timestamp`
- `best_bid`
- `best_ask`
- `last_price`
- `volume`

## Stratégies implémentées

### 1. MomentumStrategy
- compare une moyenne mobile courte à une moyenne mobile longue
- BUY si fast MA > slow MA + epsilon
- SELL si fast MA < slow MA - epsilon

### 2. MeanReversionStrategy
- calcule un z-score sur une fenêtre glissante
- BUY si le prix est trop bas par rapport à la moyenne
- SELL si le prix est trop haut

## Build

```bash
mkdir build
cd build
cmake ..
make