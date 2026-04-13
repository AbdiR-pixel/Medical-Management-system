#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define PASSWORD_MEDECIN "med123"
#define PASSWORD_ADMIN "admin123"
#define PASSWORD_STA "sta123"
#define MAX_PATIENTS 100
#define MAX_RENDEZVOUS 100
#define MAX_EXAMENS 100
#define MAX_USERS 100

typedef struct {
    char date[20];
    char heure[10];
    char objet[100];
} RendezVous;

typedef struct {
    char id[20];
    char nom[50];
    char prenom[50];
    char date_naissance[20];
    char genre[10];
    char telephone[20];
    char adresse[100];
    char categorie[50]; // Payant ou CNSS
    char service[100];
    char situation_familiale[20]; // Marié ou Célibataire
    char fonctionnalite[200]; // Fonctionnalité du patient
    bool estDecede; // si la personne est decede
    RendezVous rendezvous[MAX_RENDEZVOUS];
    int nbRendezVous;
    char diagnostic[200];
    char traitement[200];
    char observations[200];
    char antecedents[200];
    char examens_complementaires[200];
    char analyse_sang[200];
} Patient;

Patient patients[MAX_PATIENTS];
int nbPatients = 0;

typedef struct {
    char id[20];
    char password[20];
    char role[10]; // "admin", "med", "staff"
} Utilisateur;

Utilisateur utilisateurs[MAX_USERS];
int nbUtilisateurs = 0;

void clear() {
    printf("\033[H\033[J");
}
void chargerUtilisateurs() {
    FILE *f = fopen("users.txt", "r");
    if (!f) return;
    nbUtilisateurs = 0;
    while (fscanf(f, "%19[^,],%19[^,],%9s\n", utilisateurs[nbUtilisateurs].id, utilisateurs[nbUtilisateurs].password, utilisateurs[nbUtilisateurs].role) == 3) {
        nbUtilisateurs++;
    }
    fclose(f);
}

// Sauvegarde des utilisateurs dans "users.txt"
void sauvegarderUtilisateurs() {
    FILE *f = fopen("users.txt", "w");
    if (!f) return;
    for (int i = 0; i < nbUtilisateurs; i++) {
        fprintf(f, "%s,%s,%s\n", utilisateurs[i].id, utilisateurs[i].password, utilisateurs[i].role);
    }
    fclose(f);
}

// Vérifie l'identité d'un utilisateur
int verifierUtilisateur(const char* id, const char* password, const char* role) {
    for (int i = 0; i < nbUtilisateurs; i++) {
        if (strcmp(utilisateurs[i].id, id) == 0 && strcmp(utilisateurs[i].password, password) == 0 && strcmp(utilisateurs[i].role, role) == 0) {
            return 1;
        }
    }
    return 0;
}

// Logger une action
void logger(const char* user, const char* action) {
    FILE *f = fopen("log.txt", "a");
    if (!f) return;
    time_t now = time(NULL);
    fprintf(f, "[%s] %s: %s\n", strtok(ctime(&now), "\n"), user, action);
    fclose(f);
}

// Ajout d'un utilisateur (admin uniquement)
void ajouterUtilisateur() {
    if (nbUtilisateurs >= MAX_USERS) {
        printf("Limite utilisateur atteinte.\n");
        return;
    }
    Utilisateur u;
    printf("ID nouvel utilisateur : ");
    scanf("%19s", u.id);
    printf("Mot de passe : ");
    scanf("%19s", u.password);
    printf("Role (admin/med/staff) : ");
    scanf("%9s", u.role);
    utilisateurs[nbUtilisateurs++] = u;
    sauvegarderUtilisateurs();
    printf("Utilisateur ajoute.\n");
    logger("ADMIN", "Ajout d'un nouvel utilisateur");
}

// Suppression d'un utilisateur
void supprimerUtilisateur() {
    char id[20];
    printf("ID de l'utilisateur a supprimer : ");
    scanf("%19s", id);
    for (int i = 0; i < nbUtilisateurs; i++) {
        if (strcmp(utilisateurs[i].id, id) == 0) {
            for (int j = i; j < nbUtilisateurs - 1; j++) {
                utilisateurs[j] = utilisateurs[j+1];
            }
            nbUtilisateurs--;
            sauvegarderUtilisateurs();
            printf("Utilisateur supprime.\n");
            logger("ADMIN", "Suppression d'un utilisateur");
            return;
        }
    }
    printf("Utilisateur non trouve.\n");
}

// Changer son mot de passe
void changerMotDePasse() {
    char id[20], oldpass[20], newpass[20];
    printf("ID : "); scanf("%19s", id);
    printf("Ancien mot de passe : "); scanf("%19s", oldpass);
    for (int i = 0; i < nbUtilisateurs; i++) {
        if (strcmp(utilisateurs[i].id, id) == 0 && strcmp(utilisateurs[i].password, oldpass) == 0) {
            printf("Nouveau mot de passe : ");
            scanf("%19s", newpass);
            strcpy(utilisateurs[i].password, newpass);
            sauvegarderUtilisateurs();
            printf("Mot de passe mis a jour.\n");
            logger(id, "Changement de mot de passe");
            return;
        }
    }
    printf("Utilisateur ou mot de passe incorrect.\n");
}

