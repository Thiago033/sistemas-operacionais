#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_NEBULIZERS 4
#define MAX_PATIENTS_WAITING 12
#define TOTAL_PATIENTS 16

#define NUM_DOCTORS 2

#define SIMULATION_TIME 60

pthread_mutex_t patientsAgeMutex;
pthread_mutex_t patientsMutex;
pthread_mutex_t nebulizersMutex;
pthread_mutex_t waitingRoomPatientsMutex;
pthread_mutex_t zeroVitalSignPatientsMutex;

sem_t nebulizersSem;

pthread_t reduceVitalSignThread;

pthread_t patientsThread[TOTAL_PATIENTS];
pthread_t doctorsThread[NUM_DOCTORS];

int *threadID = NULL;
int patientsAge[TOTAL_PATIENTS];
int patientsVitalSign[TOTAL_PATIENTS];
int nebulizers[NUM_NEBULIZERS];
int doctors[NUM_DOCTORS];

int waitingRoomPatients = 0;
int zeroVitalSignPatients = 0;
int endSimulation = 0;

/*
==============================
TryGetNebulizer
Description:
    This function attempts to find a nebulizer associated with a specific patient.
    It iterates through the nebulizers array to check if the patientIndex matches any nebulizer.
Parameters:
    - patientIndex: The index of the patient to search for in the nebulizers array.
Returns:
    1 if a nebulizer is found for the specified patientIndex, 0 otherwise.
==============================
*/
int TryGetNebulizer( int patientIndex ) {

    pthread_mutex_lock( &nebulizersMutex );
    // Iterate through the nebulizers to check if any nebulizer is assigned to the patient.
    for ( int i = 0; i < NUM_NEBULIZERS; i++ ) {
        if ( nebulizers[i] == patientIndex ) {
            pthread_mutex_unlock( &nebulizersMutex );
            return 1;
        }
    }
    pthread_mutex_unlock( &nebulizersMutex );

    return 0;
}

/*
==============================
NebulizerScheduling
Description:
    This function is responsible for scheduling a nebulizer for a patient based on their vital signs.
    It searches for the patient with the lowest vital signs (greater than or equal to 0) who has not been assigned a nebulizer.
Returns:
    The index of the patient to whom a nebulizer should be scheduled, or -1 if no eligible patient is found.
==============================
*/
int NebulizerScheduling() {
    int patientIndex;
    int lowestVitalSign = INT_MAX;

    pthread_mutex_lock( &patientsMutex );
    // Iterate through the list of patients to find the one with the lowest vital sign who is not currently using a nebulizer.
    for ( int i = 0; i < TOTAL_PATIENTS; i++ ) {
        if ( 
            patientsVitalSign[i] >= 0 && 
            patientsVitalSign[i] < lowestVitalSign &&
            TryGetNebulizer( i ) == 0 
        ) { 
            // Update the patientIndex and lowestVitalSign when a suitable patient is found.
            patientIndex = i;
            lowestVitalSign = patientsVitalSign[i];
        }
    }
    pthread_mutex_unlock( &patientsMutex );

    // Return the index of the patient to schedule for nebulizer treatment.
    // If no suitable patient is found, this will indicate that no scheduling is required.
    return patientIndex;
}

/*
==============================
ReduceVitalSign
Description:
    This function is responsible for reducing the vital signs of patients who have not been assigned a nebulizer.
    It iterates through all patients, checks if they have a positive vital sign, and if they have not been assigned a nebulizer.
    If both conditions are met, it reduces the patient's vital sign by a random value between 1 and 2 (inclusive).
==============================
*/
void ReduceVitalSign() {
    for ( int i = 0; i < TOTAL_PATIENTS; i++ ) {

        pthread_mutex_lock( &patientsMutex );
        if ( patientsVitalSign[i] >= 0 && TryGetNebulizer(i) == 0 ) {
            // Calculate a new vital sign by reducing the current one by a random value between 1 and 2.
            int newVitalSign = patientsVitalSign[i] - ( ( rand() % 1 ) + 1 );

            printf("Patient %d vital sign REDUCED to: %d\n", i, newVitalSign); 

            if ( newVitalSign > 0 ) {
                patientsVitalSign[i] = newVitalSign;
            } else {
                // If the vital sign reaches 0, update patient data and count zero vital sign patients.
                patientsVitalSign[i] = 0;
                printf("patient %d reached vital sign of 0\n", i);

                pthread_mutex_lock( &zeroVitalSignPatientsMutex );
                zeroVitalSignPatients++;
                pthread_mutex_unlock( &zeroVitalSignPatientsMutex );
            }
        }
        pthread_mutex_unlock( &patientsMutex );
    }
}

