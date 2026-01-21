# GPS Problem Solver (C)

Ce projet implémente un **Générateur de Plans (GPS)** en langage C.  
À partir d’un état initial (faits), d’objectifs et d’un ensemble de règles (préconditions / add / delete), le programme génère automatiquement une **séquence d’actions** permettant d’atteindre les objectifs si une solution existe.

Deux méthodes de résolution sont proposées :
- le **chaînage avant simple**,
- le **chaînage avant avec retour arrière (backtracking)**.

## Contenu
- `main.c` : implémentation du moteur GPS
- `school.txt` : exemple de problème

## Compilation

### Linux / macOS
```bash
gcc main.c -o main
./main
