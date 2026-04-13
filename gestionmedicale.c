/* =============================================================================
 * Medical Management System
 * =============================================================================
 * Authors  : AbdiR-pixel, Zya

 * Date     : April 2025
 *
 * Description:
 *   A console-based medical management system written in C.
 *   Supports three roles: Administrator, Doctor, and Patient.
 *   All patient data and user accounts are persisted in text files
 *   to ensure backup and recovery of records across sessions.
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* ----------------------------- Constants ---------------------------------- */

#define PASSWORD_MEDECIN "med123"   /* Default password for doctors           */
#define PASSWORD_ADMIN   "admin123" /* Default password for administrators    */
#define PASSWORD_STA     "sta123"   /* Default password for statistics access */
#define MAX_PATIENTS     100        /* Maximum number of patients in the system*/
#define MAX_RENDEZVOUS   100        /* Maximum appointments per patient        */
#define MAX_EXAMENS      100        /* Maximum additional exams per patient    */
#define MAX_USERS        100        /* Maximum number of user accounts         */

/* ----------------------------- Data Structures ---------------------------- */

/* Represents a single medical appointment (date, time, and purpose) */
typedef struct {
    char date[20];    /* Appointment date (dd/mm/yyyy)      */
    char heure[10];   /* Appointment time (hh:mm)           */
    char objet[100];  /* Purpose / reason for appointment   */
} RendezVous;

/* Represents a full patient record with personal, medical, and appointment info */
typedef struct {
    char id[20];                          /* Unique patient identifier           */
    char nom[50];                         /* Last name                           */
    char prenom[50];                      /* First name                          */
    char date_naissance[20];             /* Date of birth (dd/mm/yyyy)          */
    char genre[10];                       /* Gender (M / F)                      */
    char telephone[20];                   /* Phone number                        */
    char adresse[100];                    /* Home address                        */
    char categorie[50];                   /* Payment category: Paying or CNSS    */
    char service[100];                    /* Medical department / service        */
    char situation_familiale[20];        /* Marital status: Married / Single    */
    char fonctionnalite[200];            /* Patient's functional status / notes */
    bool estDecede;                       /* True if the patient is deceased     */
    RendezVous rendezvous[MAX_RENDEZVOUS];/* List of scheduled appointments      */
    int nbRendezVous;                     /* Number of appointments recorded     */
    char diagnostic[200];                 /* Doctor's diagnosis                  */
    char traitement[200];                 /* Prescribed treatment                */
    char observations[200];              /* Clinical observations               */
    char antecedents[200];               /* Medical history / past conditions   */
    char examens_complementaires[200];   /* Additional / complementary exams    */
    char analyse_sang[200];              /* Blood test results                  */
} Patient;

/* Global array of patients and counter */
Patient patients[MAX_PATIENTS];
int nbPatients = 0;

/* Represents a system user account (admin, doctor, or staff) */
typedef struct {
    char id[20];       /* Username / login ID      */
    char password[20]; /* Account password         */
    char role[10];     /* Role: "admin", "med", or "staff" */
} Utilisateur;

/* Global array of user accounts and counter */
Utilisateur utilisateurs[MAX_USERS];
int nbUtilisateurs = 0;

/* =========================================================================
 * UTILITY FUNCTIONS
 * ========================================================================= */

/* Clears the terminal screen using ANSI escape codes */
void clear() {
    printf("\033[H\033[J");
}

/* Pauses execution and waits for the user to press Enter before continuing */
void pause() {
    printf("\nPress Enter to continue...");
    while(getchar() != '\n'); /* Flush the input buffer */
    getchar();               /* Wait for the user to press Enter */
}

/* =========================================================================
 * USER ACCOUNT MANAGEMENT
 * ========================================================================= */

/* Loads all user accounts from "users.txt" into the global users array.
 * Each line in the file follows the format: id,password,role
 * This function is typically called once at program startup. */
void chargerUtilisateurs() {
    FILE *f = fopen("users.txt", "r");
    if (!f) return; /* File does not exist yet — nothing to load */

    nbUtilisateurs = 0;
    /* Read each user record line by line */
    while (fscanf(f, "%19[^,],%19[^,],%9s\n",
                  utilisateurs[nbUtilisateurs].id,
                  utilisateurs[nbUtilisateurs].password,
                  utilisateurs[nbUtilisateurs].role) == 3) {
        nbUtilisateurs++;
    }
    fclose(f);
}

/* Saves all user accounts from memory to "users.txt".
 * Overwrites the file completely to reflect the current state.
 * Called after any change to user accounts (add, delete, password change). */
void sauvegarderUtilisateurs() {
    FILE *f = fopen("users.txt", "w");
    if (!f) return; /* Could not open file for writing */

    for (int i = 0; i < nbUtilisateurs; i++) {
        fprintf(f, "%s,%s,%s\n",
                utilisateurs[i].id,
                utilisateurs[i].password,
                utilisateurs[i].role);
    }
    fclose(f);
}

/* Verifies a user's identity by checking ID, password, and role.
 * Returns 1 if a matching user is found, 0 otherwise. */
int verifierUtilisateur(const char* id, const char* password, const char* role) {
    for (int i = 0; i < nbUtilisateurs; i++) {
        if (strcmp(utilisateurs[i].id,       id)       == 0 &&
            strcmp(utilisateurs[i].password, password) == 0 &&
            strcmp(utilisateurs[i].role,     role)     == 0) {
            return 1; /* Match found — user is authenticated */
        }
    }
    return 0; /* No match found — authentication failed */
}

/* Logs a timestamped action to "log.txt" for audit purposes.
 * Parameters:
 *   user   - the username or role performing the action
 *   action - a short description of what was done */
