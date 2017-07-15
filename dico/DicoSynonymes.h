/**
* \file DicoSynonymes.h
* \brief Interface du type DicoSynonymes
* \author IFT-2008
* \version 0.5
* \date juillet 2017
*
* Travail pratique numéro 3
*
*/

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <queue>


#ifndef _DICOSYNONYMES__H
#define _DICOSYNONYMES__H

namespace TP3
{
	//classe représentant un dictionnaire des synonymes
	class DicoSynonymes
	{
	public:

		//Constructeur
		DicoSynonymes();

		//Constructeur de dictionnaire à partir d'un fichier
		//Le fichier est ouvert au préalable
		//Cette méthode appelle chargerDicoSynonyme !
		DicoSynonymes(std::ifstream &fichier);


		//Destructeur.
		~DicoSynonymes();

		//Ajouter un radical au dictionnaire des synonymes
		//tout en s’assurant de maintenir l'équilibre de l'arbre.
		//Erreur si le radical existe déjà.
		void ajouterRadical(const std::string& motRadical);

		//Ajouter une flexion (motFlexion) d'un radical (motRadical) à sa liste de flexions.
		//Erreur si motFlexion existe déjà ou motRadical n'existe pas.
		void ajouterFlexion(const std::string& motRadical, const std::string& motFlexion);

		//Ajouter un synonyme (motSynonyme) d'un radical (motRadical) à un de ses groupes de synonymes.
		//Si numGroupe vaut –1, le synonyme est ajouté dans un nouveau groupe de synonymes
		//et retourne le numéro de ce nouveau groupe dans numgroupe par référence.
		//Erreur si motSynonyme est déjà dans la liste des synonymes du motRadical.
		//Erreur si numGroupe n'est pas correct ou motRadical n'existe pas.
		void ajouterSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe);

		//Supprimer un radical du dictionnaire des synonymes
		//tout en s’assurant de maintenir l'équilibre de l'arbre.
		//Si le radical appartient au dictionnaire, on l'enlève et on équilibre.
		//Il faut libérer la liste des synonymes du radical en question ainsi que sa liste des flexions.
		//Il faut aussi s’assurer d’éliminer tout pointeur pointant sur le nœud contenant le radical.
		//Erreur si l'arbre est vide ou motRadical n'existe pas.
		void supprimerRadical(const std::string& motRadical);

		//Supprimer une flexion (motFlexion) d'un radical (motRadical) de sa liste de flexions.
		//motRadical et motFlexion font parti de l'arbre.
		//La flexion est enlevée du dictionnaire des synonymes.
		// Erreur si l'arbre est vide ou motFlexion n'existe pas ou motRadical n'existe pas.
		void supprimerFlexion(const std::string& motRadical, const std::string& motFlexion);

