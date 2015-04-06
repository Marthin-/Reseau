#include "Include/Define.h"

int condition_victoire(pecheurs joueur, int temps_victoire,int victoire){

	if(joueur.nombre_de_poissons > 20 && joueur.nombre_planctons > 6 && joueur.nombre_herbivores > 6 && joueur.nombre_predateurs > 6){
		if (temps_victoire<20){
			temps_victoire+=2;
		}

		if (temps_victoire >= 20 && joueur.nombre_de_poissons > 20 && joueur.nombre_planctons > 6 && joueur.nombre_herbivores > 6 && joueur.nombre_predateurs > 6){
			victoire=1;
		}
		else {
			temps_victoire=0;
		}
	}
	return victoire;
}

void* compteur_competence(int* tableau_comp){
	while(1){
		sleep(1);
		int i;
		printf("competence : %i\n",tableau_comp[1]);
		for(i=0;i<10;i++){
			tableau_comp[i]++;
		}
		printf("competence : %i\n",tableau_comp[1]);
		return NULL;
	}	
}

void* multijoueur(){
	void serveur();
	return NULL;
}

void serveur(){}

void* envoyer_predateur(){
	return NULL;
}

void* SDL_demander_joueur(){
printf("\n");
	return NULL;
}
