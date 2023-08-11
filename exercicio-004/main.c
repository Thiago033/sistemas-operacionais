#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 5

int processes, resources;
int max[MAX_PROCESSES][MAX_RESOURCES];
int allocated[MAX_PROCESSES][MAX_RESOURCES];
int need[MAX_PROCESSES][MAX_RESOURCES];
int available[MAX_RESOURCES];
bool finished[MAX_PROCESSES];

void generate_data() {
    // Generate random data for max, allocated, and available
    // (You can replace this with your own generation logic)
    for (int i = 0; i < processes; i++) {
        for (int j = 0; j < resources; j++) {
            max[i][j] = rand() % 10;
            allocated[i][j] = rand() % (max[i][j] + 1);
            need[i][j] = max[i][j] - allocated[i][j];
        }
        finished[i] = false;
    }

    for (int j = 0; j < resources; j++) {
        available[j] = rand() % 10;
    }
}

bool is_safe(int process) {
    for (int j = 0; j < resources; j++) {
        if (need[process][j] > available[j]) {
            return false;
        }
    }
    return true;
}

int main() {
    printf("Enter the number of processes: ");
    scanf("%d", &processes);

    printf("Enter the number of resources: ");
    scanf("%d", &resources);

    generate_data();

    bool safe_sequence_found = false;

    for (int i = 0; i < processes; i++) {
        for (int j = 0; j < processes; j++) {

            if (!finished[j] && is_safe(j)) {

                finished[j] = true;
                safe_sequence_found = true;
                printf(" -> Process %d", j);

                for (int k = 0; k < resources; k++) {
                    available[k] += allocated[j][k];
                }
                
                break;
            }
        }
    }

    if (!safe_sequence_found) {
        printf("No safe sequence exists.\n");
    }

    return 0;
}