void logger(const char* user, const char* action) {
    FILE *f = fopen("log.txt", "a"); /* Open in append mode to preserve history */
    if (!f) return;

    time_t now = time(NULL);
    /* Format: [Day Mon DD HH:MM:SS YYYY] username: action */
    fprintf(f, "[%s] %s: %s\n", strtok(ctime(&now), "\n"), user, action);
    fclose(f);
}

/* Adds a new user account to the system (Admin only).
 * Prompts for a new user ID, password, and role, then saves to file. */
void ajouterUtilisateur() {
    if (nbUtilisateurs >= MAX_USERS) {
        printf("User limit reached.\n");
        return;
    }

    Utilisateur u;
    printf("New user ID: ");
    scanf("%19s", u.id);
    printf("Password: ");
    scanf("%19s", u.password);
    printf("Role (admin/med/staff): ");
    scanf("%9s", u.role);

    utilisateurs[nbUtilisateurs++] = u; /* Add to in-memory array */
    sauvegarderUtilisateurs();          /* Persist to file immediately */
    printf("User added.\n");
    logger("ADMIN", "Added a new user account");
}

/* Deletes a user account from the system by ID.
 * Shifts remaining users left in the array and saves the updated list. */
void supprimerUtilisateur() {
    char id[20];
    printf("ID of user to delete: ");
    scanf("%19s", id);

    for (int i = 0; i < nbUtilisateurs; i++) {
        if (strcmp(utilisateurs[i].id, id) == 0) {
            /* Shift all subsequent users one position to the left */
            for (int j = i; j < nbUtilisateurs - 1; j++) {
                utilisateurs[j] = utilisateurs[j+1];
            }
            nbUtilisateurs--;
            sauvegarderUtilisateurs();
            printf("User deleted.\n");
            logger("ADMIN", "Deleted a user account");
            return;
        }
    }
    printf("User not found.\n");
}

/* Allows a user to change their own password after verifying their old one.
 * Prompts for user ID, current password, and new password. */
void changerMotDePasse() {
    char id[20], oldpass[20], newpass[20];

    printf("ID: ");      scanf("%19s", id);
    printf("Current password: "); scanf("%19s", oldpass);

    for (int i = 0; i < nbUtilisateurs; i++) {
        if (strcmp(utilisateurs[i].id,       id)      == 0 &&
            strcmp(utilisateurs[i].password, oldpass) == 0) {
            printf("New password: ");
            scanf("%19s", newpass);
            strcpy(utilisateurs[i].password, newpass); /* Update in memory */
            sauvegarderUtilisateurs();                 /* Persist to file  */
            printf("Password updated successfully.\n");
            logger(id, "Changed password");
            return;
        }
    }
    printf("Incorrect user ID or password.\n");
}

/* =========================================================================
 * DATA PERSISTENCE — PATIENT FILE I/O
 * =========================================================================
 * All patient data is stored in "patients.txt" using a structured text format.
 * Each patient block begins with [Patient] and ends with [FinPatient].
 * This ensures backup and recovery of records across program sessions.
 * ========================================================================= */

/* Saves all patient records to "patients.txt".
 * Each patient's fields are written as key=value pairs inside [Patient] blocks.
 * Appointments are saved as sequential RendezVous_Date / Heure / Objet entries.
 * Called automatically after every modification to ensure data is never lost. */
void sauvegarderPatientsVersTXT() {
    FILE *fichier = fopen("patients.txt", "w");
    if (fichier == NULL) {
        printf("Error: Could not create backup file!\n");
        return;
    }

    for (int i = 0; i < nbPatients; i++) {
        fprintf(fichier, "[Patient]\n");
        fprintf(fichier, "Identite=%s\n",          patients[i].id);
        fprintf(fichier, "Nom=%s\n",               patients[i].nom);
        fprintf(fichier, "Prenom=%s\n",            patients[i].prenom);
        fprintf(fichier, "DateNaissance=%s\n",     patients[i].date_naissance);
        fprintf(fichier, "Genre=%s\n",             patients[i].genre);
        fprintf(fichier, "Telephone=%s\n",         patients[i].telephone);
        fprintf(fichier, "Adresse=%s\n",           patients[i].adresse);
        fprintf(fichier, "Categorie=%s\n",         patients[i].categorie);
        fprintf(fichier, "Service=%s\n",           patients[i].service);
        fprintf(fichier, "SituationFamiliale=%s\n",patients[i].situation_familiale);
        fprintf(fichier, "Fonctionnalite=%s\n",    patients[i].fonctionnalite);
        fprintf(fichier, "Decede=%d\n",            patients[i].estDecede);
        fprintf(fichier, "Diagnostic=%s\n",        patients[i].diagnostic);
        fprintf(fichier, "Traitement=%s\n",        patients[i].traitement);
        fprintf(fichier, "Observations=%s\n",      patients[i].observations);
        fprintf(fichier, "Antecedents=%s\n",       patients[i].antecedents);
        fprintf(fichier, "Examens=%s\n",           patients[i].examens_complementaires);
        fprintf(fichier, "AnalyseSang=%s\n",       patients[i].analyse_sang);

        /* Save all appointments for this patient */
        fprintf(fichier, "NbRendezVous=%d\n", patients[i].nbRendezVous);
        for (int j = 0; j < patients[i].nbRendezVous; j++) {
            fprintf(fichier, "RendezVous_Date=%s\n",  patients[i].rendezvous[j].date);
            fprintf(fichier, "RendezVous_Heure=%s\n", patients[i].rendezvous[j].heure);
            fprintf(fichier, "RendezVous_Objet=%s\n", patients[i].rendezvous[j].objet);
        }
        fprintf(fichier, "[FinPatient]\n\n"); /* End-of-patient marker */
    }
    fclose(fichier);
}

