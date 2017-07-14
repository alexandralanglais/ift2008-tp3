/**
* \file DicoSynonymes.cpp
* \brief Le code des opérateurs du DicoSynonymes.
* \author ?
* \version 0.5
* \date ?
*
* Travail pratique numéro 3.
*
*/

#include "DicoSynonymes.h"
#include "AssertionException.h"

namespace TP3
{





	// Méthode fournie que vous pouvez modifier si vous voulez
	void DicoSynonymes::chargerDicoSynonyme(std::ifstream& fichier)
	{
		ASSERTION(fichier.is_open());

		std::string ligne;
		std::string buffer;
		std::string radical;
		int cat = 1;

		while (std::getline(fichier, ligne)) // tant qu'on peut lire dans le fichier
		{

			if (ligne == "$")
			{
				cat = 3;
				std::getline(fichier, ligne);
			}
			if (cat == 1)
			{
				radical = ligne;
				ajouterRadical(radical);
				cat = 2;
			}
			else if (cat == 2)
			{
				std::stringstream ss(ligne);
				while (ss >> buffer)
					ajouterFlexion(radical, buffer);
				cat = 1;
			}
			else
			{
				std::stringstream ss(ligne);
				ss >> radical;
				ss >> buffer;
				int position = -1;
				ajouterSynonyme(radical, buffer, position);
				while (ss >> buffer)
					ajouterSynonyme(radical, buffer, position);
			}
		}
	}

	//Mettez l'implantation des autres méthodes demandées ici.

	DicoSynonymes::DicoSynonymes()
		:racine(0),nbRadicaux(0),groupesSynonymes()
		{
		}

		DicoSynonymes::DicoSynonymes(std::ifstream &fichier)
		:racine(0),nbRadicaux(0),groupesSynonymes()
		{
			chargerDicoSynonyme(fichier);
		}

		DicoSynonymes::~DicoSynonymes(){
			destructeur_recursif(racine);
		}

	void DicoSynonymes::destructeur_recursif(NoeudDicoSynonymes *& noeud){
		if (noeud != 0) {
			destructeur_recursif(noeud->gauche);
			destructeur_recursif(noeud->droit);
			delete noeud;
			noeud = 0;
		}
	}

	std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const{
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(radical);
		std::vector<std::string> ret_vector;
		for(auto flex : noeud_courant->flexions){
			ret_vector.push_back(flex);
		}


		return ret_vector;
	}


	std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const{
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(radical);
		std::vector<std::string> ret_vector;
		for(auto syn : groupesSynonymes[noeud_courant->appSynonymes[position]]){
			ret_vector.push_back(syn->radical);

		}
		return ret_vector;

	}

	std::string DicoSynonymes::rechercherRadical(const std::string& mot) const{
		NoeudDicoSynonymes * noeud_courant = racine;
		float score=0;
		std::string meilleur_match;

		while(noeud_courant != nullptr){
			if(similitude(noeud_courant->radical,mot)>score){
				score = similitude(noeud_courant->radical,mot);
				meilleur_match = noeud_courant->radical;
			}
			noeud_courant = noeud_courant->gauche;
		}
		if(score>.50){
			return meilleur_match;
		}else{
			throw std::invalid_argument("word not found");
		}
	}



	void DicoSynonymes::ajouterFlexion(const std::string& motRadical, const std::string& motFlexion){
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(motRadical);

		if(noeud_courant != nullptr){
			noeud_courant->flexions.push_back(motFlexion);
		}
	}



	void DicoSynonymes::supprimerRadical(const std::string& motRadical){

		std::string new_radical = rechercherRadical(motRadical);
		NoeudDicoSynonymes * noeud_courant = racine;

		if(racine->radical==new_radical){
			NoeudDicoSynonymes *noeud_a_detruire = racine;
			racine = racine->gauche;
			delete noeud_a_detruire;
			return;
		}

		while(noeud_courant != nullptr){
			if(noeud_courant->gauche->radical == new_radical){
				NoeudDicoSynonymes *noeud_a_detruire = noeud_courant->gauche;
				noeud_courant->gauche = noeud_courant->gauche->gauche;
				delete noeud_a_detruire;
				return;
			}
			noeud_courant = noeud_courant->gauche;
		}
	}

	void DicoSynonymes::supprimerFlexion(const std::string& motRadical, const std::string& motFlexion){
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(motRadical);

			if(noeud_courant != nullptr){
				noeud_courant->flexions.remove(motFlexion);
			}

	}

	void DicoSynonymes::supprimerSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe){
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(motRadical);
		std::list<NoeudDicoSynonymes*> la_liste = groupesSynonymes[noeud_courant->appSynonymes[numGroupe]];
		std::list<NoeudDicoSynonymes*>::iterator element_a_enlever;
		for(auto item: la_liste){
			if(item->radical == motSynonyme){
				la_liste.remove(item);
				break;
			}
		}
		return;

	}


	bool DicoSynonymes::estVide() const{
		return racine==0;
	}

	//Retourne le nombre de radicaux dans le dictionnaire
	int DicoSynonymes::nombreRadicaux() const{
		return racine->hauteur;
	}


	int DicoSynonymes::getNombreSens(std::string radical) const{
		return 1;
	}

	std::string DicoSynonymes::getSens(std::string radical, int position) const{
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(radical);

	}



	DicoSynonymes::NoeudDicoSynonymes * DicoSynonymes::trouver_noeud_radical(const std::string & radical) const{
		NoeudDicoSynonymes * noeud_courant = racine;

		 std::string new_radical = rechercherRadical(radical);

		while(noeud_courant != nullptr){
			if(noeud_courant->radical == new_radical){
				return noeud_courant;
			}
			noeud_courant = noeud_courant->gauche;
		}
	}

	//Ajouter un synonyme (motSynonyme) d'un radical (motRadical) à un de ses groupes de synonymes.
	//Si numGroupe vaut –1, le synonyme est ajouté dans un nouveau groupe de synonymes
	//et retourne le numéro de ce nouveau groupe dans numgroupe par référence.
	//Erreur si motSynonyme est déjà dans la liste des synonymes du motRadical.
	//Erreur si numGroupe n'est pas correct ou motRadical n'existe pas.

	void DicoSynonymes::ajouterSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe){
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(motRadical);
		NoeudDicoSynonymes * noeud_cible = trouver_noeud_radical(motSynonyme);
		if(numGroupe == -1){
			std::list<NoeudDicoSynonymes*> nouvelle_liste_syn;
			nouvelle_liste_syn.push_back(noeud_cible);
			groupesSynonymes.push_back(nouvelle_liste_syn);
			noeud_courant->appSynonymes.push_back(groupesSynonymes.size()-1);
		}else{
			groupesSynonymes[noeud_courant->appSynonymes[numGroupe]].push_back(noeud_cible);
		}

	}

	float DicoSynonymes::similitude(const std::string& mot1, const std::string& mot2) const{
		const std::size_t len1 = mot1.size();
		const std::size_t len2 = mot2.size();
		float score = 0;
		for(size_t i = 0; i<std::min(len1,len2);i++){
			score += (float)(mot1[i]==mot2[i])/len1;
		}
		return score;

	}

	void DicoSynonymes::ajouterRadical(const std::string& motRadical){
		ajouterEmplacementVide(racine,motRadical);

	}

	void DicoSynonymes::ajouterEmplacementVide(NoeudDicoSynonymes *& noeud_destination, const std::string & data) const {
		if(noeud_destination==0){
			noeud_destination = new NoeudDicoSynonymes(data);
			return;
		}
		ajouterEmplacementVide(noeud_destination->gauche,data);
	}


}//Fin du namespace
