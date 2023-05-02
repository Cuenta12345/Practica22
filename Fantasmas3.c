
/*
Proceso de los fantasmas
*/

#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>		/* per exit() */
#include <unistd.h>		/* per getpid() */
#include "winsuport.h"		/* incloure definicions de funcions propies */
#include <stdbool.h>
#include <pthread.h>
#include <stdint.h>		/* intptr_t per màquines de 64 bits */

/*
Estructura
*/

				/* definir estructures d'informacio */
typedef struct {		/* per un objecte (menjacocos o fantasma) */
	int f;				/* posicio actual: fila */
	int c;				/* posicio actual: columna */
	int d;				/* direccio actual: [0..3] */
    float r;            /* per indicar un retard relati */
	char a;				/* caracter anterior en pos. actual */
} objecte;

int df[] = {-1, 0, 1, 0};	/* moviments de les 4 direccions possibles */
int dc[] = {0, -1, 0, 1};	/* dalt, esquerra, baix, dreta */

int retard;		    /* valor del retard de moviment, en mil.lisegons */

/* funcio per moure un fantasma una posicio; retorna 1 si el fantasma   */
/* captura al menjacocos, 0 altrament					*/
int main(void *index)
{
  objecte seg;
  int k, vk, nd, vd[3];
  int indice = (intptr_t) index;
  
  fflush(stdout);
  while (condicion == -1)
  {
    nd = 0;
    for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
    {
      pthread_mutex_lock(&mutex);
      vk = (elementos[indice].d + k) % 4;		/* direccio veina */
      if (vk < 0) vk += 4;		/* corregeix negatius */
      seg.f = elementos[indice].f + df[vk]; /* calcular posicio en la nova dir.*/
      seg.c = elementos[indice].c + dc[vk];
      seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
      pthread_mutex_unlock(&mutex);	/* obre semafor */  
      if ((seg.a==' ') || (seg.a=='.') || (seg.a=='0'))
      { 
        vd[nd] = vk;			/* memoritza com a direccio possible */
        nd++;
      }
    }
    if (nd == 0)
    {
      pthread_mutex_lock(&mutex);	/* tanca semafor */
  	  elementos[indice].d = (elementos[indice].d + 2) % 4;		/* canvia totalment de sentit */
      pthread_mutex_unlock(&mutex);	/* obre semafor */  
    }
    else
    { 
      pthread_mutex_lock(&mutex);	/* tanca semafor */
      if (nd == 1)			/* si nomes pot en una direccio */
      {
  	    elementos[indice].d = vd[0];			/* li assigna aquesta */
      }
      else				/* altrament */
    	  elementos[indice].d = vd[rand() % nd];		/* segueix una dir. aleatoria */
      pthread_mutex_unlock(&mutex);	/* obre semafor */  
      seg.f = elementos[indice].f + df[elementos[indice].d];  /* calcular seguent posicio final */
      seg.c = elementos[indice].c + dc[elementos[indice].d];
      pthread_mutex_lock(&mutex);
      seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
      win_escricar(elementos[indice].f,elementos[indice].c,elementos[indice].a,NO_INV);	/* esborra posicio anterior */
      elementos[indice].f = seg.f; elementos[indice].c = seg.c; elementos[indice].a = seg.a;	/* actualitza posicio */
      win_escricar(elementos[indice].f,elementos[indice].c, (char)('0'+indice),NO_INV);		/* redibuixa fantasma */
      if (elementos[indice].a == '0') condicion = 1;		/* ha capturat menjacocos */
      pthread_mutex_unlock(&mutex);	/* obre semafor */ 
    }
    win_retard(retard);
  //printf("Valor de a -->%c\n", elementos[indice].a);
  }
  return ((void *) NULL);
}