void pause() {
    printf("\nAppuyez sur Entree pour continuer...");
    while(getchar() != '\n'); // Vide le buffer
    getchar(); // Attend la nouvelle entrée
}

// Fonction pour sauvegarder les données des patients dans un fichier texte
void sauvegarderPatientsVersTXT() {
    FILE *fichier = fopen("patients.txt", "w");
    if (fichier == NULL) {
        printf("Erreur lors de la creation du fichier de sauvegarde!\n");
        return;
    }

    for (int i = 0; i < nbPatients; i++) {
        fprintf(fichier, "[Patient]\n");
        fprintf(fichier, "Identite=%s\n", patients[i].id);
        fprintf(fichier, "Nom=%s\n", patients[i].nom);
        fprintf(fichier, "Prenom=%s\n", patients[i].prenom);
        fprintf(fichier, "DateNaissance=%s\n", patients[i].date_naissance);
        fprintf(fichier, "Genre=%s\n", patients[i].genre);
        fprintf(fichier, "Telephone=%s\n", patients[i].telephone);
        fprintf(fichier, "Adresse=%s\n", patients[i].adresse);
        fprintf(fichier, "Categorie=%s\n", patients[i].categorie);
        fprintf(fichier, "Service=%s\n", patients[i].service);
        fprintf(fichier, "SituationFamiliale=%s\n", patients[i].situation_familiale);
        fprintf(fichier, "Fonctionnalite=%s\n", patients[i].fonctionnalite);
        fprintf(fichier, "Decede=%d\n", patients[i].estDecede);
        fprintf(fichier, "Diagnostic=%s\n", patients[i].diagnostic);
        fprintf(fichier, "Traitement=%s\n", patients[i].traitement);
        fprintf(fichier, "Observations=%s\n", patients[i].observations);
        fprintf(fichier, "Antecedents=%s\n", patients[i].antecedents);
        fprintf(fichier, "Examens=%s\n", patients[i].examens_complementaires);
        fprintf(fichier, "AnalyseSang=%s\n", patients[i].analyse_sang);

        // Sauvegarde des rendez-vous
        fprintf(fichier, "NbRendezVous=%d\n", patients[i].nbRendezVous);
        for (int j = 0; j < patients[i].nbRendezVous; j++) {
            fprintf(fichier, "RendezVous_Date=%s\n", patients[i].rendezvous[j].date);
            fprintf(fichier, "RendezVous_Heure=%s\n", patients[i].rendezvous[j].heure);
            fprintf(fichier, "RendezVous_Objet=%s\n", patients[i].rendezvous[j].objet);
        }
        fprintf(fichier, "[FinPatient]\n\n");
    }
    fclose(fichier);
}

void chargerPatientsDepuisTXT() {
    FILE *fichier = fopen("patients.txt", "r");
    if (fichier == NULL) {
        printf("Aucune sauvegarde trouvee. Creation d'un nouveau fichier.\n");
        return;
    }

    nbPatients = 0;
    char ligne[256];
    Patient *patientActuel = NULL;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        ligne[strcspn(ligne, "\n")] = 0; // Supprime le saut de ligne

        if (strcmp(ligne, "[Patient]") == 0) {
            // Nouveau patient
            if (nbPatients < MAX_PATIENTS) {
                patientActuel = &patients[nbPatients];
                memset(patientActuel, 0, sizeof(Patient)); // Initialisation
                nbPatients++;
            }
        }
        else if (patientActuel != NULL) {
            // Lecture des champs
            char *cle = strtok(ligne, "=");
            char *valeur = strtok(NULL, "=");

            if (cle != NULL && valeur != NULL) {
                if (strcmp(cle, "ID") == 0) strcpy(patientActuel->id, valeur);
                else if (strcmp(cle, "Nom") == 0) strcpy(patientActuel->nom, valeur);
                else if (strcmp(cle, "Prenom") == 0) strcpy(patientActuel->prenom, valeur);
                else if (strcmp(cle, "DateNaissance") == 0) strcpy(patientActuel->date_naissance, valeur);
                else if (strcmp(cle, "Genre") == 0) strcpy(patientActuel->genre, valeur);
                else if (strcmp(cle, "Telephone") == 0) strcpy(patientActuel->telephone, valeur);
                else if (strcmp(cle, "Adresse") == 0) strcpy(patientActuel->adresse, valeur);
                else if (strcmp(cle, "Categorie") == 0) strcpy(patientActuel->categorie, valeur);
                else if (strcmp(cle, "Service") == 0) strcpy(patientActuel->service, valeur);
                else if (strcmp(cle, "SituationFamiliale") == 0) strcpy(patientActuel->situation_familiale, valeur);
                else if (strcmp(cle, "Fonctionnalite") == 0) strcpy(patientActuel->fonctionnalite, valeur);
                else if (strcmp(cle, "Decede") == 0) patientActuel->estDecede = atoi(valeur);
                else if (strcmp(cle, "Diagnostic") == 0) strcpy(patientActuel->diagnostic, valeur);
                else if (strcmp(cle, "Traitement") == 0) strcpy(patientActuel->traitement, valeur);
                else if (strcmp(cle, "Observations") == 0) strcpy(patientActuel->observations, valeur);
                else if (strcmp(cle, "Antecedents") == 0) strcpy(patientActuel->antecedents, valeur);
                else if (strcmp(cle, "Examens") == 0) strcpy(patientActuel->examens_complementaires, valeur);
                else if (strcmp(cle, "AnalyseSang") == 0) strcpy(patientActuel->analyse_sang, valeur);
                else if (strcmp(cle, "NbRendezVous") == 0) patientActuel->nbRendezVous = atoi(valeur);
                else if (strncmp(cle, "RendezVous_", 11) == 0 && patientActuel->nbRendezVous > 0) {
                    int index = patientActuel->nbRendezVous - 1;
                    if (strcmp(cle+11, "Date") == 0) strcpy(patientActuel->rendezvous[index].date, valeur);
                    else if (strcmp(cle+11, "Heure") == 0) strcpy(patientActuel->rendezvous[index].heure, valeur);
                    else if (strcmp(cle+11, "Objet") == 0) strcpy(patientActuel->rendezvous[index].objet, valeur);
                }
            }
        }
    }
    fclose(fichier);
}

