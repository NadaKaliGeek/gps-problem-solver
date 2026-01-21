#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//definir un nouveau type string qui est un tableau de 50 carract�res
typedef char string[50];
typedef struct{
    string action; //nom de l'action
    string preconds[10]; //liste des preconditions
    int preconds_count; //nombre des preconditions
    string add[10]; //liste des faits a ajouter
    int add_count; //nombre des faits a ajouter
    string deleete[10]; // liste des faits a supprimer
    int deleete_count; //nombre des faits a supprimer
} Rule; //structure qui represente une regle
//structure qui representeun etat (pour le backracking)
typedef struct {
    string facts[10]; //liste des faits actuels dans cet etat
    int facts_count; //nombre des faits
    int rule_index; //index de la regle ayant g�n�r� cet etat
} State;
//variables globales
Rule rules[100]; // tableau de regles de type Rule
int Rule_count = 0;
string initial_facts[10]; //faits de d�part pour pouvoir les r�initialiser
int initial_facts_count = 0;
string facts[10]; //faits actuels
int facts_count = 0;
string finish[10]; //objectifs finals � atteindre
int finish_count = 0;
State states[100]; //les etats explor�s (backtracking)
int state_count = 0;
State visited_states[1000]; //etats deja visit�s pour eviter les boucles
int visited_states_count = 0;

//fonction qui parse une ligne du fichier et extrait les �l�ments entre les virgules
int ParseLine(char source[], string cible[]){
    int i=0, n=0;
    while(source[i]!=':')i++; //avance jusqu'a trouver :
    i++; //passer :
    int j=i; //j sera le debut du prochain element a extraire
    while(source[i]!='\n'){ //tant que ce n'est pas la fin de la ligne
        if(source[i]==','){ //si on trouve une virgule
            memcpy(&cible[n], &source[j], i-j); //copier l'element trouv� de source � cible
            n++; //incrementer n qui est le nombre d'elements copi�s(extraits)
            j=i+1; //passer au prochain element
        }
        i++;
    }
    return n; //retourner le nombre d'elements extraits
    }
//fonction du chargement du fichier probleme
void load_problem(string fileName){
    FILE* f=fopen(fileName, "r"); //ouvrir le fichier en mode lecture
    char s[1000]; //tableau temporaire pour lire chaque ligne
    if(f!=NULL){ //v�rifier si l'ouverture du fichier a r�ussi
        printf("Lecture du ficier %s \n", fileName);
        fgets(s,1000,f); // lire la ligne depuis le fichier f et la stocke dans s
        facts_count=ParseLine(s, facts); //utilise la fonction parseline pour determiner le nombre de faits lu
        initial_facts_count = facts_count; //sauvegarder le nombre initial des faits
        int i;
        for (i = 0; i < facts_count; i++) {
            strcpy(initial_facts[i], facts[i]); //copier les faits de facts dans initial facts pour pouvoir r�initialiser plus tard
        }

        fgets(s,1000,f); //lecture une nouvelle ligne du fichier f
        finish_count=ParseLine(s,finish); //r�cup�rer le nombre de finish lu
        while(fgets(s,1000,f)!=NULL){  //lecture des regles
            fgets(s,1000,f);
            string temp[]={""}; //Cr�er un tableau temporaire temp pour stocker l'action apr�s l'avoir "d�coup�e".
            ParseLine(s,temp); //D�coupe ce qu'il y a dans s et remplit temp[0] avec le nom de l'action.
            strcpy(rules[Rule_count].action,temp[0]);//Copier l'action trouv�e dans la structure rules[Rule_count].action.
            fgets(s,1000,f); //lire la prochaine ligne qui contient les pr�conds
            rules[Rule_count].preconds_count=ParseLine(s, rules[Rule_count].preconds); //stocker dans rules[Rule_count].preconds_count le nombre de preconds trouv�s
            fgets(s,1000,f); //lire la ligne suivante qui contient les add
            rules[Rule_count].add_count=ParseLine(s, rules[Rule_count].add); //stocker le nombre de add trouv�s
            fgets(s,1000,f); //likre la ligne suivante qui contient les delete
            rules[Rule_count].deleete_count=ParseLine(s,rules[Rule_count].deleete); //stocker le nombre de delete trouv�s
            Rule_count++; //passer a la prochaine regle
        }
        fclose(f);
    }
}

//fonction de r�initialisation
void reset_facts() {
    facts_count = initial_facts_count; //remettre le nombre de faits au nombre initial pour recommencer
    int i;
    for (i = 0; i < facts_count; i++) {
        strcpy(facts[i], initial_facts[i]); //copier tout ce qu'il ya dans le tab initial_facts au tab facts
    }
}

