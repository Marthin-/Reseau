#include "Include/Define.h"

pecheur_default joueur_d;
pont_default pont_d;
objet **grille;
objet **grille_adverse;
pecheurs *joueurs;
espece_default *espece_d;
int **peut_manger;
int **graphe;
clr *tab_clr;
int TAILLE;
int Nb_objet=0;
int fini=0;
int Nb_vivant=0;
int Nb_tour=0;
int Nb_Joueur_tour=0;
int nb_affiche_graphe;
int Nb_joueur=-1;
int taille_cadre_X;
int taille_cadre_Y;
SDL_Surface *ecran=NULL;
int err_flag=0;
int cpt_peut_marcher;
char erreur[5][20];
int fils;
int temps_victoire;
int victoire;
int signal_victoire_recu=0;
int tableau_comp[9];
pthread_t compteur_temps;
pthread_t reception_multi;


/*
   Alarme qui permet de kill le fils qui affiche l'ecran d'acceuil si
   l'utilisateur fait CTRL+C durant le choix du nombre de joueur
 */

void lancement_threads(int* tableau_competences){
	printf("lancement_threads 16\n");
	pthread_create((pthread_t*) &compteur_temps, NULL, (void*)compteur_competence(tableau_competences), tableau_competences);
	pthread_create((pthread_t*) &reception_multi, NULL,(void*) serveur(), NULL);
	printf("lancement_threads succes (17)\n");
}

void INTHandler(){
	kill(fils,SIGKILL);
}

/*
   Cette fonction va remplir les données utilisables par le graphique en mode information
   et il va détecter le cas de fin : Il n'existe plus de poisson pechable.
 */
void compte_grille(){
	printf("debut compte_grille 12\n");
	int i,j;
	int nb_peut_manger=0;
	/*
	   Compte le nombre de poisson que peut pecher le joueur.
	 */
	for(i=0;i<Nb_vivant;i++){
		if(peut_manger[0][i+2])			nb_peut_manger++;
	}
	/*
	 */

	/*===============================
	//	for(i=0;i<Nb_vivant+1;i++){
	//		for(j=0;j<nb_affiche_graphe;j++){
	//			if(j<nb_affiche_graphe-1)
	//				graphe[i][j]=graphe[i][j+1];
	//			else if (j==nb_affiche_graphe-1)
	//				graphe[i][j]=0;
	//		}
	//	}
	//==============================*/
	/*
	   Initialise chaque case de façon à ce que les animaux puissent de nouveau
	   manger et bouger.Compte le nombre de poisson présent dans la grille pour
	   chaque espece.
	 */
	for(i=0;i<TAILLE;i++){
		for(j=0;j<TAILLE;j++){
			grille[i][j].a_bouger=NON;
			grille[i][j].a_manger=NON;
			if(grille[i][j].est_vide!=1 && grille[i][j].nom>1){
				graphe[grille[i][j].nom-2][nb_affiche_graphe-1]++;
				graphe[Nb_vivant][nb_affiche_graphe-1]++;
			}
		}
	}
	/*
	   Si une espece pechable par le joueur a disparu,on décrement le nombre d'espece
	   pechable restant.
	 */
	for(i=0;i<Nb_vivant;i++)
		if(graphe[i][nb_affiche_graphe-1]==0 && peut_manger[0][i+2])
			nb_peut_manger--;
	/*
	   Si il ne reste plus rien a pecher, on arrete le jeu.
	 */
	if(nb_peut_manger==0){
		fini=2 ;
	}
	printf("fin compte_grille 13\n");
}
/*
   Execute chaques étapes du cycle
 */
void cycle(int i,int j){
	Survie(&grille[i][j]);
	Reproduction(&grille[i][j],i,j);
	Predation(&grille[i][j],i,j);
	Deplacement(&grille[i][j],i,j);
	Tour(&grille[i][j]);
}

