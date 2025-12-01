#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TAMTOKEN 50
#define MAX_PALABRAS 20000
#define MAX_SUGERENCIAS 5000

void Diccionario(char* szNombre, char szPalabras[][TAMTOKEN], int iEstadisticas[], int& iNumElementos);
void ClonaPalabras(char* szPalabraLeida, char szPalabrasSugeridas[][TAMTOKEN], int& iNumSugeridas);
void ListaCandidatas(char szPalabrasSugeridas[][TAMTOKEN], int iNumSugeridas, char szPalabras[][TAMTOKEN], int iEstadisticas[], int iNumElementos, char szListaFinal[][TAMTOKEN], int iPesos[], int& iNumLista);

void ordenarDiccionario(char szPalabras[][TAMTOKEN], int iEstadisticas[], int n);
void ordenarPorPeso(char szListaFinal[][TAMTOKEN], int iPesos[], int n);
int busquedaBinaria(char* palabra, char szPalabras[][TAMTOKEN], int n);
void agregarSugerencia(char* candidata, char szPalabrasSugeridas[][TAMTOKEN], int& iNumSugeridas);
void strToLower(char* str);

int main() {
    char diccionario[MAX_PALABRAS][TAMTOKEN];
    int estadisticas[MAX_PALABRAS];
    int numElementos = 0;

    memset(diccionario, 0, sizeof(diccionario));

    FILE* f = fopen("diccionario.txt", "w");
    if (f) {
        fprintf(f, "hola mundo programacion computadora proyecto final cyp unam aragon correccion palabra texto");
        fclose(f);
    }

    char nombreArchivo[] = "diccionario.txt";
    Diccionario(nombreArchivo, diccionario, estadisticas, numElementos);

    printf("Diccionario cargado con %d palabras.\n", numElementos);

    char palabraErronea[TAMTOKEN];
    printf("Ingrese una palabra a corregir: ");
    scanf_s("%s", palabraErronea, (unsigned int)TAMTOKEN);
    strToLower(palabraErronea);

    char sugerencias[MAX_SUGERENCIAS][TAMTOKEN];
    int numSugeridas = 0;
    ClonaPalabras(palabraErronea, sugerencias, numSugeridas);

    char listaFinal[MAX_SUGERENCIAS][TAMTOKEN];
    int pesos[MAX_SUGERENCIAS];
    int numLista = 0;

    ListaCandidatas(sugerencias, numSugeridas, diccionario, estadisticas, numElementos, listaFinal, pesos, numLista);

    if (numLista == 0) {
        printf("No se encontraron sugerencias.\n");
    }
    else {
        printf("Sugerencias encontradas: %d\n", numLista);
        for (int i = 0; i < numLista; i++) {
            printf("%d. %s (Frecuencia: %d)\n", i + 1, listaFinal[i], pesos[i]);
        }
    }

    printf("\nPresione ENTER para salir...");
    getchar(); getchar();
    return 0;
}

void Diccionario(char* szNombre, char szPalabras[][TAMTOKEN], int iEstadisticas[], int& iNumElementos) {
    FILE* archivo = fopen(szNombre, "r");
    iNumElementos = 0;

    if (archivo == NULL) {
        return;
    }

    char buffer[TAMTOKEN];
    char palabraLimpia[TAMTOKEN];

    while (fscanf(archivo, "%s", buffer) != EOF && iNumElementos < MAX_PALABRAS) {
        int k = 0;
        for (int i = 0; buffer[i]; i++) {
            if (isalpha((unsigned char)buffer[i])) {
                palabraLimpia[k++] = tolower((unsigned char)buffer[i]);
            }
        }
        palabraLimpia[k] = '\0';

        if (k > 0) {
            int encontrado = -1;
            for (int i = 0; i < iNumElementos; i++) {
                if (strcmp(szPalabras[i], palabraLimpia) == 0) {
                    encontrado = i;
                    break;
                }
            }

            if (encontrado != -1) {
                iEstadisticas[encontrado]++;
            }
            else {
                strcpy(szPalabras[iNumElementos], palabraLimpia);
                iEstadisticas[iNumElementos] = 1;
                iNumElementos++;
            }
        }
    }
    fclose(archivo);
    ordenarDiccionario(szPalabras, iEstadisticas, iNumElementos);
}

