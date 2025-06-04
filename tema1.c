#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nod
{
    char c;
    struct nod *urmator;
    struct nod *precedent;
} nod;

typedef struct banda
{ // structura pentru banda cu deget
    struct nod *santinela;
    struct nod *deget;
} banda;

typedef struct element
{ // struct pt elem din coada/stiva
    char operatie[20];
    char c;
    struct element *urmator;
} element;

typedef struct coada
{ // structura pt coada
    struct element *primul;
    struct element *ultimul;
} coada;

typedef struct element_stiva
{ // struct pt elementul stivei
    struct nod *pozitie;
    struct element_stiva *urmator;
} element_stiva;

typedef struct Stiva
{ // struct pentru stiva
    element_stiva *varf;
    int dimensiune;
} Stiva;

void adauga_stiva(Stiva *S, nod *pozitie)
{
    element_stiva *el_undo = (element_stiva *)malloc(sizeof(element_stiva));
    el_undo->pozitie = pozitie;
    el_undo->urmator = NULL;

    if (S->varf != NULL)
        el_undo->urmator = S->varf;
    S->varf = el_undo;
    S->dimensiune += 1;
}

void sterge_din_stiva(Stiva *S)
{
    element_stiva *cr = S->varf;
    if (S->dimensiune > 1)
        S->varf = cr->urmator;
    else
        S->varf = NULL;
    free(cr);
    S->dimensiune -= 1;
}

void goleste_stiva(Stiva *S)
{
    while (S->dimensiune > 0)
    {
        sterge_din_stiva(S);
    }
}

void move_left(banda *B, coada *C, Stiva *U, FILE *r, FILE *w)
{
    if (B->deget->precedent != B->santinela)
    {
        adauga_stiva(U, B->deget);
        B->deget = B->deget->precedent;
    } // verific si mut la stanga
}

void move_right(banda *B, coada *C, Stiva *U, FILE *r, FILE *w)
{

    if (B->deget->urmator == NULL)
    {
        nod *nou = (nod *)malloc(sizeof(nod));
        nou->c = '#';
        nou->urmator = NULL;
        // leg pe nou in continuarea lui deget
        B->deget->urmator = nou;
        nou->precedent = B->deget;
    }

    adauga_stiva(U, B->deget);
    B->deget = B->deget->urmator; // mut degetul la dreapta
}

void move_right_char(char c, banda *B, FILE *r, FILE *w)
{
    nod *p = B->deget;
    nod *copie;
    while (p != NULL)
    {
        if (p->c == c)
        {
            B->deget = p;
            return;
        }
        copie = p; // e nevoie de copie ca sa ne putem intoarce cand dam de NULL
        p = p->urmator;
    }

    nod *q = (nod *)malloc(sizeof(nod));
    q->c = '#';
    q->precedent = copie;
    q->urmator = NULL;
    copie->urmator = q;

    B->deget = q;
}

void move_left_char(banda *B, FILE *r, FILE *w, char c)
{
    nod *p = B->deget;
    while (p != B->santinela)
    {
        if (p->c == c)
        {

            B->deget = p;
            return;
        }

        p = p->precedent;
    }
    // daca ajungem la santinela *deget nu se modifica
    fprintf(w, "ERROR\n");
}

void write_char(banda *B, Stiva *U, Stiva *R, FILE *r, FILE *w, char c)
{
    B->deget->c = c;
    goleste_stiva(U);
    goleste_stiva(R);
}

void insert_left(banda *B, FILE *r, FILE *w, char c)
{
    if (B->deget->precedent == B->santinela)
    {
        // printf("ERROR\n");
        fprintf(w, "ERROR\n");
        return;
    }
    nod *q = (nod *)malloc(sizeof(nod));
    q->c = c;
    q->precedent = B->deget->precedent;
    q->urmator = B->deget;
    B->deget->precedent->urmator = q;
    B->deget->precedent = q;

    B->deget = q; // degetul arata la final catre nodul inserat
    // fprintf(w, "insert_left %c\n", c);
}

void insert_right(banda *B, FILE *r, FILE *w, char c)
{
    nod *q = (nod *)malloc(sizeof(nod));
    q->c = c;
    q->precedent = B->deget;
    q->urmator = B->deget->urmator;
    if (B->deget->urmator != NULL) // daca e null nu am pe cine lega
        B->deget->urmator->precedent = q;
    B->deget->urmator = q;

    B->deget = q; // degetul arata la final catre nodul inserat

    // fprintf(w, "INSERT_RIGHT %c\n", c);
}

void show_current(banda *B, FILE *w)
{
    // fprintf(w, "show_current: %c\n", B->deget->c);
    fprintf(w, "%c\n", B->deget->c);
}

void show_banda(banda *B, FILE *w)
{
    nod *p = B->santinela->urmator;
    while (p != NULL)
    {
        if (p == B->deget)
        {
            fprintf(w, "|%c|", p->c);
        }
        else
        {
            fprintf(w, "%c", p->c);
        }
        p = p->urmator;
    }
    fprintf(w, "\n");
}

