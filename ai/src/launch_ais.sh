#!/bin/bash

PORT=4242
SCRIPT_PATH="./zappy_ai.py"

# Lancer 5 IA pour TeamA
for i in {1..10}; do
    echo "Lancement TeamA instance $i..."
    $SCRIPT_PATH -p $PORT -n TeamA &
done

# Lancer 5 IA pour TeamB
for i in {1..10}; do
    echo "Lancement TeamB instance $i..."
    $SCRIPT_PATH -p $PORT -n TeamB &
done

# Lancer 5 IA pour TeamB
for i in {1..10}; do
    echo "Lancement TeamC instance $i..."
    $SCRIPT_PATH -p $PORT -n TeamC &
done

echo "Toutes les IA ont été lancées."