// Fonction pour choisir un service avec switch
void choisirService(char* service) {
    int choice;

    printf("\n=== LISTE DES CONSULTATIONS ===\n");
    printf("1. Pediatrie\n");
    printf("2. Dialyse\n");
    printf("3. Cardiologie\n");
    printf("4. Radiologie\n");
    printf("5. Urgences\n");
    printf("6. Chirurgie generale\n");
    printf("7. Dermatologie\n");
    printf("8. Gyncologie\n");
    printf("9. Ophtalmologie\n");
    printf("10. ORL\n");
    printf("11. Neurologie\n");
    printf("12. Pneumologie\n");
    printf("13. Endocrinologie\n");
    printf("14. Gastro-enterologie\n");
    printf("15. Rhumatologie\n");
    printf("Choisissez (1-15) : ");

    scanf("%d", &choice);
    while(getchar() != '\n'); // Vide le buffer

    switch(choice) {
        case 1: strcpy(service, "Pediatrie"); break;
        case 2: strcpy(service, "Dialyse"); break;
        case 3: strcpy(service, "Cardiologie"); break;
        case 4: strcpy(service, "Radiologie"); break;
        case 5: strcpy(service, "Urgences"); break;
        case 6: strcpy(service, "Chirurgie generale"); break;
        case 7: strcpy(service, "Dermatologie"); break;
        case 8: strcpy(service, "Gynecologie"); break;
        case 9: strcpy(service, "Ophtalmologie"); break;
        case 10: strcpy(service, "ORL"); break;
        case 11: strcpy(service, "Neurologie"); break;
        case 12: strcpy(service, "Pneumologie"); break;
        case 13: strcpy(service, "Endocrinologie"); break;
        case 14: strcpy(service, "Gastro-enterologie"); break;
        case 15: strcpy(service, "Rhumatologie"); break;
        default:
            printf("Choix invalide, service par defaut (Medecine generale) selectionne.\n");
            strcpy(service, "Medecine generale");
    }
}


// Fonction d'ajout d'un nouveau patient
void ajouterPatient() {
    if (nbPatients >= MAX_PATIENTS) {
        printf("Limite atteinte.\n");
        return;
    }
    Patient p;
    printf("\n=== Ajouter un patient ===\n");
    printf("ID : "); scanf("%19s", p.id);while(getchar() != '\n');
    printf("Nom : "); scanf("%49s", p.nom);while(getchar() != '\n');
    printf("Prenom : ");
    fgets(p.prenom, sizeof(p.prenom), stdin);
    p.prenom[strcspn(p.prenom, "\n")] = '\0';
    printf("Date de naissance (jj/mm/aaaa) : "); scanf("%19s", p.date_naissance);
    printf("Genre (M/F) : "); scanf("%9s", p.genre);while(getchar() != '\n');
    printf("Telephone : "); scanf("%19s", p.telephone);while(getchar() != '\n');
    printf("Adresse : "); scanf(" %99[^\n]", p.adresse);while(getchar() != '\n');

    printf("Categorie (p pour payant, c pour CNSS) : ");
    char choixa;
    scanf(" %c", &choixa);

    if (choixa == 'p' || choixa == 'P') {
        strcpy(p.categorie, "payant");
    } else {
        strcpy(p.categorie, "CNSS");
        printf("Numero CNSS (9 chiffres) : ");
        char num[20];
        scanf("%19s", num);
        strcat(p.categorie, " ");
        strcat(p.categorie, num);
    }
    printf("\nType de consultation :");
    choisirService(p.service);
    printf("Situation familiale (marie/celibataire) : "); scanf("%19s", p.situation_familiale);
    printf("Fonctionnalite du patient : "); scanf(" %199[^\n]", p.fonctionnalite);
    p.estDecede = false;
    p.nbRendezVous = 0;
    strcpy(p.diagnostic, "");
    strcpy(p.traitement, "");
    strcpy(p.observations, "");
    strcpy(p.antecedents, "");
    strcpy(p.examens_complementaires, "");
    strcpy(p.analyse_sang, "");

    patients[nbPatients++] = p;
    sauvegarderPatientsVersTXT();
    printf("Patient ajoute.\n");
}