/*
==============================
ThreadReduceVitalSign
Description:
    This is a thread function responsible for periodically reducing the vital signs of patients who have not been assigned a nebulizer.
    It sleeps for 5 seconds initially to allow other threads to start, and then enters a loop to periodically call the 'ReduceVitalSign' function every 3 seconds.
    It continues to do so until the 'endSimulation' flag is set to true, at which point it exits the thread.
==============================
*/
void *ThreadReduceVitalSign( void *arg ) {
    // Sleep for an initial 5 seconds to allow the simulation to start.
    sleep( 5 );

    // Continuously reduce vital signs until the end of the simulation is signaled.
    while ( !endSimulation ) {
        // Call the ReduceVitalSign function to reduce vital signs for eligible patients.
        ReduceVitalSign();

        // Sleep for 3 seconds before the next reduction cycle.
        sleep( 3 );
    }

    // Exit the thread once the simulation ends.
    pthread_exit(NULL);
}

/*
==============================
IncreaseVitalSign
Description:
    This function simulates a patient using a nebulizer to increase their vital signs.
    It attempts to find an available nebulizer by iterating through the nebulizers array.
    If an available nebulizer is found, it assigns it to the patient and increases their vital signs.
Parameters:
    patientIndex: The index of the patient using the nebulizer.
    idPaciente: The identifier of the patient.
==============================
*/
void IncreaseVitalSign( int patientIndex, int idPaciente ) {
    int nebulizerIndex;

    // Find an available nebulizer for the patient.
    for ( int i = 0; i < NUM_NEBULIZERS; i++ ) {

        pthread_mutex_lock( &nebulizersMutex );
        if ( nebulizers[i] == -1 ) {
            // Assign the nebulizer to the patient.
            nebulizers[i] = patientIndex;
            nebulizerIndex = i;
            pthread_mutex_unlock( &nebulizersMutex );

            break;
        } else {
            pthread_mutex_unlock( &nebulizersMutex );
        }
    }
    
    // Increase the patient's vital sign.
    pthread_mutex_lock( &patientsMutex );
    if ( patientsVitalSign[patientIndex] >= 0 ) {
        // Increase vital sign by a random value between 2 and 3, capped at a maximum of 10.
        patientsVitalSign[patientIndex] += ( ( rand() % 2 ) + 2 );
        patientsVitalSign[patientIndex] = ( patientsVitalSign[patientIndex] > 10 ) ? 10 : patientsVitalSign[patientIndex];
    }
    pthread_mutex_unlock( &patientsMutex );

    printf("Patient %d is USING the nebulizer %d. Vital signs INCREASED to %d.\n", idPaciente, nebulizerIndex, patientsVitalSign[patientIndex]);

    // Simulate nebulizer use for a random time between 1 to 2 seconds.
    sleep( ( rand() % 1 ) + 1 );

    printf("Patient %d ENDED using the nebulizer %d.\n", idPaciente, nebulizerIndex);
    printf("Nebulizer %d is now available.\n", nebulizerIndex);

    // Release the nebulizer for other patients to use.
    pthread_mutex_lock( &nebulizersMutex );
    nebulizers[nebulizerIndex] = -1;
    pthread_mutex_unlock( &nebulizersMutex );
}

