#include "Define.h"

extern objet **grille_adverse;
/* On récupère la compétence (spawn bar, thon...)->touches <f1> à <f9>
 *  On on teste si la compétence est disponible (cooldown)
 *  on teste si la case est vide
 *  On verifie la visibilité (SDL)
 *  On fait spawner le poisson (comme une naissance) sur la case visée
 *  On incrémente le nombre de poissons du pêcheur et le compteur correspondant (predateur, herbivore, plancton)
 *  On reset le cooldown
 *  On actualise la carte et le tableau de vue
 *  Le poisson bougera au prochain déplacement
 */
int generation_poisson(int type,int Nb_tour){
	/*initialise le poisson*/
	objet a;
	int type_poisson;
	vide(&a);
	a.nom=type;
	a.satiete=4;
	a.dernier_repas=Nb_tour;
	a.derniere_reproduction=Nb_tour;
	a.est_vivant=OUI;
	a.est_vide=NON;
	/*renvoie le type de poisson (predateur, herbivore...)*/
	if(type==2){
		type_poisson=1;
	}
	else if (type == 3 || type == 4 || type == 5 || type == 10){
		type_poisson=2;
	}
	else if (type == 7 || type == 8 || type == 9){
		type_poisson=3;
	}

	return type_poisson;

}


objet** spawn(objet** grille, int type, pecheurs joueur, int mode, int* tableau_comp,int Nb_tour){
	int type_poisson;
	if (tableau_comp[type]>=10){
		type_poisson = generation_poisson(type,Nb_tour);
		tableau_comp[type]=0;
		joueur.nombre_de_poissons++;
		if(type_poisson == 1)
			joueur.nombre_planctons++;
		else if(type_poisson == 2)
			joueur.nombre_herbivores++;
		else if(type_poisson == 3)
			joueur.nombre_predateurs++;
		afficher_SDL(mode, 1);
	}
	return(objet**) grille;
}

/*TODO*/
int SDL_envoyer_predateur(int mode){
	int joueur = SDL_demander_joueur();
	/*envoyer predateur au joueur*/
	envoyer_predateur(joueur);
	afficher_SDL(mode,1);
	return(EXIT_SUCCESS);
}


void envoyer_map(objet** grille,int joueur){
	/*Envoyer au joueur [joueur] du tableau IP la grille.*/
}

objet** recevoir_map(objet** grille,int joueur){
	/*demander au joueur [joueur] du tableau IP sa grille
	 *recevoir la grille*/
	return (objet**) grille;
}