// Fonction pour modifier les informations d'un patient
void modifierPatient() {
    char id[20];
    printf("Identite du patient a modifier : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("Modifier les informations du patient %s %s\n", patients[i].nom, patients[i].prenom);
        printf("Nom : "); scanf("%49s", patients[i].nom);
        printf("Prenom : ");
        fgets(patients[i].prenom, sizeof(patients[i].prenom), stdin);
        patients[i].prenom[strcspn(patients[i].prenom, "\n")] = '\0';
        printf("Date de naissance (jj/mm/aaaa) : "); scanf("%19s", patients[i].date_naissance);while(getchar() != '\n');
        printf("Genre : "); scanf("%9s", patients[i].genre);while(getchar() != '\n');
        printf("Telephone : "); scanf("%19s", patients[i].telephone);while(getchar() != '\n');
        printf("Adresse : "); scanf(" %99[^\n]", patients[i].adresse);while(getchar() != '\n');

        printf("Nouvelle catégorie (p pour payant, c pour CNSS) : ");
        char choix;
        scanf(" %c", &choix);

        if (choix == 'p' || choix == 'P') {
            strcpy(patients[i].categorie, "payant");

        } else {
            strcpy(patients[i].categorie, "CNSS");
            printf("Numero CNSS : ");
            char num[20];
            scanf("%19s", num);
            strcat(patients[i].categorie, " ");
            strcat(patients[i].categorie, num);
}
        printf("Service hospitalise : "); scanf(" %99[^\n]", patients[i].service);
        printf("Situation familiale (marie/celibataire) : "); scanf("%19s", patients[i].situation_familiale);while(getchar() != '\n');
        printf("Fonctionnalite du patient : "); scanf(" %199[^\n]", patients[i].fonctionnalite);while(getchar() != '\n');
        sauvegarderPatientsVersTXT();
        printf("Patient modifie.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour supprimer un patient
void supprimerPatient() {
    char id[20];
    printf("Identite du patient a supprimer : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        for (int j = i; j < nbPatients - 1; j++) {
            patients[j] = patients[j + 1];
        }
        nbPatients--;
        sauvegarderPatientsVersTXT();
        printf("Patient supprime.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour programmer un rendez-vous pour un patient
void programmerRendezVous() {
    char id[20];
    printf("Identite du patient pour rendez-vous : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        RendezVous rdv;
        printf("Date du rendez-vous (jj/mm/aaaa) : ");
        scanf("%19s", rdv.date);
        printf("Heure du rendez-vous : ");
        scanf("%9s", rdv.heure);
        printf("Objet du rendez-vous : ");
        scanf(" %99[^\n]", rdv.objet);
        patients[i].rendezvous[patients[i].nbRendezVous++] = rdv;
        sauvegarderPatientsVersTXT();
        printf("Rendez-vous programme.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour consulter le dossier complet d'un patient
void consulterDossierPatient() {
    char id[20];
    printf("Identite du patient a consulter : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("\n=== Dossier Complet ===\n");
        printf("ID : %s\nNom : %s\nPrenom : %s\nDate de naissance : %s\nGenre : %s\nTelephone : %s\nAdresse : %s\nCategorie : %s\nService : %s\nSituation Familiale : %s\nFonctionnalite : %s\nDecede : %s\n",
            patients[i].id, patients[i].nom, patients[i].prenom, patients[i].date_naissance, patients[i].genre, patients[i].telephone, patients[i].adresse,
            patients[i].categorie, patients[i].service, patients[i].situation_familiale, patients[i].fonctionnalite, patients[i].estDecede ? "Oui" : "Non");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour consulter le dossier complet d'un patient pour le dmedecin
void consulterDossierPatientpourmed() {
    char id[20];
    printf("Identite du patient a consulter : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("\n=== Dossier Complet ===\n");
        printf("ID : %s\nNom : %s\nPrenom : %s\nDate de naissance : %s\nGenre : %s\nTelephone : %s\nAdresse : %s\nCategorie : %s\nService : %s\nSituation Familiale : %s\nFonctionnalite : %s\nDiagnostic : %s\nTraitement : %s\nObservations : %s\nAntecedents : %s\nAnalyse du sang : %s\nExamens complementaires  : %s\nDecede : %s\n",
            patients[i].id, patients[i].nom, patients[i].prenom, patients[i].date_naissance, patients[i].genre, patients[i].telephone, patients[i].adresse,
            patients[i].categorie, patients[i].service, patients[i].situation_familiale, patients[i].fonctionnalite,patients[i].diagnostic,patients[i].traitement,patients[i].observations,patients[i].antecedents,patients[i].analyse_sang,patients[i].examens_complementaires, patients[i].estDecede ? "Oui" : "Non");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour afficher la liste de tous les patients enregistrés
void afficherTousPatients() {
    printf("\n=== Liste de tous les patients ===\n");
    for (int i = 0; i < nbPatients; i++) {
        printf("%s - %s %s\n", patients[i].id, patients[i].nom, patients[i].prenom);
    }
}

// Fonction pour afficher la liste des rendez-vous programmés
void afficherRendezVous() {
    printf("\n=== Liste des rendez-vous programmes ===\n");
    for (int i = 0; i < nbPatients; i++) {
        printf("Patient : %s %s\n", patients[i].nom, patients[i].prenom);
        for (int j = 0; j < patients[i].nbRendezVous; j++) {
            printf("Date : %s, Heure : %s, Objet : %s\n", patients[i].rendezvous[j].date, patients[i].rendezvous[j].heure, patients[i].rendezvous[j].objet);
        }
    }
}

// Menu Administration
void menuAdministration() {
    char mdp[20];
    printf("Mot de passe : ");
    scanf("%19s", mdp);
    if (strcmp(mdp, PASSWORD_ADMIN) != 0) {
        printf("Mot de passe incorrect.\n");
        return;
    }
    int choix;
    do {
        clear();
        printf("=== MENU ADMIN ===\n");
        printf("1. Ajouter un patient\n2. Modifier un patient\n3. Supprimer un patient\n4. Programmer un rendez-vous\n5. Consulter dossier patient\n6. Voir tous les patients\n7. Voir les rendez-vous programmes\n8. Sauvegarder les donnees\n0. Retour\nChoix : ");
        scanf("%d", &choix);
        switch (choix) {
            case 1: ajouterPatient(); break;
            case 2: modifierPatient(); break;
            case 3: supprimerPatient(); break;
            case 4: programmerRendezVous(); break;
            case 5: consulterDossierPatient(); break;
            case 6: afficherTousPatients(); break;
            case 7: afficherRendezVous(); break;
            case 8: sauvegarderPatientsVersTXT(); break;
        }
        if (choix != 0) pause();
    } while (choix != 0);
}

// Fonction pour afficher tous les patients (partie Médecin)
void afficherPatients() {
    printf("\n--- Liste des patients ---\n");
    for (int i = 0; i < nbPatients; i++) {
        printf("ID : %s - Nom : %s %s\n", patients[i].id, patients[i].nom, patients[i].prenom);
    }
}

// Fonction pour ajouter/modifier un diagnostic pour un patient
void ajouterModifierDiagnostic() {
    char id[20];
    printf("Identite du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("Ajouter ou modifier le diagnostic pour %s %s :\n", patients[i].nom, patients[i].prenom);
        printf("Diagnostic : ");
        scanf(" %199[^\n]", patients[i].diagnostic);
        sauvegarderPatientsVersTXT();
        printf("Diagnostic mis a jour.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour prescrire un traitement à un patient
void prescrireTraitement() {
    char id[20];
    printf("ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("Prescrire un traitement pour %s %s :\n", patients[i].nom, patients[i].prenom);
        printf("Traitement : ");
        scanf(" %199[^\n]", patients[i].traitement);
        sauvegarderPatientsVersTXT();
        printf("Traitement prescrit.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour ajouter des observations cliniques
void ajouterObservations() {
    char id[20];
    printf("ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("Ajouter des observations cliniques pour %s %s :\n", patients[i].nom, patients[i].prenom);
        printf("Observations : ");
        scanf(" %199[^\n]", patients[i].observations);
        sauvegarderPatientsVersTXT();
        printf("Observations cliniques ajoutees.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour ajouter des antécédents médicaux
void ajouterAntecedents() {
    char id[20];
    printf("ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("Ajouter les antecedents medicaux pour %s %s :\n", patients[i].nom, patients[i].prenom);
        printf("Antecedents : ");
        scanf(" %199[^\n]", patients[i].antecedents);
        sauvegarderPatientsVersTXT();
        printf("Antecedents ajoutes.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour ajouter des examens complémentaires
void ajouterExamensComplementaires() {
    char id[20];
    printf("ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("Ajouter des examens complementaires pour %s %s :\n", patients[i].nom, patients[i].prenom);
        printf("Examens : ");
        scanf(" %199[^\n]", patients[i].examens_complementaires);
        sauvegarderPatientsVersTXT();
        printf("Examens complementaires ajoutes.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour ajouter des analyses de sang
void ajouterAnalyseSang() {
    char id[20];
    printf("ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("Ajouter une analyse de sang pour %s %s :\n", patients[i].nom, patients[i].prenom);
        printf("Analyse de sang : ");
        scanf(" %199[^\n]", patients[i].analyse_sang);
        sauvegarderPatientsVersTXT();
        printf("Analyse de sang ajoutee.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour marquer un patient comme décédé
void marquerCommeDecede() {
    char id[20];
    printf("ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        patients[i].estDecede = true;
        sauvegarderPatientsVersTXT();
        printf("Patient marque comme decede.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Menu Médecin
void menuMedecin() {
    char mdp[20];
    printf("Mot de passe : ");
    scanf("%19s", mdp);
    if (strcmp(mdp, PASSWORD_MEDECIN) != 0) {
        printf("Mot de passe incorrect.\n");
        return;
    }
    int choix;
    do {
        clear();
        printf("=== MENU MEDECIN ===\n");
        printf("1. Afficher les patients\n2. Consulter un dossier d'un patient\n3. Ajouter/Modifier diagnostic\n4. Prescrire un traitement\n5. Ajouter des observations cliniques\n6. Ajouter des antecedents medicaux\n7. Ajouter des examens complementaires\n8. Ajouter une analyse de sang\n9. Programmer un rendez-vous\n10. Marquer comme decede\n0. Retour\nVotre Choix : ");
        scanf("%d", &choix);
        switch (choix) {
            case 1: afficherPatients(); break;
            case 2: consulterDossierPatientpourmed(); break;
            case 3: ajouterModifierDiagnostic(); break;
            case 4: prescrireTraitement(); break;
            case 5: ajouterObservations(); break;
            case 6: ajouterAntecedents(); break;
            case 7: ajouterExamensComplementaires(); break;
            case 8: ajouterAnalyseSang(); break;
            case 9: programmerRendezVous(); break;
            case 10: marquerCommeDecede(); break;
        }
        if (choix != 0) pause();
    } while (choix != 0);
}

// Fonction pour consulter le dossier médical complet
void consulterDossier() {
    char id[20];
    printf("\nEntrez l'ID du patient : ");
    if (scanf("%19s", id) != 1) {
        printf("Erreur de saisie.\n");
        while (getchar() != '\n'); // Vide le buffer d'entrée
        return;
    }

    int patientIndex = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            patientIndex = j;
            break;
        }
    }

    if (patientIndex == -1) {
        printf("\nPatient non trouve.\n");
        pause();
        return;
    }

    // Calcul de l'âge plus précis
    int anneeNaissance;
    if (sscanf(patients[patientIndex].date_naissance + 6, "%d", &anneeNaissance) != 1) {
        printf("Erreur: Format de date invalide\n");
        anneeNaissance = 0;
    }
    int age = 2025 - anneeNaissance; // Remplacez 2025 par l'année actuelle si nécessaire

    // Affichage formaté
    printf("\n=== DOSSIER MEDICAL ===\n");
    printf("=== %s %s ===\n\n", patients[patientIndex].nom, patients[patientIndex].prenom);

    printf("*Informations personnelles :\n");
    printf("ID: %s\n", patients[patientIndex].id);
    printf("Age: %d ans\n", age);
    printf("Genre: %s\n", patients[patientIndex].genre);
    printf("Telephone: %s\n", patients[patientIndex].telephone);
    printf("Adresse: %s\n\n", patients[patientIndex].adresse);

    printf("*Informations medicales :\n");
    printf("Service: %s\n", patients[patientIndex].service);
    printf("Traitement: %s\n", patients[patientIndex].traitement);
    printf("Statut: %s\n\n", patients[patientIndex].estDecede ? "Decede" : "Vivant");

    printf(" Rendez-vous programmes: %d\n", patients[patientIndex].nbRendezVous);
    for (int j = 0; j < patients[patientIndex].nbRendezVous; j++) {
        printf(" %s a %s: %s\n",
               patients[patientIndex].rendezvous[j].date,
               patients[patientIndex].rendezvous[j].heure,
               patients[patientIndex].rendezvous[j].objet);
    }
}

// Fonction pour prendre un rendez-vous médical
void prendreRendezVous() {
    char id[20];
    printf("Entrez l'ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        RendezVous rdv;
        printf("Date du rendez-vous (jj/mm/aaaa) : ");
        scanf("%19s", rdv.date);
        printf("Heure du rendez-vous : ");
        scanf("%9s", rdv.heure);
        printf("Objet du rendez-vous : ");
        scanf(" %99[^\n]", rdv.objet);
        patients[i].rendezvous[patients[i].nbRendezVous++] = rdv;
        sauvegarderPatientsVersTXT();
        printf("Rendez-vous pris.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour voir les rendez-vous à venir
void voirRendezVous() {
    char id[20];
    printf("Entrez l'ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("\n=== Rendez-vous à venir de %s %s ===\n", patients[i].nom, patients[i].prenom);
        for (int j = 0; j < patients[i].nbRendezVous; j++) {
            printf("Date : %s - Heure : %s - Objet : %s\n", patients[i].rendezvous[j].date, patients[i].rendezvous[j].heure, patients[i].rendezvous[j].objet);
            sauvegarderPatientsVersTXT();
        }
    } else {
        printf("Patient non trouvé.\n");
    }
}

// Fonction pour annuler ou modifier un rendez-vous existant
void annulerModifierRendezVous() {
    char id[20];
    printf("Entrez l'ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("\n=== Rendez-vous de %s %s ===\n", patients[i].nom, patients[i].prenom);
        for (int j = 0; j < patients[i].nbRendezVous; j++) {
            printf("Rendez-vous %d - Date : %s - Heure : %s - Objet : %s\n", j+1, patients[i].rendezvous[j].date, patients[i].rendezvous[j].heure, patients[i].rendezvous[j].objet);
        }
        int choix;
        printf("Choisir le rendez-vous à annuler ou modifier (1 à %d) : ", patients[i].nbRendezVous);
        scanf("%d", &choix);
        if (choix >= 1 && choix <= patients[i].nbRendezVous) {
            printf("1. Annuler le rendez-vous\n2. Modifier le rendez-vous\nChoix : ");
            int action;
            scanf("%d", &action);
            if (action == 1) {
                for (int j = choix - 1; j < patients[i].nbRendezVous - 1; j++) {
                    patients[i].rendezvous[j] = patients[i].rendezvous[j + 1];
                }
                patients[i].nbRendezVous--;
                sauvegarderPatientsVersTXT();
                printf("Rendez-vous annule.\n");
            } else if (action == 2) {
                printf("Nouvelle date du rendez-vous (jj/mm/aaaa) : ");
                scanf("%19s", patients[i].rendezvous[choix - 1].date);
                printf("Nouvelle heure du rendez-vous : ");
                scanf("%9s", patients[i].rendezvous[choix - 1].heure);
                printf("Nouvel objet du rendez-vous : ");
                scanf(" %99[^\n]", patients[i].rendezvous[choix - 1].objet);
                sauvegarderPatientsVersTXT();
                printf("Rendez-vous modifié.\n");
            }
        } else {
            printf("Rendez-vous non trouvé.\n");
        }
    } else {
        printf("Patient non trouvé.\n");
    }
}

// Fonction pour mettre à jour les informations personnelles
void mettreAJourInfos() {
    char id[20];
    printf("Entrez l'ID du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("Modifier les informations personnelles pour %s %s :\n", patients[i].nom, patients[i].prenom);
        printf("Nouvelle adresse : ");
        scanf(" %99[^\n]", patients[i].adresse);
        printf("Nouveau numero de telephone : ");
        scanf("%19s", patients[i].telephone);
        sauvegarderPatientsVersTXT();
        printf("Informations mises a jour.\n");
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour suivre les prescriptions de médicaments
void suivreMeds() {
    char id[20];
    printf("Entrez l'Identite du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        printf("\n==== Suivi des medicaments pour %s %s ===\n", patients[i].nom, patients[i].prenom);
        printf("Traitement prescrit : %s\n", patients[i].traitement);
        sauvegarderPatientsVersTXT();
    } else {
        printf("Patient non trouve.\n");
    }
}

// Fonction pour poser des questions ou soumettre des préoccupations
void poserQuestions() {
    char id[20];
    printf("Entrez l'Identite du patient : ");
    scanf("%19s", id);
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) {
            i = j;
            break;
        }
    }
    if (i != -1) {
        char question[200];
        printf("\nPosez votre question si vous avez une preoccupation pour %s %s :\n", patients[i].nom, patients[i].prenom);
        printf("Votre question : ");
        scanf(" %199[^\n]", question);
        sauvegarderPatientsVersTXT();
        printf("Question envoyee au medecin.\n");

    } else {
        printf("Patient non trouve.\n");
    }
}

void statistiquesPatients() {
    clear();
    printf("\n=== STATISTIQUES GENERALES ===\n\n");

    if (nbPatients == 0) {
        printf("Aucun patient enregistre.\n");
        pause();
        return;
    }

    // 1. Statistiques de base
    printf("Nombre total de patients : %d\n", nbPatients);
    afficherAgeMoyen();
    afficherRendezVousMoyen();

    // 2. Répartition par genre
    int hommes = 0, femmes = 0;
    for (int i = 0; i < nbPatients; i++) {
        if (strcasecmp(patients[i].genre, "M") == 0) {
            hommes++;
        } else {
            femmes++;
        }
    }
    printf("\nRepartition par genre :\n");
    printf("- Hommes : %d (%.1f%%)\n", hommes, (float)hommes/nbPatients*100);
    printf("- Femmes : %d (%.1f%%)\n", femmes, (float)femmes/nbPatients*100);

    pause();
}
// Fonction pour calculer l'âge d'un patient
int calculerAge(const char* date_naissance) {
    int jour, mois, annee;
    sscanf(date_naissance, "%d/%d/%d", &jour, &mois, &annee);
    return 2025 - annee; //  2025  l'année actuelle
}

// Fonction pour calculer l'âge moyen
void afficherAgeMoyen() {
    if (nbPatients == 0) {
        printf("Aucun patient enregistre.\n");
        return;
    }

    int sommeAges = 0;
    for (int i = 0; i < nbPatients; i++) {
        sommeAges += calculerAge(patients[i].date_naissance);
    }

    printf("Age moyen des patients : %.1f ans\n", (float)sommeAges/nbPatients);
}

// Fonction pour calculer le nombre moyen de rendez-vous
void afficherRendezVousMoyen() {
    if (nbPatients == 0) {
        printf("Aucun patient enregistre.\n");
        return;
    }

    int totalRdv = 0;
    for (int i = 0; i < nbPatients; i++) {
        totalRdv += patients[i].nbRendezVous;
    }

    printf("Nombre moyen de rendez-vous par patient : %.1f\n", (float)totalRdv/nbPatients);
}

// Menu des statistiques
void menuStatistiques() {
    char mdp[20];
    printf("Mot de passe : ");
    scanf("%19s", mdp);
    if (strcmp(mdp, PASSWORD_STA) != 0) {
        printf("Mot de passe incorrect.\n");
        return;
    }
    int choix;
    do {
        clear();
        printf("=== MENU STATISTIQUES ===\n");
        printf("1. Statistiques generales\n");
        printf("2. Age moyen des patients\n");
        printf("3. Nombre moyen de rendez-vous\n");
        printf("0. Retour\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                statistiquesPatients();
                break;
            case 2:
                clear();
                printf("=== AGE MOYEN ===\n");
                afficherAgeMoyen();
                pause();
                break;
            case 3:
                clear();
                printf("=== RENDEZ-VOUS MOYENS ===\n");
                afficherRendezVousMoyen();
                pause();
                break;
        }
    } while (choix != 0);
}

// Menu Patient
void menuPatient() {
    int choix;
    do {
        clear();
        printf("=== MENU PATIENT ===\n");
        printf("1. Consultez votre dossier medical\n2. Prendre un rendez-vous\n3. Voir les rendez-vous a venir\n4. Annuler ou modifier un rendez-vous\n5. Mettre a jour les informations personnelles\n6. Suivre les prescriptions de medicaments\n7. Poser des questions ou preoccupations\n0. Retour\nChoix : ");
        scanf("%d", &choix);
        switch (choix) {
            case 1: consulterDossier(); break;
            case 2: prendreRendezVous(); break;
            case 3: voirRendezVous(); break;
            case 4: annulerModifierRendezVous(); break;
            case 5: mettreAJourInfos(); break;
            case 6: suivreMeds(); break;
            case 7: poserQuestions(); break;
        }
        if (choix != 0) pause();
    } while (choix != 0);
}

// --- Main ---
int main() {
    chargerPatientsDepuisTXT();// Charge les données au démarrage

    int choix;
    do {
        clear();
        printf("             === MENU PRINCIPAL ===\n");
        printf("\n====================================================\n");
        printf("    BIENVENUE DANS LE SYSTEME DE GESTION MEDICALE\n");
        printf("====================================================\n");
        printf("-> Securise | Simple d utilisation | Optimise\n");
        printf("Ce programme a ete developpe pour permettre aux professionnels\n");
        printf("de la sante de gerer efficacement les dossiers de leurs patients.\n");
        printf("Les donnees sont enregistrees automatiquement dans un fichier en txt\n");
        printf("afin d'assurer la sauvegarde et la recuperation des dossiers.\n\n");
        printf("REALISE PAR : ABDIRAHIM MAHAMOUD ABDILLAH\n");
        printf("     SARAH MAIZAR SAID ET ZEINAB YLYAS ABANEH\n");
        printf("Encadrement : Mr.Mohamed Hamid Mohamed\n");
        printf("Date : Avril 2025\n");
        printf("====================================================\n");
        printf("1. Administration\n2. Medecin\n3. Patient\n4. Statistiques\n0. Quitter\nChoix : ");
        scanf("%d", &choix);
        switch (choix) {
            case 1: menuAdministration(); break;
            case 2: menuMedecin(); break;
            case 3: menuPatient(); break;
            case 4: menuStatistiques();break;
        }
    } while (choix != 0);
    // Sauvegarde finale avant de quitter
    sauvegarderPatientsVersTXT();
    return 0;
}