void EXECUTE(banda *B, coada *C, Stiva *U, Stiva *R, FILE *r, FILE *w)
{
    if (C->primul != NULL)
    {
        element *e = C->primul;

        if (strcmp(e->operatie, "MOVE_LEFT") == 0)
            move_left(B, C, U, r, w);
        else if (strcmp(e->operatie, "MOVE_RIGHT") == 0)
            move_right(B, C, U, r, w);
        else if (strcmp(e->operatie, "MOVE_LEFT_CHAR") == 0)
            move_left_char(B, r, w, e->c);
        else if (strcmp(e->operatie, "MOVE_RIGHT_CHAR") == 0)
            move_right_char(e->c, B, r, w);
        else if (strcmp(e->operatie, "INSERT_LEFT") == 0)
            insert_left(B, r, w, e->c);
        else if (strcmp(e->operatie, "INSERT_RIGHT") == 0)
            insert_right(B, r, w, e->c);
        else if (strcmp(e->operatie, "WRITE") == 0)
            write_char(B, U, R, r, w, e->c);

        C->primul = e->urmator;
        free(e);
    }
}
void elibereaza_banda(banda *B)
{
    // parcurge toate celulele benzii și eliberează memoria alocată
    nod *p = B->santinela;
    nod *q;
    while (p != NULL)
    {
        q = p;
        p = p->urmator;
        free(q);
    }
    free(B);
}

void elibereaza_stiva(Stiva *R)
{
    element_stiva *varf = R->varf;
    element_stiva *p;
    while (varf != NULL)
    {
        p = varf;
        varf = varf->urmator;
        // free(p->pozitie);
        free(p);
    }
    free(R);
}

void elibereaza_coada(coada *C)
{
    // parcurge toate elementele cozii și eliberează memoria alocată
    element *p = C->primul;
    element *q;
    while (p != NULL)
    {
        q = p;
        p = p->urmator;
        free(q);
    }
    free(C);
}

void adauga_coada(coada *c, char *operatie, char q)
{
    element *e = (element *)malloc(sizeof(element));
    strcpy(e->operatie, operatie);
    e->c = q;
    e->urmator = NULL;
    if (c->primul == NULL)
    {
        c->primul = e;
        c->ultimul = e;
    }
    else
    {
        c->ultimul->urmator = e;
        c->ultimul = e;
    }
}
/*
void sterge_coada(coada *c)
{
    if (c->primul != NULL)
    {
        element *e = c->primul;
        c->primul = c->primul->urmator;
        free(e);
    }
}

int coada_vida(coada *c)
{
    return c->primul == NULL;
}
*/

int main()
{
    // creaza o instanta a benzii si cozii
    banda *B = (banda *)malloc(sizeof(banda));
    coada *C = (coada *)malloc(sizeof(coada));
    C->primul = NULL;
    C->ultimul = NULL;

    // initializeaza banda cu o santinela goala
    B->santinela = (nod *)malloc(sizeof(nod));
    B->santinela->precedent = NULL;

    nod *n2 = (nod *)malloc(sizeof(nod));
    B->santinela->urmator = n2;
    n2->precedent = B->santinela;
    n2->urmator = NULL;
    n2->c = '#';

    // pune degetul sa arate catre nodul 2
    B->deget = n2;

    // se creeaza stiva undo si redo
    Stiva *U = (Stiva *)malloc(sizeof(Stiva));
    U->varf = NULL;
    U->dimensiune = 0;

    Stiva *R = (Stiva *)malloc(sizeof(Stiva));
    R->varf = NULL;
    R->dimensiune = 0;

    // deschid fisierele de intrare si iesire
    FILE *f = fopen("tema1.in", "r");
    FILE *w = fopen("tema1.out", "w");

    // citesc operatiile din fisierul de intrare si execut
    char op[20], c;
    int n, i;
    fscanf(f, "%d", &n);

    for (i = 1; i <= n; i++)
    {
        fscanf(f, "%s", op);
        if (strcmp(op, "MOVE_LEFT") == 0 || strcmp(op, "MOVE_RIGHT") == 0)
        {
            adauga_coada(C, op, '#');
        }
        else if (strcmp(op, "MOVE_LEFT_CHAR") == 0 ||
                 strcmp(op, "MOVE_RIGHT_CHAR") == 0 ||
                 strcmp(op, "WRITE") == 0 ||
                 strcmp(op, "INSERT_LEFT") == 0 ||
                 strcmp(op, "INSERT_RIGHT") == 0)
        {
            fscanf(f, " %c", &c);
            adauga_coada(C, op, c);
        }
        else if (strcmp(op, "SHOW_CURRENT") == 0)
        {
            show_current(B, w);
        }
        else if (strcmp(op, "SHOW") == 0)
        {
            show_banda(B, w);
        }
        else if (strcmp(op, "EXECUTE") == 0)
        {
            EXECUTE(B, C, U, R, f, w);
        }
        else if (strcmp(op, "WRITE") == 0)
        {
            fscanf(f, " %c", &c);
            adauga_coada(C, op, c);
        }
        else if (strcmp(op, "UNDO") == 0)
        {
            if (U->dimensiune > 0)
            {
                adauga_stiva(R, B->deget);
                B->deget = U->varf->pozitie;
                sterge_din_stiva(U);
            }
        }
        else if (strcmp(op, "REDO") == 0)
        {
            if (R->dimensiune > 0)
            {
                adauga_stiva(U, B->deget);
                B->deget = R->varf->pozitie;
                sterge_din_stiva(R);
            }
        }
    }

    // inchide fisierele de intrare si iesire
    fclose(f);
    fclose(w);

    // elibereaza memoria alocata pentru banda si coada
    elibereaza_banda(B);
    elibereaza_coada(C);
    elibereaza_stiva(U);
    elibereaza_stiva(R);

    return 0;
}