/* Loads all patient records from "patients.txt" into memory.
 * Reads each [Patient] block line by line and reconstructs the Patient structs.
 * Called once at program startup to restore data from the last session. */
void chargerPatientsDepuisTXT() {
    FILE *fichier = fopen("patients.txt", "r");
    if (fichier == NULL) {
        /* No existing file — this is likely the first run */
        printf("No saved data found. Starting with an empty record.\n");
        return;
    }

    nbPatients = 0;
    char ligne[256];
    Patient *patientActuel = NULL; /* Pointer to the patient currently being loaded */

    while (fgets(ligne, sizeof(ligne), fichier)) {
        ligne[strcspn(ligne, "\n")] = 0; /* Strip trailing newline character */

        if (strcmp(ligne, "[Patient]") == 0) {
            /* Start of a new patient block — allocate a slot in the array */
            if (nbPatients < MAX_PATIENTS) {
                patientActuel = &patients[nbPatients];
                memset(patientActuel, 0, sizeof(Patient)); /* Zero-initialize all fields */
                nbPatients++;
            }
        }
        else if (patientActuel != NULL) {
            /* Parse a key=value pair and assign it to the correct field */
            char *cle    = strtok(ligne, "="); /* Key   (left of '=') */
            char *valeur = strtok(NULL,  "="); /* Value (right of '=') */

            if (cle != NULL && valeur != NULL) {
                if      (strcmp(cle, "ID")               == 0) strcpy(patientActuel->id,                   valeur);
                else if (strcmp(cle, "Nom")              == 0) strcpy(patientActuel->nom,                  valeur);
                else if (strcmp(cle, "Prenom")           == 0) strcpy(patientActuel->prenom,               valeur);
                else if (strcmp(cle, "DateNaissance")    == 0) strcpy(patientActuel->date_naissance,       valeur);
                else if (strcmp(cle, "Genre")            == 0) strcpy(patientActuel->genre,                valeur);
                else if (strcmp(cle, "Telephone")        == 0) strcpy(patientActuel->telephone,            valeur);
                else if (strcmp(cle, "Adresse")          == 0) strcpy(patientActuel->adresse,              valeur);
                else if (strcmp(cle, "Categorie")        == 0) strcpy(patientActuel->categorie,            valeur);
                else if (strcmp(cle, "Service")          == 0) strcpy(patientActuel->service,              valeur);
                else if (strcmp(cle, "SituationFamiliale")== 0)strcpy(patientActuel->situation_familiale,  valeur);
                else if (strcmp(cle, "Fonctionnalite")   == 0) strcpy(patientActuel->fonctionnalite,       valeur);
                else if (strcmp(cle, "Decede")           == 0) patientActuel->estDecede = atoi(valeur);
                else if (strcmp(cle, "Diagnostic")       == 0) strcpy(patientActuel->diagnostic,           valeur);
                else if (strcmp(cle, "Traitement")       == 0) strcpy(patientActuel->traitement,           valeur);
                else if (strcmp(cle, "Observations")     == 0) strcpy(patientActuel->observations,         valeur);
                else if (strcmp(cle, "Antecedents")      == 0) strcpy(patientActuel->antecedents,          valeur);
                else if (strcmp(cle, "Examens")          == 0) strcpy(patientActuel->examens_complementaires, valeur);
                else if (strcmp(cle, "AnalyseSang")      == 0) strcpy(patientActuel->analyse_sang,         valeur);
                else if (strcmp(cle, "NbRendezVous")     == 0) patientActuel->nbRendezVous = atoi(valeur);
                /* Load appointment sub-fields using the "RendezVous_" prefix */
                else if (strncmp(cle, "RendezVous_", 11) == 0 && patientActuel->nbRendezVous > 0) {
                    int index = patientActuel->nbRendezVous - 1; /* Index of the current appointment */
                    if      (strcmp(cle+11, "Date")  == 0) strcpy(patientActuel->rendezvous[index].date,  valeur);
                    else if (strcmp(cle+11, "Heure") == 0) strcpy(patientActuel->rendezvous[index].heure, valeur);
                    else if (strcmp(cle+11, "Objet") == 0) strcpy(patientActuel->rendezvous[index].objet, valeur);
                }
            }
        }
    }
    fclose(fichier);
}

/* =========================================================================
 * SERVICE SELECTION
 * ========================================================================= */

/* Displays a numbered list of available medical departments and lets the user
 * choose one. The selected department name is written into the 'service' buffer.
 * If an invalid number is entered, defaults to "General Medicine". */
void choisirService(char* service) {
    int choice;

    printf("\n=== LIST OF CONSULTATIONS ===\n");
    printf(" 1. Pediatrics\n");
    printf(" 2. Dialysis\n");
    printf(" 3. Cardiology\n");
    printf(" 4. Radiology\n");
    printf(" 5. Emergency\n");
    printf(" 6. General Surgery\n");
    printf(" 7. Dermatology\n");
    printf(" 8. Gynecology\n");
    printf(" 9. Ophthalmology\n");
    printf("10. ENT (Ear, Nose, Throat)\n");
    printf("11. Neurology\n");
    printf("12. Pulmonology\n");
    printf("13. Endocrinology\n");
    printf("14. Gastroenterology\n");
    printf("15. Rheumatology\n");
    printf("Choose (1-15): ");

    scanf("%d", &choice);
    while(getchar() != '\n'); /* Flush the input buffer after reading the integer */

    /* Map the numeric choice to the corresponding department name */
    switch(choice) {
        case 1:  strcpy(service, "Pediatrics");          break;
        case 2:  strcpy(service, "Dialysis");            break;
        case 3:  strcpy(service, "Cardiology");          break;
        case 4:  strcpy(service, "Radiology");           break;
        case 5:  strcpy(service, "Emergency");           break;
        case 6:  strcpy(service, "General Surgery");     break;
        case 7:  strcpy(service, "Dermatology");         break;
        case 8:  strcpy(service, "Gynecology");          break;
        case 9:  strcpy(service, "Ophthalmology");       break;
        case 10: strcpy(service, "ENT");                 break;
        case 11: strcpy(service, "Neurology");           break;
        case 12: strcpy(service, "Pulmonology");         break;
        case 13: strcpy(service, "Endocrinology");       break;
        case 14: strcpy(service, "Gastroenterology");    break;
        case 15: strcpy(service, "Rheumatology");        break;
        default:
            printf("Invalid choice. Defaulting to General Medicine.\n");
            strcpy(service, "General Medicine");
    }
}