/*
==============================
PatientsWaitingRoom
Description:
    This is a thread function representing patients waiting in the waiting room.
    It assigns patients a vital sign, adds them to a priority queue based on age, and simulates their wait for nebulizer treatment.
Parameters:
    arg: A pointer to the patient's ID (patientID).
==============================
*/
void *PatientsWaitingRoom( void *arg ) {
    int patientID = *(int *)arg;

    int patientIndex;

    pthread_mutex_lock( &patientsMutex );
    // Find an available slot for the patient in the patient data array.
    for ( int i = 0; i < TOTAL_PATIENTS; i++ ) {
        if ( patientsVitalSign[i] == -1 ) {
            pthread_mutex_unlock( &patientsMutex );
            // Mark the index as occupied and store the patient's index.
            patientIndex = i;
            break;
        } else {
            pthread_mutex_unlock( &patientsMutex );
        }
    }

    // Set the initial vital sign for the patient.
    pthread_mutex_lock( &patientsMutex );
    // Define um valor inicial aleatório para o sinal vital do paciente.
    // Gera um número aleatório entre 0 e 4 com rand(), adiciona 5 para garantir que o valor inicial esteja entre 5 e 9.
    patientsVitalSign[patientIndex] = ( ( rand() % 5 ) + 5 );
    pthread_mutex_unlock( &patientsMutex );

    // Generate a random age for the patient (between 10 and 80) and add them to the priority queue based on age.
    int age = ( ( rand() % 70 ) + 10 );

    // Add the patient to the priority queue based on age
    pthread_mutex_lock( &patientsAgeMutex );
    patientsAge[patientIndex] = age;
    pthread_mutex_unlock( &patientsAgeMutex );

    printf("Patient %d WAITING. Vital sign: %d. Age: %d\n", patientID, patientsVitalSign[patientIndex], age);

    if ( patientsVitalSign[patientIndex] == 0 ) {
        printf("Patient %d reached vital sign of 0\n", patientID);

        pthread_mutex_lock( &zeroVitalSignPatientsMutex );
        zeroVitalSignPatients++;
        pthread_mutex_unlock( &zeroVitalSignPatientsMutex );
    }

    pthread_mutex_lock( &patientsMutex );
    // Continue monitoring the patient's status until the end of the simulation or vital sign becomes -2.
    while ( patientsVitalSign[patientIndex] != -2 && endSimulation == 0 ) {

        pthread_mutex_unlock(&patientsMutex);

        // Check if it's the patient's turn for nebulizer treatment.
        if ( NebulizerScheduling() == patientIndex ) {
            int nebulizerSemCounter = -1;

            // Get the current semaphore value for nebulizers.
            sem_getvalue( &nebulizersSem, &nebulizerSemCounter );

            if ( nebulizerSemCounter > 0 ) {
                sem_wait( &nebulizersSem );
                // Simulate an increase in vital signs while using the nebulizer.
                IncreaseVitalSign( patientIndex, patientID );
                sem_post( &nebulizersSem );
            }
        }

        // Sleep for 3 seconds before the next check.
        sleep( 3 );

        pthread_mutex_lock( &patientsMutex );
    }

    // Mark the patient's status as -1 (no longer in the waiting room).
    patientsVitalSign[patientIndex] = -1;
    pthread_mutex_unlock( &patientsMutex );

    pthread_mutex_lock( &waitingRoomPatientsMutex );
    waitingRoomPatients--;
    pthread_mutex_unlock( &waitingRoomPatientsMutex );
    
    pthread_exit( NULL );
}

/*
==============================
DoctorTreatment
Description:
    This is a thread function representing a doctor treating patients.
    It waits for a brief period initially to allow other threads to start, and then enters a loop to treat patients based on their age.
    The doctor looks for the oldest patient in the priority queue (based on age) and treats them.
    After treatment, the patient's vital sign is set to -2 to indicate they have been treated.
Parameters:
    arg: A pointer to the thread's ID (id_thread).
==============================
*/
void *DoctorTreatment( void *arg ) {
    // Sleep for an initial 5 seconds to allow the simulation to start.
    sleep( 5 );

    int id_thread = *(int *)arg;

    while ( endSimulation == 0 ) {
        int patientIndex;
        int minAgeIndex = -1;
        int minAge = INT_MAX;

        pthread_mutex_lock( &patientsAgeMutex );
        // Find the index of the oldest patient in the priority queue.
        for ( int i = 0; i < TOTAL_PATIENTS; i++ ) {
            if ( patientsAge[i] >= 0 && patientsAge[i] < minAge ) {
                minAge = patientsAge[i];
                minAgeIndex = i;
            }
        }

        if ( minAgeIndex >= 0 ) {
            // Mark the patient as being treated by a doctor.
            patientsAge[minAgeIndex] = -1;
        }
        pthread_mutex_unlock( &patientsAgeMutex );

        // Set the patientIndex to the index of the patient to be treated.
        patientIndex = minAgeIndex;

        if ( patientIndex != -1 ) {

            pthread_mutex_lock( &patientsMutex );
            if ( patientsVitalSign[patientIndex] != -1 ) {
                pthread_mutex_unlock( &patientsMutex );

                printf("Doctor %d STARTED treating patient %d\n", id_thread, patientIndex);
                
                // Start treating the patient and simulate the treatment time (between 3 to 5 seconds).
                sleep( ( rand() % 2 ) + 3 );

                // Mark the patient as treated.
                patientsVitalSign[patientIndex] = -2;

                printf("Doctor %d FINISHED treating patient %d\n", id_thread, patientIndex);

                // The doctor waits for 3 seconds before checking again.
                sleep( 3 );
            }
            pthread_mutex_unlock( &patientsMutex );
        } else {
            // If no patient is available for treatment, the doctor waits for 3 seconds before checking again.
            sleep( 3 );
        }
    }

    pthread_exit( NULL );
}