// chainage avant simple
//fonction qui verifie si un fait est present dans les faits actuels
int is_in_facts(string fact) {
    int i;
    for (i = 0; i < facts_count; i++) {
        if (strcmp(facts[i], fact) == 0) // on cherche si un fait particulier existe d�j� dans la liste facts.
        //strcmp verifie si deux chaines de caracteres sont identiques
            return 1; //s'ils sont identiques (==0) donc le fait est trouv� et on retourne 1
    }
    return 0; //fait non trouv� donc on retourne 0
}
//fonction qui verifie si une regle est applicable(toutes les preconds doivent etre presents)
int is_rule_applicable(Rule rule) {
    int i;
    for (i = 0; i < rule.preconds_count; i++) {
        if (!is_in_facts(rule.preconds[i])) {
            return 0; //une precondition manque donc regle pas applicable
        }
    }
    return 1; //toutes les proconds sont present donc regle applicable
}
//fonction qui applique une regle
void apply_rule(Rule rule) {
    int i;
    // dabbord on supprime des faits
    for (i = 0; i < rule.deleete_count; i++) {
        int j = 0;
        while (j < facts_count) {
            if (strcmp(facts[j], rule.deleete[i]) == 0) {
                 // verifie si le delete est dans facts,
                // si oui, elle ecrase le delete dans facts en decalanat toutes les cases du tableau vers la gauche(ecraser)
                int k;
                for (k = j; k < facts_count - 1; k++) {
                    strcpy(facts[k], facts[k+1]); //copier tout les faits de la case d'apres a la case d'avant (pour ecraser et decaler le tableau d'une case a gauche
                }
                facts_count--; //diminuer le nombre de faits par 1 car on vient d'ecraser un fait
                // On incremente pas j car on doit checker le nouvel element dans la case j
            } else {
                j++; //sinon passer au fait suivant
            }
        }
    }

    // Mtn ajouter les nouveaux faits
    for (i = 0; i < rule.add_count; i++) {
        if (!is_in_facts(rule.add[i])) { //si le fait n'est pas deja present dans add
            strcpy(facts[facts_count], rule.add[i]); //copier rule.add[i] dans la derniere case de facts
            facts_count++;
        }
    }
}
//fonction qui verifie si tout les objectifs sont atteints
int are_goals_satisfied() {
    int i;
    for (i = 0; i < finish_count; i++) {
        if (!is_in_facts(finish[i])) //un objectifs n'est pas atteint
            return 0;
    }
    return 1; //tout les objectifs sont atteint
}

int forward_chaining() {
    int max_iterations = 100, iterations = 0, rule_applied;

    printf("\nExecution de l'algorithme de chainage avant simple :\n");

    while (!are_goals_satisfied()) {
        rule_applied = 0; //on part du fait qu'aucune regle n'a �t� appliqu�
        int i;
        for (i = 0; i < Rule_count; i++) {
            if (is_rule_applicable(rules[i])) {
                apply_rule(rules[i]); //appliquer la premiere regle
                rule_applied = 1; //=1 lorsque la regle est appliqu�
                break; //One at a time
            }
        }
        if (!rule_applied) {
            printf("Aucune regle applicable trouv�\n");
            return 0;
        }
        iterations++;
        if (iterations > max_iterations) { //limiter le nombre d'iterations car max_iterations=100
            printf("Nombre maximal d iterations atteint. Arret du programme.");
            return 0;
        }
    }
    return 1;
}