void ClonaPalabras(char* szPalabraLeida, char szPalabrasSugeridas[][TAMTOKEN], int& iNumSugeridas) {
    iNumSugeridas = 0;
    char temp[TAMTOKEN];
    int len = (int)strlen(szPalabraLeida);
    char alfabeto[] = "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; i++) {
        int k = 0;
        for (int j = 0; j < len; j++) {
            if (i != j) temp[k++] = szPalabraLeida[j];
        }
        temp[k] = '\0';
        agregarSugerencia(temp, szPalabrasSugeridas, iNumSugeridas);
    }

    for (int i = 0; i < len - 1; i++) {
        strcpy(temp, szPalabraLeida);
        char aux = temp[i];
        temp[i] = temp[i + 1];
        temp[i + 1] = aux;
        agregarSugerencia(temp, szPalabrasSugeridas, iNumSugeridas);
    }

    for (int i = 0; i < len; i++) {
        strcpy(temp, szPalabraLeida);
        for (int j = 0; j < 26; j++) {
            temp[i] = alfabeto[j];
            if (strcmp(temp, szPalabraLeida) != 0) {
                agregarSugerencia(temp, szPalabrasSugeridas, iNumSugeridas);
            }
        }
    }

    for (int i = 0; i <= len; i++) {
        for (int j = 0; j < 26; j++) {
            int k = 0;
            for (int m = 0; m < i; m++) temp[k++] = szPalabraLeida[m];
            temp[k++] = alfabeto[j];
            for (int m = i; m < len; m++) temp[k++] = szPalabraLeida[m];
            temp[k] = '\0';
            agregarSugerencia(temp, szPalabrasSugeridas, iNumSugeridas);
        }
    }
}

void ListaCandidatas(char szPalabrasSugeridas[][TAMTOKEN], int iNumSugeridas, char szPalabras[][TAMTOKEN], int iEstadisticas[], int iNumElementos, char szListaFinal[][TAMTOKEN], int iPesos[], int& iNumLista) {
    iNumLista = 0;
    for (int i = 0; i < iNumSugeridas; i++) {
        int indice = busquedaBinaria(szPalabrasSugeridas[i], szPalabras, iNumElementos);
        if (indice != -1) {
            int yaExiste = 0;
            for (int k = 0; k < iNumLista; k++) {
                if (strcmp(szListaFinal[k], szPalabrasSugeridas[i]) == 0) {
                    yaExiste = 1;
                    break;
                }
            }
            if (!yaExiste) {
                strcpy(szListaFinal[iNumLista], szPalabras[indice]);
                iPesos[iNumLista] = iEstadisticas[indice];
                iNumLista++;
            }
        }
    }
    ordenarPorPeso(szListaFinal, iPesos, iNumLista);
}

void strToLower(char* str) {
    for (int i = 0; str[i]; i++) str[i] = tolower((unsigned char)str[i]);
}

void ordenarDiccionario(char szPalabras[][TAMTOKEN], int iEstadisticas[], int n) {
    char tempPalabra[TAMTOKEN];
    int tempStat;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (strcmp(szPalabras[j], szPalabras[j + 1]) > 0) {
                strcpy(tempPalabra, szPalabras[j]);
                strcpy(szPalabras[j], szPalabras[j + 1]);
                strcpy(szPalabras[j + 1], tempPalabra);
                tempStat = iEstadisticas[j];
                iEstadisticas[j] = iEstadisticas[j + 1];
                iEstadisticas[j + 1] = tempStat;
            }
        }
    }
}

void ordenarPorPeso(char szListaFinal[][TAMTOKEN], int iPesos[], int n) {
    char tempPalabra[TAMTOKEN];
    int tempPeso;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (iPesos[j] < iPesos[j + 1]) {
                strcpy(tempPalabra, szListaFinal[j]);
                strcpy(szListaFinal[j], szListaFinal[j + 1]);
                strcpy(szListaFinal[j + 1], tempPalabra);
                tempPeso = iPesos[j];
                iPesos[j] = iPesos[j + 1];
                iPesos[j + 1] = tempPeso;
            }
        }
    }
}

int busquedaBinaria(char* palabra, char szPalabras[][TAMTOKEN], int n) {
    int izq = 0, der = n - 1;
    while (izq <= der) {
        int medio = izq + (der - izq) / 2;
        int res = strcmp(palabra, szPalabras[medio]);
        if (res == 0) return medio;
        if (res > 0) izq = medio + 1;
        else der = medio - 1;
    }
    return -1;
}

void agregarSugerencia(char* candidata, char szPalabrasSugeridas[][TAMTOKEN], int& iNumSugeridas) {
    if (iNumSugeridas >= MAX_SUGERENCIAS) return;
    for (int i = 0; i < iNumSugeridas; i++) {
        if (strcmp(szPalabrasSugeridas[i], candidata) == 0) return;
    }
    strcpy(szPalabrasSugeridas[iNumSugeridas], candidata);
    iNumSugeridas++;
}