int main(int argc,char *argv[])
{
	int i, j_gagner=0,go=0;
	victoire=0;
	int Joueur_courant=1,mode=Jeux,done=0,x=0,y=0;
	SDL_Event event;
	for(i=0;i<9;i++){
		tableau_comp[i]=0;
	}
	/*
	   Vérifie le nombre d'argument
	 */
	if(argc!=1){
		printf("Usage : %s\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	/*
	   Affiche message bienvenue
	 */
	/*
	   On crée un processus qui va afficher l'écran d'accueil
	 */
	fils=fork();
	/*
	   Pour kill le fils en cas de CTRL+C,on arme une alarme
	 */
	signal(SIGINT,INTHandler);

	switch(fils){
		case -1:
			exit(EXIT_FAILURE);
			break;
		case 0 :
			/*
			   Le fils lance la musique d'acceuil puis
			   affiche l'ecran d'accueil
			 */
			charger_musique_accueil();
			afficher_accueil_SDL();
			break;
		default :
			/*
			   Initialise le jeu
			 */
			srand(time(NULL));
			mode=nombre_de_joueur(Nb_joueur);
			if(mode==-1){
				kill(fils,SIGKILL);
				exit(EXIT_FAILURE);
			}
			charger_config();
			printf("post config\n");
			charger_ecosystem();
			printf("post eco\n");
			creation_SDL();
			printf("post creation\n");
			compte_grille();
			printf("post compte\n");
			printf("kill fils 14\n");
			kill(fils,SIGKILL);
			printf("fils killed 15\n");
			lancement_threads(tableau_comp);
			printf("post threads\n");
			break;
	}
	/*
	   Termine la musique d'accueil et commence celle de fond si
	   l'utilisateur le veux(configuration.txt)
	 */
	terminer_musique_accueil();
	printf("terminaison terminer_musique_accueil 18\n");
	charger_musique_fond();
	printf("musique fond chargee 19\n");
	afficher_SDL(mode,Joueur_courant);
	printf("SDL affichee 20\n");


	cpt_peut_marcher=joueur_d.nb_depl_par_tour;

	while (!fini)
	{
		/*
		   Si un message d'erreur doit etre afficher
		 */
		if(err_flag)
			afficher_SDL(mode,Joueur_courant);
		/*
		   Si une action a été faite hors déplacement,et que ça n'a pas encore été 
		   affiché.
		 */
		if(done){
			afficher_SDL(mode,Joueur_courant);
			go=1;
		}
		if(go){
			/*
			   Reinitialise le nombre de tour
			 */
			cpt_peut_marcher=joueur_d.nb_depl_par_tour;
			/*
			   Si le joueur a pecher de nouveaux poissons,on affiche le sac pendant 1sec
			 */
			if(Nb_joueur>0 && joueurs[Joueur_courant-1].new_poisson){
				afficher_SDL(Sac,Joueur_courant);
				sleep(1);
				joueurs[Joueur_courant-1].new_poisson=0;
			}
			done=0;
			go=0;
			/*
			   Fait tourner l'écosysteme sur 1 tour.
			 */
			for(x=0;x<TAILLE;x++){
				for(y=0;y<TAILLE;y++){
					cycle(x,y);
				}
			}
			compte_grille();
			victoire = condition_victoire(joueurs[Joueur_courant-1], temps_victoire,victoire);	
			Nb_tour++;
			/*
			   Si il existe des joueurs on augmente leur tour
			 */
			if(mode==Jeux && Nb_joueur>0)
				Nb_Joueur_tour++;
			/*
			   On passe au joueur suivant et si le joueur nage, on augmente la valeur de peut_nager
			 */
			if(Nb_joueur>=1 && mode!=Information){
				Joueur_courant++;
				if(Joueur_courant>Nb_joueur)
					Joueur_courant=1;

				/*	if(joueurs[Joueur_courant-1].nage && joueurs[Joueur_courant-1].peut_nager<2)
					joueurs[Joueur_courant-1].peut_nager++;
				 */					
			}
			/*
			   Puis on affiche la nouvelle map
			 */
			afficher_SDL(mode,Joueur_courant);

		}
		/*
		   Si il n'y pas de joueur ,tout ce que l'on attend c'est d'appuyer sur la touche
		   entrée pour faire passé un tour ,donc on attend pas que la touche soit relaché
		   =>SDL_POLL_EVENT, sinon on attend.On fait cela afin de rendre le jeu plus
		   fluide lors de l'utilisation sans joueurs.
		 */
		if(Nb_joueur>0)
			SDL_WaitEvent (&event);
		else
			SDL_PollEvent (&event);
		switch(event.type)
		{
			/*
			   Si on change la taille de l'écran,on modifie l'affichage
			 */
			case SDL_VIDEORESIZE:
				taille_cadre_X=event.resize.w;
				taille_cadre_Y=event.resize.h;
				ecran = SDL_SetVideoMode(event.resize.w, event.resize.h, 32,SDL_HWSURFACE | SDL_RESIZABLE);
				afficher_SDL(mode,Joueur_courant);
				break;
				/*
				   Si on quitte le jeu,on signale que le jeu est fini
				 */
			case SDL_QUIT:
				fini = 1;
				break;
			case SDL_MOUSEMOTION:
				x=event.motion.x;
				y=event.motion.y;
				break;
				/*
				   Si on utilise la souri
				 */
			case SDL_MOUSEBUTTONDOWN:
				x=event.motion.x;
				y=event.motion.y;
				switch(event.button.button){
					/*
					   CLICK GAUCHE=>lancer un poisson
					 */
					case SDL_BUTTON_LEFT:
						if(!done && Nb_joueur>0 && !joueurs[Joueur_courant-1].nage)
							done=lancer(Joueur_courant,x,y);
						break;
					case SDL_BUTTON_RIGHT:
						/*TODO menu déroulant*/
						break;
					default:
						break;
				}
				/*
				   Si on appuie sur une touche
				 */
			case SDL_KEYDOWN:
				/*
				   Si on appuie en meme temps sur Maj,SHIFT Gauche
				 */
				switch (event.key.keysym.sym)
				{
					/*
					   Si on appuie sur i,on affiche le graphique et les animaux sous l'eau
					 */
					case SDLK_i:
						if(mode!=Information){
							mode=Information;
						}
						else if (Nb_joueur>0)
						{
							mode=Jeux;
							afficher_SDL(mode,Joueur_courant);
						}
						break;
					default:
						break;
				}
		}
		switch (event.key.keysym.sym)
		{
			/*
			   touche R => redémarre la carte
			 */
			case SDLK_r:
				err_flag=0;
				Nb_tour=0;
				Nb_Joueur_tour=0;
				fini=0;
				reinitialise();
				charger_ecosystem();
				compte_grille();
				afficher_SDL(mode,Joueur_courant);
				break;
				/*
				   touche S => affiche le sac
				 */
			case SDLK_s:
				if(mode!=Sac && Nb_joueur>0){
					mode=Sac;
				}
				else if (Nb_joueur>0)
				{mode=Jeux;}
				afficher_SDL(mode,Joueur_courant);
				break;
				/*
				   touche C => peche canne
				 */	 
			case SDLK_c:
				if(!done && Nb_joueur>0 && !joueurs[Joueur_courant-1].nage)
				{done=Pecher(Joueur_courant,x,y,CANNE);}
				break;
				/*
				   touche F => peche filet
				 */
			case SDLK_f:
				if(!done && Nb_joueur>0 && !joueurs[Joueur_courant-1].nage)
					done=Pecher(Joueur_courant,x,y,FILET);
				break;
				/*
				   touche ESC => fini le jeu
				 */
			case SDLK_ESCAPE:
				fini = 1;
				break;
				/*
				   touche Entree => passe au tour suivant
				 */	                
			case SDLK_RETURN:
				if(mode!=Information)
					mode=Jeux;
				go=1;
				break;

				/* Fonctions de spawn*/
			case SDLK_F1:

				break;

			case SDLK_F2:
				spawn(grille,4,joueurs[Joueur_courant-1],mode,(int*)tableau_comp,Nb_tour);
				break;

			case SDLK_F3:
				spawn(grille,5,joueurs[Joueur_courant-1],mode,(int*)tableau_comp,Nb_tour);
				break;

			case SDLK_F4:
				if (joueurs[Joueur_courant-1].XP > 20){
					spawn(grille,3,joueurs[Joueur_courant-1],mode,(int*)tableau_comp,Nb_tour);
				}
				break;

			case SDLK_F5:
				if (joueurs[Joueur_courant-1].XP > 35){
					spawn(grille,7,joueurs[Joueur_courant-1],mode,(int*)tableau_comp,Nb_tour);
				}
				break;

			case SDLK_F6:
				if (joueurs[Joueur_courant-1].XP > 35){
					spawn(grille,8,joueurs[Joueur_courant-1],mode,(int*)tableau_comp,Nb_tour);
				}
				break;

			case SDLK_F7:
				if (joueurs[Joueur_courant-1].XP > 35){
					spawn(grille,9,joueurs[Joueur_courant-1],mode,(int*)tableau_comp,Nb_tour);
				}
				break;

			case SDLK_F8:
				if (joueurs[Joueur_courant-1].XP > 35){
					spawn(grille,10,joueurs[Joueur_courant-1],mode,(int*)tableau_comp,Nb_tour);
				}
				break;

			case SDLK_F9:
				if (joueurs[Joueur_courant-1].XP > 35){
					SDL_envoyer_predateur();
				}
				break;
			default:
				break;
		}
	}



	if (victoire==1 || signal_victoire_recu ==1 ){
		fini=1;	
		/*TODO
		  envoyer en broadcast que le joueur a gagné
		  ou recevoir le vainqueur et afficher le vainqueur*/
	}


	/*Affiche le gagnant si il existe*/
	if(Nb_joueur>0 && j_gagner!=0) {
		afficher_gagnant_SDL(j_gagner);
	}
	/*
	   Libere la mémoire,l'écran,termine la musique de fond
	   si elle existe et quitte SDL
	 */
	fin();
	terminer_musique_fond();
	SDL_FreeSurface(ecran);
	SDL_Quit();
	return EXIT_SUCCESS;
}