//fonctions du chainage avant avec retour arriere
//fonction qui verifie si un fait est dans un etat
int is_fact_in_state(string fact, State state) {
    int i;
    for (i = 0; i < state.facts_count; i++) {
        if (strcmp(state.facts[i], fact) == 0) //verifie s'ils sont identiques
            return 1; //le fait est dans un etat
    }
    return 0; //le fait n'est pas dans l'etat
}
//fonction qui verifie si une regle est appliquable a un etat donn�
int is_rule_applicable_for_state(Rule rule, State state) {
    int i;
    for (i = 0; i < rule.preconds_count; i++) { //verifie si chaque precondition est satisfaite
        if (!is_fact_in_state(rule.preconds[i], state))
            return 0; //y'a pas un seul fait identique a state
    }
    return 1;//il y'en a
}
//fonction qui applique une regle a un nouvel etat
State apply_rule_to_new_state(int rule_idx, State prev_state) {
    Rule rule = rules[rule_idx];
    State new_state;

    //donner le nombre de faits de l'etat precedent au nombre de faits du nouvel etat
    new_state.facts_count = prev_state.facts_count;
    int i;
    for (i = 0; i < prev_state.facts_count; i++) {
        strcpy(new_state.facts[i], prev_state.facts[i]);// Copier les faits de letat precedent dans le nouvel etat

    }

    //supprimer les faits
    for (i = 0; i < rule.deleete_count; i++) {
        int j = 0;
        while (j < new_state.facts_count) {
            if (strcmp(new_state.facts[j], rule.deleete[i]) == 0) { //verifier s'ils sont identiques
                int k;
                for (k = j; k < new_state.facts_count - 1; k++) {
                    strcpy(new_state.facts[k], new_state.facts[k+1]);
                } //decale tout les faits du tableau a gauche en ecrasant le fait
                new_state.facts_count--; //decrementer de 1 car on vient d'ecraser un fait
                //on incremente pas j ici car on doit checker le nouvel element dans la case j
            } else {
                j++; //sinon on passe au fait suivant
            }
        }
    }

    // Ajouter les nouveaux faits
    for (i = 0; i < rule.add_count; i++) {
        if (!is_fact_in_state(rule.add[i], new_state)) {
            strcpy(new_state.facts[new_state.facts_count], rule.add[i]); //copier les add dans la derniere case du teableau de facts de new state
            new_state.facts_count++;
        }
    }

    new_state.rule_index = rule_idx; //enregistre l'index de la regle applique pour l'affichage de la solution apres

    return new_state;
}
//fonction qui verifie si tout les objectifs sont satisfaits dans un etat donn�
int are_goals_satisfied_for_state(string goals[], int goals_count, State state) {
    int i;
    for (i = 0; i < goals_count; i++) {
        if (!is_fact_in_state(goals[i], state)) //si un objectif n'est pas present
            return 0; //on continue
    }
    return 1; //sinon tout les objectifs sont atteints
}
//fonction qui verifie si deux etats sont identiques en les comparants
int are_states_equal(State state1, State state2) {
    if (state1.facts_count != state2.facts_count) //verifier s'ils ont le meme nombre de faits
        return 0;  //ils ne sont pas egaux donc pas identiques
    int i, j;
    for (i = 0; i < state1.facts_count; i++) {
        int found = 0;
        for (j = 0; j < state2.facts_count; j++) {
            if (strcmp(state1.facts[i], state2.facts[j]) == 0) { //verifier si les faits des deux etats sont identiques
                found = 1; //les deux etats sont identiques
                break; //one at a time
            }
        }
        if (!found)
            return 0; //les deux etats ne sont pas identiques
    }

    return 1;
}
//fonction qui verifie si un etat a deja ete visit� pour eviter les cycles dans la recherche
int is_state_visited(State state) {
    int i;
    for (i = 0; i < visited_states_count; i++) {
        if (are_states_equal(state, visited_states[i])) //utilise la fonction precedente pour verifier si les etats sont egaux
            return 1; //l'etat a deja ete visit�
    }
    return 0; //l'etat n'a pas ete visit�
}
//fonction qui marque un �tat comme visit�
void mark_state_as_visited(State state) {
    if (visited_states_count < 1000) {
        int i;
        for (i = 0; i < state.facts_count; i++) {
            strcpy(visited_states[visited_states_count].facts[i], state.facts[i]); //copier un fait de l'etat dans les faits des etats visit�s
        }
        visited_states[visited_states_count].facts_count = state.facts_count; //donner le nombre des faits de l'etat au nombre des faits des etats visit�s
        visited_states[visited_states_count].rule_index = state.rule_index; //donner l'index de la regle de l'etat aux index de la regle des etats visit�s
        visited_states_count++;
    }
}
//fonction du chainage avant avec retour arriere
int forward_chaining_with_backtrack() {
    int max_iterations = 100, iterations = 0, IR = 0, possible = 1;
//max_iterations=100 : limiter 100 essais pour eviter les boucles infinies
//iterations=0 : compteur pour compter dans la boucle
//IR c'est l'index de regle a tester
    printf("\nExecution du programme avec le chainage avant avec retour arriere...\n");
//on commence avec l'etat initial
    state_count = 1;
    states[0].facts_count = facts_count; //Le nombre de faits de l'�tat initial = le nombre de faits actuel.
    int i;
    for (i = 0; i < facts_count; i++) {
        strcpy(states[0].facts[i], facts[i]); //copier tout les faits existants dans la premiere case de states
    }
    states[0].rule_index = -1; //aucune regle n'a ete apploiqu�e pour arriver � letat initial
    visited_states_count = 0; //remettre a 0 le compteur des etats visit�s
    mark_state_as_visited(states[0]); //on marque l'etat initial comme deja visit� car on vient de le visiter

    while (!are_goals_satisfied_for_state(finish, finish_count, states[state_count-1]) && possible) { //tant que on a pas atteint les objectifs et c'est toujours possible de continuer
        int rule_found = 0; //variable qui va nous permettre de voir si on a trouver une regle appliquavle (on assume que non au debut)
        for (i = IR; i < Rule_count; i++) { //boucle pour tester toutes les regles
            if (is_rule_applicable_for_state(rules[i], states[state_count-1])) { //si la regle i peut etre appliqu�e sur l'etat actuel
                State temp_state = apply_rule_to_new_state(i, states[state_count-1]); //on applique la regle i et on la met dans un nouvel etat temporaire
                if (!is_state_visited(temp_state)) { //verifier si l'etat n'a jamais ete visit�
                    states[state_count] = temp_state; //on ajoute ce nouvel etat dans la liste detats
                    mark_state_as_visited(states[state_count]); //on le maarque comme visit�

                    state_count++; //et on passe a un nouvel etat
                    IR = 0; //on reinitialise ir a 0 pour la prochaine noucle
                    rule_found = 1; //et on note qu'on a trouv� une regle appliquable
                    break; //on arrete la boucle de recherche de regle car on a trouv� une qui marche
                }
            }
        }
        if (!rule_found) { //si aucune regle n'etait applicable
            if (state_count > 1) { //si on nest pas a l'etat initial
                IR = states[state_count-1].rule_index + 1; //on met IR pour tester la regle suivante
                state_count--; //on revient a l'etat precedent
            } else { //si on est deja a l'etat initial
                possible = 0;
                printf("No solution possible.\n"); //plus de solutions possible
            }
        }
        iterations++; //psk on a fait une iteration de plus
        if (iterations > max_iterations) { //si on a depass� 100 it�rations
            printf("Maximum iterations reached. Stopping.\n");
            return 0; //on arrete le programme pour eviter une boucle infinie
        }
    }
    if (are_goals_satisfied_for_state(finish, finish_count, states[state_count-1])) { //si les objectifs sont atteints dans le dernier etat
        facts_count = states[state_count-1].facts_count; //m�j facts count pour correspondre aux faits du dernier etat
        for (i = 0; i < facts_count; i++) {
            strcpy(facts[i], states[state_count-1].facts[i]); //copier tout les faits du dernier etat dans les faits globaux
        }
        return 1; //on a atteint les objectifs
    }
    return 0; //sinon echec
}