/* =========================================================================
 * PATIENT CRUD OPERATIONS (Admin)
 * ========================================================================= */

/* Registers a new patient by collecting all personal and medical details.
 * Appends the patient to the global array and saves immediately to file.
 * Medical fields (diagnosis, treatment, etc.) are initialized as empty strings. */
void ajouterPatient() {
    if (nbPatients >= MAX_PATIENTS) {
        printf("Patient limit reached.\n");
        return;
    }

    Patient p;
    printf("\n=== Add a Patient ===\n");

    printf("ID: ");               scanf("%19s", p.id);              while(getchar() != '\n');
    printf("Last name: ");        scanf("%49s", p.nom);             while(getchar() != '\n');
    printf("First name: ");
    fgets(p.prenom, sizeof(p.prenom), stdin);
    p.prenom[strcspn(p.prenom, "\n")] = '\0'; /* Remove trailing newline */

    printf("Date of birth (dd/mm/yyyy): "); scanf("%19s", p.date_naissance);
    printf("Gender (M/F): ");               scanf("%9s",  p.genre);  while(getchar() != '\n');
    printf("Phone number: ");               scanf("%19s", p.telephone); while(getchar() != '\n');
    printf("Address: ");                    scanf(" %99[^\n]", p.adresse); while(getchar() != '\n');

    /* Determine payment category */
    printf("Category (p = Paying, c = CNSS): ");
    char choixa;
    scanf(" %c", &choixa);

    if (choixa == 'p' || choixa == 'P') {
        strcpy(p.categorie, "Paying");
    } else {
        /* CNSS patients must provide their CNSS number */
        strcpy(p.categorie, "CNSS");
        printf("CNSS number (9 digits): ");
        char num[20];
        scanf("%19s", num);
        strcat(p.categorie, " ");
        strcat(p.categorie, num); /* Append CNSS number to category field */
    }

    printf("\nType of consultation:");
    choisirService(p.service); /* Let user pick a medical department */

    printf("Marital status (married/single): ");
    scanf("%19s", p.situation_familiale);
    printf("Patient functional status: ");
    scanf(" %199[^\n]", p.fonctionnalite);

    /* Initialize medical fields and appointment counter */
    p.estDecede    = false;
    p.nbRendezVous = 0;
    strcpy(p.diagnostic,             "");
    strcpy(p.traitement,             "");
    strcpy(p.observations,           "");
    strcpy(p.antecedents,            "");
    strcpy(p.examens_complementaires,"");
    strcpy(p.analyse_sang,           "");

    patients[nbPatients++] = p; /* Add patient to the global array */
    sauvegarderPatientsVersTXT();
    printf("Patient added successfully.\n");
}

/* Modifies an existing patient's personal information by their ID.
 * Searches for the patient and prompts for updated field values, then saves. */