		//Retirer motSynonyme faisant partie du numéro de groupe numGroupe du motRadical.
		//motRadical et motSynonyme existent et motRadical a une appartenance au groupe numGroupe
		//Le synonyme est enlevé du dictionnaire des synonymes.
		//Erreur si motSynonyme ou motRadical ou numGroupe n'existent pas.
		void supprimerSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe);

		//Vérifier si le dictionnaire est vide
		bool estVide() const;

		//Retourne le nombre de radicaux dans le dictionnaire
		int nombreRadicaux() const;

		//Affiche à l'écran l'arbre niveau par niveau de façon à voir si l'arbre est bien balancé.
		//Ne touchez pas s.v.p. à cette méthode !
		friend std::ostream& operator<<(std::ostream& out, const DicoSynonymes& d)
		{
    	  if (d.racine != 0)
    	  {
    		  std::queue<NoeudDicoSynonymes*> file;
    		  std::queue<std::string> fileNiveau;

    		  NoeudDicoSynonymes * noeudDicotemp;
    		  std::string niveauTemp;

    		  int hg = 0, hd = 0;

    		  file.push(d.racine);
    		  fileNiveau.push("1");
    		  while (!file.empty())
    		  {
    			  noeudDicotemp = file.front();
    			  niveauTemp = fileNiveau.front();
    			  out << noeudDicotemp->radical;
    			  if (noeudDicotemp->gauche == 0) hg = -1; else hg = noeudDicotemp->gauche->hauteur;
    			  if (noeudDicotemp->droit == 0) hd = -1; else hd = noeudDicotemp->droit->hauteur;
    			  out << ", " << hg - hd;
    			  out << ", " << niveauTemp;
    			  out << std::endl;
    			  file.pop();
    			  fileNiveau.pop();
    			  if (noeudDicotemp->gauche != 0)
    			  {
    				  file.push(noeudDicotemp->gauche);
    				  fileNiveau.push(niveauTemp + ".1");
    			  }
    			  if (noeudDicotemp->droit != 0)
    			  {
    				  file.push(noeudDicotemp->droit);
    				  fileNiveau.push(niveauTemp + ".2");
    			  }
    		  }
    	  }
    	  return out;
		}

		//Charge le dictionnaire de synonymes
		//Erreur si p_fichier n'est pas ouvert
		void chargerDicoSynonyme(std::ifstream & fichier);

		//Retourne le radical du mot entré en paramètre
		//mot est dans la liste des flexions d'un radical
		//Erreur si l'arbre est vide
		//Erreur si la flexion n'est pas dans la liste de flexions du radical
		std::string rechercherRadical(const std::string& mot) const;

		//Retourne un réel entre 0 et 1 qui représente le degré de similitude entre mot1 et mot2 où
		//0 représente deux mots complétement différents et 1 deux mots identiques.
		//Vous pouvez utiliser par exemple la distance de Levenshtein, mais ce n'est pas obligatoire !
		float similitude(const std::string& mot1, const std::string& mot2) const;

		//Donne le nombre de groupes de synonymes d'un radical.
		//Le nombre de cases de appSynonymes pour le radical entré en paramètre est retourné
		int getNombreSens(std::string radical) const;

		//Donne le premier synonyme du groupe de synonymes d'un radical.
		//un string correspondant au premier synonyme est retourné
		std::string getSens(std::string radical, int position) const;

		//Donne tous les synonymes d'un radical pour un groupe en particulier qui est identifé par position.
		//Un vecteur est retourné avec tous les synonymes.
		std::vector<std::string> getSynonymes(std::string radical, int position) const;

		//Donne toutes les flexions d'un radical.
		//Un vecteur est retourné avec toutes les flexions
		std::vector<std::string> getFlexions(std::string radical) const;

   private:

		//classe représentant un noeud dans un dictionnaire (arbre AVL)
		class NoeudDicoSynonymes
		{
		  public:
			 std::string radical;             		// Un radical qui est un élément fondamental d'un mot, auquel s'ajoutent suffixes et préfixes.
			 std::list<std::string> flexions;   	// Une liste chaînée de chaîne de caractères représentant les flexions du radical.
			 std::vector<int> appSynonymes;   		// Un tableau contenant les indices (entiers) des groupes de synonymes auquel appartient le radical.
			 int hauteur;                    		// La hauteur de ce noeud (afin de maintenir l'équilibre de l'arbre AVL).
			 NoeudDicoSynonymes *gauche, *droit;	// Les pointeurs sur les enfants du noeud.
			 NoeudDicoSynonymes(const std::string& m) : radical(m), hauteur(0), gauche(0), droit(0) { }
		};

		void ajouterEmplacementVide(NoeudDicoSynonymes *& noeud_d,const std::string & data) const;
//		std::vector<NoeudDicoSynonymes> obtenirTousLesNoeuds(NoeudDicoSynonymes * noeud_s) const;

		NoeudDicoSynonymes *racine;                           // pointeur vers la racine de l'arbre des radicaux.
		int nbRadicaux;                                       // Nombre de noeuds dans l'arbre
		std::vector< std::list<NoeudDicoSynonymes*> > groupesSynonymes;	// Tableau de groupes de synonymes dont chaque élément est une liste
      	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	// de pointeurs sur des noeuds de l'arbre représentant les radicaux.
		bool synonyme_existe(const std::string & motRadical,const std::string & motSynonyme, int position ) const;
		void destructeur_recursif(NoeudDicoSynonymes*& noeud);
		NoeudDicoSynonymes * trouver_noeud_radical(const std::string & radical) const;
      // Ajoutez vos méthodes privées ici !

   };

}//Fin du namespace

#endif
