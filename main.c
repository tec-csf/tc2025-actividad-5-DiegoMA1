//
//  Actividad 5
//  Diego Moreno Acevedo A01022113
//  Hecho con Saúl Montes de Oca A01025975
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int tuberia[2];
} tuberias_t;

int main(int argc, char * argv[]) {

    char *pvalue = NULL;
    int index;
    int p;
    while ((p = getopt (argc, argv, "n:")) != -1)
        switch (p)
        {
        case 'n':
            pvalue = optarg;
            break;
        case '?':
            if (optopt == 'n')
                fprintf (stderr, "Opción -%c requiere un argumento.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Opción desconocida '-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Opción desconocida '\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort ();
    }
    for (index = optind; index < argc; index++)
        printf ("El argumento no es una opción válida %s\n", argv[index]);

    //Convirtiendo a entero argumento recibido en linea de comando atoi()
    int nprocesos = atoi(pvalue);
    char test;

    if(nprocesos>=1){
 
        pid_t pid;
        tuberias_t * tubos = (tuberias_t *) malloc(sizeof(tuberias_t) * nprocesos);
        tuberias_t * current = tubos;
        //Creando tuberias
        for(; current < tubos+nprocesos; current++) {
            if(pipe(current->tuberia) == -1) {
                perror("Error al crear las tuberias\n");
                exit(-1);
            }
        }
        int i = 0;
        while (i < nprocesos-1){
            pid = fork();
            switch (pid) {
                case -1:
                    printf("Hubo un error al crear proceso hijo. Numero de procesos hijos creados hasta ahora: %d\n", i+1);
                    exit(-1);
                case 0:
                    while (1) {
                        //Cierre de tuberias
                        for(int j = 0; j < nprocesos-1; j++) {
                            if((tubos+j)->tuberia[0] != (tubos+i)->tuberia[0]) {
                                close((tubos+j)->tuberia[0]);
                            }
                        }
                        close((tubos+i)->tuberia[1]);
                        //Leyendo
                        read((tubos+i)->tuberia[0], &test, sizeof(char));
                        printf("--> Soy el proceso con PID %d y recibí el testigo %c, el cual tendré por 5 segundos\n", getpid(), test);
                        sleep(5);
                        close((tubos+i+1)->tuberia[0]);
                        //Escribiendo en siguiente
                        printf("<-- Soy el proceso con PID %d y acabo de enviar el testigo %c\n", getpid(), test);
                        write((tubos+i+1)->tuberia[1], &test, sizeof(char));
                    }
            } 
            i++;
        }
        //Solo se ejecuta una vez
        char envio = 'T';
        printf("<-- Soy el proceso padre con PID %d y acabo de enviar el testigo %c\n", getpid(), envio);
        write(tubos->tuberia[1], &envio, sizeof(char));
        
        while (1) {
            //printf("Leyendo en la ultima tuberia \n");
            close((tubos+1)->tuberia[1]);
            read((tubos+nprocesos-1)->tuberia[0], &test, sizeof(char));
            printf("--> Soy el proceso padre con PID %d y recibí el testigo %c, el cual tendré por 5 segundos\n", getpid(), test);
            sleep(5);
            //printf("Escribiendo en la ultima tuberia \n");
            close(tubos->tuberia[0]);
            printf("\n<-- Soy el proceso padre con PID %d y acabo de enviar el testigo %c\n", getpid(), test);
            write(tubos->tuberia[1], &test, sizeof(char));
        }
    }
    else{
        printf("Numero invalido para la creación de procesos");
        exit(-1);
    }
    return 0; 
}