void modifierPatient() {
    char id[20];
    printf("ID of patient to modify: ");
    scanf("%19s", id);

    /* Search for patient by ID */
    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("Editing record for %s %s\n", patients[i].nom, patients[i].prenom);

        printf("Last name: ");       scanf("%49s", patients[i].nom);
        printf("First name: ");
        fgets(patients[i].prenom, sizeof(patients[i].prenom), stdin);
        patients[i].prenom[strcspn(patients[i].prenom, "\n")] = '\0';

        printf("Date of birth (dd/mm/yyyy): "); scanf("%19s", patients[i].date_naissance); while(getchar() != '\n');
        printf("Gender: ");                     scanf("%9s",  patients[i].genre);          while(getchar() != '\n');
        printf("Phone number: ");               scanf("%19s", patients[i].telephone);      while(getchar() != '\n');
        printf("Address: ");                    scanf(" %99[^\n]", patients[i].adresse);   while(getchar() != '\n');

        /* Update payment category */
        printf("New category (p = Paying, c = CNSS): ");
        char choix;
        scanf(" %c", &choix);

        if (choix == 'p' || choix == 'P') {
            strcpy(patients[i].categorie, "Paying");
        } else {
            strcpy(patients[i].categorie, "CNSS");
            printf("CNSS number: ");
            char num[20];
            scanf("%19s", num);
            strcat(patients[i].categorie, " ");
            strcat(patients[i].categorie, num);
        }

        printf("Hospitalization department: ");    scanf(" %99[^\n]", patients[i].service);
        printf("Marital status (married/single): "); scanf("%19s", patients[i].situation_familiale); while(getchar() != '\n');
        printf("Patient functional status: ");      scanf(" %199[^\n]", patients[i].fonctionnalite); while(getchar() != '\n');

        sauvegarderPatientsVersTXT();
        printf("Patient updated successfully.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* Deletes a patient record by ID.
 * Fills the gap by shifting subsequent patients left, then saves the updated list. */
void supprimerPatient() {
    char id[20];
    printf("ID of patient to delete: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        /* Shift all patients after index i one position to the left */
        for (int j = i; j < nbPatients - 1; j++) {
            patients[j] = patients[j + 1];
        }
        nbPatients--;
        sauvegarderPatientsVersTXT();
        printf("Patient deleted successfully.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* =========================================================================
 * APPOINTMENT MANAGEMENT
 * ========================================================================= */

/* Schedules a new appointment for a patient (used by admin).
 * Prompts for patient ID, date, time, and purpose, then appends the appointment. */
void programmerRendezVous() {
    char id[20];
    printf("Patient ID for appointment: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        RendezVous rdv;
        printf("Appointment date (dd/mm/yyyy): "); scanf("%19s", rdv.date);
        printf("Appointment time: ");              scanf("%9s",  rdv.heure);
        printf("Purpose of appointment: ");        scanf(" %99[^\n]", rdv.objet);

        /* Add appointment to patient's list and save */
        patients[i].rendezvous[patients[i].nbRendezVous++] = rdv;
        sauvegarderPatientsVersTXT();
        printf("Appointment scheduled successfully.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* =========================================================================
 * PATIENT RECORD CONSULTATION (Admin View)
 * ========================================================================= */

/* Displays the basic personal record of a patient (without medical details).
 * Used by the Administration menu. */
void consulterDossierPatient() {
    char id[20];
    printf("ID of patient to view: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("\n=== Complete Record ===\n");
        printf("ID: %s\nLast name: %s\nFirst name: %s\nDate of birth: %s\n"
               "Gender: %s\nPhone: %s\nAddress: %s\nCategory: %s\n"
               "Service: %s\nMarital status: %s\nFunctional status: %s\nDeceased: %s\n",
               patients[i].id, patients[i].nom, patients[i].prenom,
               patients[i].date_naissance, patients[i].genre,
               patients[i].telephone, patients[i].adresse,
               patients[i].categorie, patients[i].service,
               patients[i].situation_familiale, patients[i].fonctionnalite,
               patients[i].estDecede ? "Yes" : "No");
    } else {
        printf("Patient not found.\n");
    }
}

/* Displays the full medical record of a patient (including diagnosis, treatment,
 * blood tests, etc.). This view is intended for use by doctors. */
void consulterDossierPatientpourmed() {
    char id[20];
    printf("ID of patient to view: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("\n=== Full Medical Record ===\n");
        printf("ID: %s\nLast name: %s\nFirst name: %s\nDate of birth: %s\n"
               "Gender: %s\nPhone: %s\nAddress: %s\nCategory: %s\n"
               "Service: %s\nMarital status: %s\nFunctional status: %s\n"
               "Diagnosis: %s\nTreatment: %s\nObservations: %s\n"
               "Medical history: %s\nBlood test: %s\n"
               "Complementary exams: %s\nDeceased: %s\n",
               patients[i].id, patients[i].nom, patients[i].prenom,
               patients[i].date_naissance, patients[i].genre,
               patients[i].telephone, patients[i].adresse,
               patients[i].categorie, patients[i].service,
               patients[i].situation_familiale, patients[i].fonctionnalite,
               patients[i].diagnostic, patients[i].traitement,
               patients[i].observations, patients[i].antecedents,
               patients[i].analyse_sang, patients[i].examens_complementaires,
               patients[i].estDecede ? "Yes" : "No");
    } else {
        printf("Patient not found.\n");
    }
}

/* Prints a summary list of all registered patients (ID, last name, first name). */
void afficherTousPatients() {
    printf("\n=== List of All Patients ===\n");
    for (int i = 0; i < nbPatients; i++) {
        printf("%s - %s %s\n", patients[i].id, patients[i].nom, patients[i].prenom);
    }
}

/* Prints all scheduled appointments for all patients. */
void afficherRendezVous() {
    printf("\n=== List of Scheduled Appointments ===\n");
    for (int i = 0; i < nbPatients; i++) {
        printf("Patient: %s %s\n", patients[i].nom, patients[i].prenom);
        for (int j = 0; j < patients[i].nbRendezVous; j++) {
            printf("  Date: %s, Time: %s, Purpose: %s\n",
                   patients[i].rendezvous[j].date,
                   patients[i].rendezvous[j].heure,
                   patients[i].rendezvous[j].objet);
        }
    }
}

/* =========================================================================
 * ADMINISTRATION MENU
 * ========================================================================= */

/* Displays the Administration menu after password verification.
 * Provides full access to all patient and appointment management operations. */
void menuAdministration() {
    char mdp[20];
    printf("Password: ");
    scanf("%19s", mdp);

    /* Verify admin password before granting access */
    if (strcmp(mdp, PASSWORD_ADMIN) != 0) {
        printf("Incorrect password.\n");
        return;
    }

    int choix;
    do {
        clear();
        printf("=== ADMINISTRATION MENU ===\n");
        printf("1. Add a patient\n");
        printf("2. Edit a patient\n");
        printf("3. Delete a patient\n");
        printf("4. Schedule an appointment\n");
        printf("5. View patient record\n");
        printf("6. View all patients\n");
        printf("7. View all appointments\n");
        printf("8. Save data manually\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: ajouterPatient();          break;
            case 2: modifierPatient();         break;
            case 3: supprimerPatient();        break;
            case 4: programmerRendezVous();    break;
            case 5: consulterDossierPatient(); break;
            case 6: afficherTousPatients();    break;
            case 7: afficherRendezVous();      break;
            case 8: sauvegarderPatientsVersTXT(); break; /* Manual save option */
        }
        if (choix != 0) pause();
    } while (choix != 0);
}

/* =========================================================================
 * DOCTOR FUNCTIONS
 * ========================================================================= */

/* Prints a compact list of all patients (ID, last name, first name).
 * Used in the Doctor menu for quick reference. */
void afficherPatients() {
    printf("\n--- Patient List ---\n");
    for (int i = 0; i < nbPatients; i++) {
        printf("ID: %s - Name: %s %s\n",
               patients[i].id, patients[i].nom, patients[i].prenom);
    }
}

/* Adds or updates a diagnosis for a patient identified by ID.
 * Only available in the Doctor menu. Saves immediately after update. */
void ajouterModifierDiagnostic() {
    char id[20];
    printf("Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("Add or update diagnosis for %s %s:\n", patients[i].nom, patients[i].prenom);
        printf("Diagnosis: ");
        scanf(" %199[^\n]", patients[i].diagnostic);
        sauvegarderPatientsVersTXT();
        printf("Diagnosis updated.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* Prescribes a treatment for a patient identified by ID.
 * Updates the treatment field and saves to file. */
void prescrireTraitement() {
    char id[20];
    printf("Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("Prescribe treatment for %s %s:\n", patients[i].nom, patients[i].prenom);
        printf("Treatment: ");
        scanf(" %199[^\n]", patients[i].traitement);
        sauvegarderPatientsVersTXT();
        printf("Treatment prescribed.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* Records clinical observations for a patient identified by ID. */
void ajouterObservations() {
    char id[20];
    printf("Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("Add clinical observations for %s %s:\n", patients[i].nom, patients[i].prenom);
        printf("Observations: ");
        scanf(" %199[^\n]", patients[i].observations);
        sauvegarderPatientsVersTXT();
        printf("Clinical observations added.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* Records medical history / past conditions for a patient identified by ID. */
void ajouterAntecedents() {
    char id[20];
    printf("Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("Add medical history for %s %s:\n", patients[i].nom, patients[i].prenom);
        printf("Medical history: ");
        scanf(" %199[^\n]", patients[i].antecedents);
        sauvegarderPatientsVersTXT();
        printf("Medical history added.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* Records additional/complementary examination results for a patient. */
void ajouterExamensComplementaires() {
    char id[20];
    printf("Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("Add complementary exams for %s %s:\n", patients[i].nom, patients[i].prenom);
        printf("Exams: ");
        scanf(" %199[^\n]", patients[i].examens_complementaires);
        sauvegarderPatientsVersTXT();
        printf("Complementary exams added.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* Records blood test results for a patient identified by ID. */
void ajouterAnalyseSang() {
    char id[20];
    printf("Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("Add blood test results for %s %s:\n", patients[i].nom, patients[i].prenom);
        printf("Blood test: ");
        scanf(" %199[^\n]", patients[i].analyse_sang);
        sauvegarderPatientsVersTXT();
        printf("Blood test results added.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* Marks a patient as deceased by setting their estDecede flag to true. */
void marquerCommeDecede() {
    char id[20];
    printf("Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        patients[i].estDecede = true;
        sauvegarderPatientsVersTXT();
        printf("Patient marked as deceased.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* =========================================================================
 * DOCTOR MENU
 * ========================================================================= */

/* Displays the Doctor menu after password verification.
 * Doctors can view patient records and manage medical information,
 * but cannot delete or modify personal patient data. */
void menuMedecin() {
    char mdp[20];
    printf("Password: ");
    scanf("%19s", mdp);

    /* Verify doctor password before granting access */
    if (strcmp(mdp, PASSWORD_MEDECIN) != 0) {
        printf("Incorrect password.\n");
        return;
    }

    int choix;
    do {
        clear();
        printf("=== DOCTOR MENU ===\n");
        printf(" 1. View all patients\n");
        printf(" 2. View full patient record\n");
        printf(" 3. Add / update diagnosis\n");
        printf(" 4. Prescribe treatment\n");
        printf(" 5. Add clinical observations\n");
        printf(" 6. Add medical history\n");
        printf(" 7. Add complementary exams\n");
        printf(" 8. Add blood test results\n");
        printf(" 9. Schedule an appointment\n");
        printf("10. Mark patient as deceased\n");
        printf(" 0. Back\n");
        printf("Your choice: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:  afficherPatients();               break;
            case 2:  consulterDossierPatientpourmed(); break;
            case 3:  ajouterModifierDiagnostic();      break;
            case 4:  prescrireTraitement();            break;
            case 5:  ajouterObservations();            break;
            case 6:  ajouterAntecedents();             break;
            case 7:  ajouterExamensComplementaires();  break;
            case 8:  ajouterAnalyseSang();             break;
            case 9:  programmerRendezVous();           break;
            case 10: marquerCommeDecede();             break;
        }
        if (choix != 0) pause();
    } while (choix != 0);
}

/* =========================================================================
 * PATIENT SELF-SERVICE FUNCTIONS
 * ========================================================================= */

/* Displays the medical record summary for a patient identified by their ID.
 * Shows personal info, medical service, treatment, status, and appointments.
 * This is the patient's own view — no password required, only their ID. */
void consulterDossier() {
    char id[20];
    printf("\nEnter your Patient ID: ");
    if (scanf("%19s", id) != 1) {
        printf("Input error.\n");
        while (getchar() != '\n'); /* Flush the input buffer */
        return;
    }

    /* Search for the patient by ID */
    int patientIndex = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { patientIndex = j; break; }
    }

    if (patientIndex == -1) {
        printf("\nPatient not found.\n");
        pause();
        return;
    }

    /* Calculate patient's age from the year of birth in their date field */
    int anneeNaissance;
    if (sscanf(patients[patientIndex].date_naissance + 6, "%d", &anneeNaissance) != 1) {
        printf("Error: Invalid date format.\n");
        anneeNaissance = 0;
    }
    int age = 2025 - anneeNaissance; /* Approximate age based on birth year */

    /* Display formatted medical record */
    printf("\n=== MEDICAL RECORD ===\n");
    printf("=== %s %s ===\n\n", patients[patientIndex].nom, patients[patientIndex].prenom);

    printf("* Personal Information:\n");
    printf("  ID:      %s\n",  patients[patientIndex].id);
    printf("  Age:     %d years\n", age);
    printf("  Gender:  %s\n",  patients[patientIndex].genre);
    printf("  Phone:   %s\n",  patients[patientIndex].telephone);
    printf("  Address: %s\n\n",patients[patientIndex].adresse);

    printf("* Medical Information:\n");
    printf("  Department: %s\n",  patients[patientIndex].service);
    printf("  Treatment:  %s\n",  patients[patientIndex].traitement);
    printf("  Status:     %s\n\n",patients[patientIndex].estDecede ? "Deceased" : "Alive");

    /* Display all scheduled appointments */
    printf("  Scheduled appointments: %d\n", patients[patientIndex].nbRendezVous);
    for (int j = 0; j < patients[patientIndex].nbRendezVous; j++) {
        printf("  %s at %s: %s\n",
               patients[patientIndex].rendezvous[j].date,
               patients[patientIndex].rendezvous[j].heure,
               patients[patientIndex].rendezvous[j].objet);
    }
}

/* Allows a patient to book a new appointment for themselves using their ID. */
void prendreRendezVous() {
    char id[20];
    printf("Enter your Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        RendezVous rdv;
        printf("Appointment date (dd/mm/yyyy): "); scanf("%19s", rdv.date);
        printf("Appointment time: ");              scanf("%9s",  rdv.heure);
        printf("Purpose of appointment: ");        scanf(" %99[^\n]", rdv.objet);

        patients[i].rendezvous[patients[i].nbRendezVous++] = rdv;
        sauvegarderPatientsVersTXT();
        printf("Appointment booked.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* Displays all upcoming appointments for a patient identified by their ID. */
void voirRendezVous() {
    char id[20];
    printf("Enter your Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("\n=== Upcoming Appointments for %s %s ===\n", patients[i].nom, patients[i].prenom);
        for (int j = 0; j < patients[i].nbRendezVous; j++) {
            printf("Date: %s - Time: %s - Purpose: %s\n",
                   patients[i].rendezvous[j].date,
                   patients[i].rendezvous[j].heure,
                   patients[i].rendezvous[j].objet);
            sauvegarderPatientsVersTXT();
        }
    } else {
        printf("Patient not found.\n");
    }
}

/* Allows a patient to cancel or reschedule an existing appointment.
 * Lists all current appointments, then prompts the patient to choose one
 * and select whether to cancel or modify it. */
void annulerModifierRendezVous() {
    char id[20];
    printf("Enter your Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        /* Display all appointments with their index numbers */
        printf("\n=== Appointments for %s %s ===\n", patients[i].nom, patients[i].prenom);
        for (int j = 0; j < patients[i].nbRendezVous; j++) {
            printf("Appointment %d - Date: %s - Time: %s - Purpose: %s\n",
                   j+1,
                   patients[i].rendezvous[j].date,
                   patients[i].rendezvous[j].heure,
                   patients[i].rendezvous[j].objet);
        }

        int choix;
        printf("Select appointment to cancel or modify (1 to %d): ", patients[i].nbRendezVous);
        scanf("%d", &choix);

        if (choix >= 1 && choix <= patients[i].nbRendezVous) {
            printf("1. Cancel appointment\n2. Modify appointment\nChoice: ");
            int action;
            scanf("%d", &action);

            if (action == 1) {
                /* Cancel: shift remaining appointments left to close the gap */
                for (int j = choix - 1; j < patients[i].nbRendezVous - 1; j++) {
                    patients[i].rendezvous[j] = patients[i].rendezvous[j + 1];
                }
                patients[i].nbRendezVous--;
                sauvegarderPatientsVersTXT();
                printf("Appointment cancelled.\n");

            } else if (action == 2) {
                /* Modify: overwrite the selected appointment with new values */
                printf("New appointment date (dd/mm/yyyy): ");
                scanf("%19s", patients[i].rendezvous[choix - 1].date);
                printf("New appointment time: ");
                scanf("%9s", patients[i].rendezvous[choix - 1].heure);
                printf("New purpose: ");
                scanf(" %99[^\n]", patients[i].rendezvous[choix - 1].objet);
                sauvegarderPatientsVersTXT();
                printf("Appointment updated.\n");
            }
        } else {
            printf("Appointment not found.\n");
        }
    } else {
        printf("Patient not found.\n");
    }
}

/* Allows a patient to update their personal contact information (address and phone). */
void mettreAJourInfos() {
    char id[20];
    printf("Enter your Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("Update personal information for %s %s:\n", patients[i].nom, patients[i].prenom);
        printf("New address: ");
        scanf(" %99[^\n]", patients[i].adresse);
        printf("New phone number: ");
        scanf("%19s", patients[i].telephone);
        sauvegarderPatientsVersTXT();
        printf("Information updated.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* Displays the currently prescribed medication/treatment for a patient.
 * Useful for patients to track their ongoing prescription. */
void suivreMeds() {
    char id[20];
    printf("Enter your Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        printf("\n==== Medication Tracker for %s %s ====\n", patients[i].nom, patients[i].prenom);
        printf("Prescribed treatment: %s\n", patients[i].traitement);
        sauvegarderPatientsVersTXT();
    } else {
        printf("Patient not found.\n");
    }
}

/* Allows a patient to submit a question or concern to their doctor.
 * The question is captured from input. (Future: could be stored separately.) */
void poserQuestions() {
    char id[20];
    printf("Enter your Patient ID: ");
    scanf("%19s", id);

    int i = -1;
    for (int j = 0; j < nbPatients; j++) {
        if (strcmp(patients[j].id, id) == 0) { i = j; break; }
    }

    if (i != -1) {
        char question[200];
        printf("\nSubmit a question or concern for %s %s:\n", patients[i].nom, patients[i].prenom);
        printf("Your question: ");
        scanf(" %199[^\n]", question);
        sauvegarderPatientsVersTXT();
        printf("Your question has been sent to the doctor.\n");
    } else {
        printf("Patient not found.\n");
    }
}

/* =========================================================================
 * PATIENT MENU
 * ========================================================================= */

/* Displays the Patient self-service menu.
 * No password required — patients access their data using their unique ID. */
void menuPatient() {
    int choix;
    do {
        clear();
        printf("=== PATIENT MENU ===\n");
        printf("1. View my medical record\n");
        printf("2. Book an appointment\n");
        printf("3. View upcoming appointments\n");
        printf("4. Cancel or modify an appointment\n");
        printf("5. Update personal information\n");
        printf("6. View my medication / prescription\n");
        printf("7. Submit a question or concern\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: consulterDossier();           break;
            case 2: prendreRendezVous();          break;
            case 3: voirRendezVous();             break;
            case 4: annulerModifierRendezVous();  break;
            case 5: mettreAJourInfos();           break;
            case 6: suivreMeds();                 break;
            case 7: poserQuestions();             break;
        }
        if (choix != 0) pause();
    } while (choix != 0);
}

/* =========================================================================
 * STATISTICS MODULE
 * ========================================================================= */

/* Calculates a patient's approximate age from their date of birth string.
 * Expected format: "dd/mm/yyyy". Returns the age as an integer. */
int calculerAge(const char* date_naissance) {
    int jour, mois, annee;
    sscanf(date_naissance, "%d/%d/%d", &jour, &mois, &annee);
    return 2025 - annee; /* Approximate age based on current year (2025) */
}

/* Calculates and prints the average age of all registered patients. */
void afficherAgeMoyen() {
    if (nbPatients == 0) {
        printf("No patients registered.\n");
        return;
    }

    int sommeAges = 0;
    for (int i = 0; i < nbPatients; i++) {
        sommeAges += calculerAge(patients[i].date_naissance);
    }
    printf("Average patient age: %.1f years\n", (float)sommeAges / nbPatients);
}

/* Calculates and prints the average number of appointments per patient. */
void afficherRendezVousMoyen() {
    if (nbPatients == 0) {
        printf("No patients registered.\n");
        return;
    }

    int totalRdv = 0;
    for (int i = 0; i < nbPatients; i++) {
        totalRdv += patients[i].nbRendezVous;
    }
    printf("Average appointments per patient: %.1f\n", (float)totalRdv / nbPatients);
}

/* Displays a general statistics report including total patients,
 * average age, average appointments, and gender distribution. */
void statistiquesPatients() {
    clear();
    printf("\n=== GENERAL STATISTICS ===\n\n");

    if (nbPatients == 0) {
        printf("No patients registered.\n");
        pause();
        return;
    }

    /* Basic statistics */
    printf("Total patients: %d\n", nbPatients);
    afficherAgeMoyen();
    afficherRendezVousMoyen();

    /* Gender breakdown */
    int hommes = 0, femmes = 0;
    for (int i = 0; i < nbPatients; i++) {
        if (strcasecmp(patients[i].genre, "M") == 0) {
            hommes++;
        } else {
            femmes++;
        }
    }
    printf("\nGender distribution:\n");
    printf("  Male:   %d (%.1f%%)\n", hommes, (float)hommes / nbPatients * 100);
    printf("  Female: %d (%.1f%%)\n", femmes, (float)femmes / nbPatients * 100);

    pause();
}

/* Displays the Statistics menu after password verification.
 * Provides access to general stats, average age, and appointment averages. */
void menuStatistiques() {
    char mdp[20];
    printf("Password: ");
    scanf("%19s", mdp);

    /* Verify statistics password before granting access */
    if (strcmp(mdp, PASSWORD_STA) != 0) {
        printf("Incorrect password.\n");
        return;
    }

    int choix;
    do {
        clear();
        printf("=== STATISTICS MENU ===\n");
        printf("1. General statistics\n");
        printf("2. Average patient age\n");
        printf("3. Average number of appointments\n");
        printf("0. Back\n");
        printf("Choice: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                statistiquesPatients();
                break;
            case 2:
                clear();
                printf("=== AVERAGE AGE ===\n");
                afficherAgeMoyen();
                pause();
                break;
            case 3:
                clear();
                printf("=== AVERAGE APPOINTMENTS ===\n");
                afficherRendezVousMoyen();
                pause();
                break;
        }
    } while (choix != 0);
}

/* =========================================================================
 * MAIN ENTRY POINT
 * ========================================================================= */

/* Program entry point.
 * Loads all patient data from file, displays the main menu,
 * and routes the user to the appropriate role-based submenu.
 * Before exiting, performs a final save to ensure no data is lost. */
int main() {
    /* Restore all patient records from the last session */
    chargerPatientsDepuisTXT();

    int choix;
    do {
        clear();
        printf("             === Main Menu ===\n");
        printf("\n====================================================\n");
        printf("    Welcome to the Medical Management System\n");
        printf("====================================================\n");
        printf("-> Secure | Simple to use | Optimised\n");
        printf("This program was developed to help healthcare\n");
        printf("professionals manage patient information efficiently.\n");
        printf("Data is automatically saved to a text file\n");
        printf("in order to ensure the backup and recovery of records.\n\n");
        printf("Developed by : AbdiR-pixel\n");
        printf("               Zya\n");
        printf("Date         : April 2025\n");
        printf("====================================================\n");
        printf("1. Administration\n");
        printf("2. Doctor\n");
        printf("3. Patient\n");
        printf("4. Statistics\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: menuAdministration(); break;
            case 2: menuMedecin();        break;
            case 3: menuPatient();        break;
            case 4: menuStatistiques();   break;
        }
    } while (choix != 0);

    /* Final save before quitting to ensure all changes are persisted */
    sauvegarderPatientsVersTXT();

    return 0;
}