int main() {
    srand( time( NULL ) );

    pthread_mutex_init( &patientsMutex, NULL );
    pthread_mutex_init( &patientsAgeMutex, NULL );
    pthread_mutex_init( &nebulizersMutex, NULL );
    pthread_mutex_init( &waitingRoomPatientsMutex, NULL );
    pthread_mutex_init( &zeroVitalSignPatientsMutex, NULL );

    sem_init(&nebulizersSem, 0, NUM_NEBULIZERS);

    for ( int i = 0; i < TOTAL_PATIENTS; i++ ) {
        patientsVitalSign[i] = -1;
    }

    for ( int i = 0; i < TOTAL_PATIENTS; i++ ) {
        patientsAge[i] = -1;
    }

    for ( int i = 0; i < NUM_NEBULIZERS; i++ ) {
        nebulizers[i] = -1;
    }

    for ( int i = 0; i < NUM_DOCTORS; i++ ) {
        doctors[i] = i;
        pthread_create( &doctorsThread[i], NULL, DoctorTreatment, &doctors[i] );
    }
    
    // Create a thread for reducing vital signs.
    pthread_create( &reduceVitalSignThread, NULL, ThreadReduceVitalSign, NULL );

    threadID = (int *) malloc( 1 * sizeof(int) );

    int totalPatients = 0;
    int patientsTreated = 0;

    // Simulate patient arrivals and treatment for a specified simulation time.
    for ( int i = 0; i < SIMULATION_TIME; i++ ) {
        totalPatients++;

        if ( waitingRoomPatients < TOTAL_PATIENTS ) { 
            // Dynamically allocate memory for thread IDs and create patient threads.
            threadID = (int *)realloc( threadID, totalPatients * sizeof(int) );
            threadID[i] = i;

            pthread_mutex_lock( &waitingRoomPatientsMutex );
            waitingRoomPatients++;
            pthread_mutex_unlock( &waitingRoomPatientsMutex );

            // Create a thread for a patient in the waiting room.
            // printf("THREAD ID %d CREATED", threadID[i]);
            pthread_create( &patientsThread[i], NULL, PatientsWaitingRoom, &threadID[i] );
            patientsTreated++;
        }
        else {
            printf("Waiting room is full \n");
        }

        // Simulate patient arrival time.
        sleep( ( rand() % 2 ) + 3 );
    }

    endSimulation = 1;
    printf("Simulation End!\n");

    pthread_join( reduceVitalSignThread, NULL ); 
    pthread_cancel( reduceVitalSignThread );

    for ( int i = 0; i < TOTAL_PATIENTS; i++ ) {
        pthread_join( patientsThread[i], NULL );
    }

    for ( int i = 0; i < TOTAL_PATIENTS; i++ ) {
        pthread_cancel( patientsThread[i] );
    }

    for ( int i = 0; i < NUM_DOCTORS; i++ ) {
        pthread_join( doctorsThread[i], NULL );
    }
    
    for ( int i = 0; i < NUM_DOCTORS; i++ ) {
        pthread_cancel( doctorsThread[i] );
    }

    printf("======================================\n");
    printf("Number of patients: %d\n", totalPatients);
    printf("Patients Treated: %d\n", patientsTreated);
    printf("Patients that reached vital sign of zero: %d\n", zeroVitalSignPatients);
    printf("======================================\n");

    free( threadID );

    pthread_mutex_destroy( &patientsAgeMutex );
    pthread_mutex_destroy( &patientsMutex );
    pthread_mutex_destroy( &nebulizersMutex );
    pthread_mutex_destroy( &waitingRoomPatientsMutex );
    pthread_mutex_destroy( &zeroVitalSignPatientsMutex );

    sem_destroy( &nebulizersSem );

    exit( 0 );
}