int main() {
    load_problem("school.txt"); //charger le probleme

        int simple_success = forward_chaining(); //lancer le chainage avant simple et le stocker dans simple_success
        if (simple_success) //on affiche celon si le chainage simple a reussi ou pas
            printf("\nProbleme resolu avec succes grace au chainage avant simple\n");
        else
            printf("\nEchec de la resolution du probleme avec le chainage avant simple\n");
// remettre les faits comme au tout d�but (avant le simple chainage) sinon les faits auraient ete modifies et ce ne serait plus le meme probleme
        reset_facts();

       //on lance la fonction forward_chaining_with_backtrack et le resultat est stock� dans backtrack_success
        int backtrack_success = forward_chaining_with_backtrack();
        if (backtrack_success) { //si c reussi on affiche que c'est resolu et on affiche le chemin suivi
            printf("\nProbleme resolu avec succes grace au chainage avec retour en arriere\n");
            printf("\nSolution :\n");
            int j;
            for (j = 1; j < state_count; j++) {//on parcourt tout les etats et pour chaque etat on affiche quelle regle a ete appliqu�
                printf("Etape %d: %s\n", j, rules[states[j].rule_index].action);
            }
        } else {
            printf("\nEchec de la resolution du probleme  avec le chainage avec retour en arriere\n"); //sinon on affiche que meme avec le retour arriere ca ne marche pas
        }
//on affiche pour chaque methode si elle a reussi ou pas
        printf("\nVerification :\n");
        printf("Chainage avant simple: %s\n", simple_success ? "Succes" : "Echec");
        printf("Chainage avant avec retour arriere: %s\n", backtrack_success ? "Success" : "Echec");
    return 0;
}
