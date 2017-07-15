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

	 /**
	 * \brief Constructeur du dictionnaire de synonymes
	 *
	 * \post Une instance du dictionnaire est cree
	 */
	DicoSynonymes::DicoSynonymes()
		:racine(0),nbRadicaux(0),groupesSynonymes()
		{
		}

	/**
	 * \brief Constructeur du dictionnaire de synonymes
	 *
	 * \post Une instance du dictionnaire est cree
	 * \param[in] fichier un stream contenant les informations a mettre dans le dictionnaire
	 */
	DicoSynonymes::DicoSynonymes(std::ifstream &fichier)
	:racine(0),nbRadicaux(0),groupesSynonymes()
	{
		chargerDicoSynonyme(fichier);
	}


	/**
	 * \brief destructeur du dictionnaire de synonymes
	 *
	 * \post le dictionnaire est detruit et la memoire est liberee
	 */
	DicoSynonymes::~DicoSynonymes(){
		destructeur_recursif(racine);
	}

	/**
	 * \brief fonction recursive de destructeur du dictionnaire de synonymes
	 *
	 * \post Le noeud en intrant des detruit et la memoire est liberee
	 * \param[in] noeud le noeud a detruire recursivement
	 */

	void DicoSynonymes::destructeur_recursif(NoeudDicoSynonymes *& noeud){
		if (noeud != 0) {
			destructeur_recursif(noeud->gauche);
			destructeur_recursif(noeud->droit);
			delete noeud;
			noeud = 0;
		}
	}



	/**
	 * \brief fonction qui retourne les flexions d'un radical
	 *
	 * \param[in] radical le mot a obtenir les flexions
	 */

	std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const{
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(radical);
		std::vector<std::string> ret_vector;
		for(auto flex : noeud_courant->flexions){
			ret_vector.push_back(flex);
		}


		return ret_vector;
	}

	/**
	 * \brief fonction qui retourne les flexions d'un radical
	 *
	 * \param[in] radical le mot a obtenir les flexions
	 */
	std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const{
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(radical);
		std::vector<std::string> ret_vector;
		for(auto syn : groupesSynonymes[noeud_courant->appSynonymes[position]]){
			ret_vector.push_back(syn->radical);

		}
		return ret_vector;

	}

	/**
	 * \brief fonction qui retourne un radical provenant du dictionnaire
	 *
	 * \param[in] mot mot a rechercher dans le dictionnaire
	 */

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
		if(score>.90){
			return meilleur_match;
		}else{
			throw std::invalid_argument("word not found");
		}
	}


	/**
	 * \brief fonction qui ajoute une flexion a un radical trouve
	 *
	 * \param[in] motRadical le radical a ajouter une flexion
	 * \param[in] motFlexion la flexion a ajouter
	 */

	void DicoSynonymes::ajouterFlexion(const std::string& motRadical, const std::string& motFlexion){
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(motRadical);

		if(noeud_courant != nullptr){
			noeud_courant->flexions.push_back(motFlexion);
		}
	}


	/**
	 * \brief fonction qui eneleve un radical
	 *
	 * \param[in] motRadical le radical a enlever
	 */

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


	/**
	 * \brief fonction qui eneleve une flexion a un radical
	 *
	 * \param[in] motRadical le radical auquel on enelve une flexion
	 * \param[in] motFlexion la flexion a enlever
	 */

	void DicoSynonymes::supprimerFlexion(const std::string& motRadical, const std::string& motFlexion){
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(motRadical);

			if(noeud_courant != nullptr){
				noeud_courant->flexions.remove(motFlexion);
			}

	}


	/**
	 * \brief fonction qui eneleve une flexion a un radical
	 *
	 * \param[in] motRadical le radical auquel on enelve un synonyme
	 * \param[in] motSynonyme le synonyme a enlever
	 * \param[in] numGroupe le numero du groupe auquel on enleve le synonyme
	 */

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

	/**
	 * \brief fonction qui verifie si le dictionnnaire est vide
	 *
	 */
	bool DicoSynonymes::estVide() const{
		return racine==0;
	}



	/**
	 * \brief fonction qui donne le nombre de radicaux dans l'arble
	 *
	 */

	int DicoSynonymes::nombreRadicaux() const{
		return racine->hauteur;
	}

	/**
		 * \brief Donne le nombre de groupes de synonymes d'un radical.
		 *
		 * \param[in] radical le radical auquel on veut savoir le nombre de groupe de synonymes
		 */

	int DicoSynonymes::getNombreSens(std::string radical) const{
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(radical);
		return noeud_courant->appSynonymes.size();
	}


	/**
	 * \brief Donne un du mot radical
	 *
	 * \param[in] radical le radical duquel on veut savoir le sens
	 * \param[in] position le numero du sens
	 */
	std::string DicoSynonymes::getSens(std::string radical, int position) const{
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(radical);
		return groupesSynonymes[noeud_courant->appSynonymes[position]].front()->radical;
	}


	/**
	 * \brief Fonction qui retourne le noeud d'un radical trouve
	 *
	 * \param[in] radical le radical qu'on veut obtenir
	 */

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

	/**
	 * \brief Ajouter un synonyme (motSynonyme) d'un radical (motRadical) à un de ses groupes de synonymes.
	 *
	 * \param[in] motRadical le radical qu'on veut ajouter un synonyme
	 * \param[in] motSynonyme le synonyme a ajouter
	 * \param[in] numGroupe le numero du groupe auquel ajouter le synonyme
	 */


	void DicoSynonymes::ajouterSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe){
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(motRadical);
		NoeudDicoSynonymes * noeud_cible ;

		if(numGroupe >-1 && synonyme_existe(motRadical,motSynonyme,numGroupe)){
			throw std::invalid_argument("le synonyme est deja dans la liste");
		}
		try{
			NoeudDicoSynonymes * noeud_cible = trouver_noeud_radical(motSynonyme);
		}	catch(const std::exception){
			ajouterRadical(motSynonyme);
		}

		if(numGroupe == -1){
			std::list<NoeudDicoSynonymes*> * nouvelle_liste_syn = new std::list<NoeudDicoSynonymes*>();
			nouvelle_liste_syn->push_back(trouver_noeud_radical(motSynonyme));

			groupesSynonymes.push_back(*nouvelle_liste_syn);
			noeud_courant->appSynonymes.push_back(groupesSynonymes.size()-1);
		}else{
			groupesSynonymes[noeud_courant->appSynonymes[numGroupe]].push_back(noeud_cible);
		}

	}

	bool DicoSynonymes::synonyme_existe(const  std::string & motRadical,const std::string & motSynonyme, int position ) const{
		NoeudDicoSynonymes * noeud_courant = trouver_noeud_radical(motRadical);
		if(position > noeud_courant->appSynonymes.size()-1){
			throw std::invalid_argument("La position recherchee n'existe pas");
		}
		for(auto syn : groupesSynonymes[noeud_courant->appSynonymes[position]]){
			if(motSynonyme ==  syn->radical){
				return true;
			}
		}

		return false;

	}

	/**
	 * \brief Fonction qui attribue un score de similitude entre deux chaine de caractere
	 *
	 * \param[in] mot1 chaine de caractere a comparer
	 * \param[in] mot2 chaine de caractere a comparer
	 */

	float DicoSynonymes::similitude(const std::string& mot1, const std::string& mot2) const{
		const std::size_t len1 = mot1.size();
		const std::size_t len2 = mot2.size();
		float score = 0;
		for(size_t i = 0; i<std::min(len1,len2);i++){
			score += (float)(mot1[i]==mot2[i])/len1;
		}
		return score;

	}


	/**
	 * \brief Fonction qui ajoute un radical au dictionnaire
	 *
	 * \param[in] motRadical radical a ajouter
	 */
	void DicoSynonymes::ajouterRadical(const std::string& motRadical){
		ajouterEmplacementVide(racine,motRadical);

	}


	/**
	 * \brief Fonction recursive qui trouve ou ajouter le radical dans l'arbre
	 *
	 * \param[in] noeud_destination noeud qu'on parcours pour ajouter la valeur
	 * \param[in] data valeur de l'element a ajouter
	 */
	void DicoSynonymes::ajouterEmplacementVide(NoeudDicoSynonymes *& noeud_destination, const std::string & data) const {
		if(noeud_destination==0){
			noeud_destination = new NoeudDicoSynonymes(data);
			return;
		}
		ajouterEmplacementVide(noeud_destination->gauche,data);
	}


}//Fin du namespace
