#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

//Registro para eleitor
struct eleitor{
  int id;
  __int128_t D;
  __int128_t P;
  __int128_t Q;
  __int128_t E;
  __int128_t N;
  __int128_t s;
  int v;
  struct eleitor *prox;
};

//Registro para votos
struct voto{
  int v;
  __int128_t s;
  struct voto *prox;
};

typedef struct voto Votos;
typedef struct eleitor Eleitores;
//Variavel usada de apoio no algorimo de euclides
int eucl;

//Declaracao da funcao para criar um registro para eleitor
Eleitores * cria_eleitor(int i, __int128_t d, int64_t p, int64_t q, __int128_t  n, __int128_t e);


//Calcula potencia i de um numero p, pequenos
int64_t potencia(int64_t p, int i)
{
 int64_t total = 1;

  while(i!=0){
    total*=p;
    i--;
  }
  return total;
}

//Algorimo de potencia modular
__int128_t potenciamodular(__int128_t b, __int128_t t, __int128_t n)
{
  __uint128_t a=b, r=1;
  while(t!=0){

    if(t%2!=0){
    r=(r*a)%n;
    t=t-1;
    }
    else if(t%2==0){
      a=(a*a)%n;
      t=t/2;
    }
  }
  return r;
}

//Verifica se um numero eh primo
int MillerRabin(int64_t n, int a, int64_t p)
{
  while(n%2==0){
    n = n/2;
    if(potenciamodular(a,n,p)==p-1)
      return 1;
  }
  if(potenciamodular(a,n,p)==1)
    return 1;
  return 0;
}

//Geracao de numero primo
int64_t primo(unsigned int t)
{
  int i, j;
  int trya = 1;
  int a[13] = {2,3,5,7,11,13,17,19,23,29,31,37,41};
  int64_t p, n; 

  j = 13;
  //Gera um numero aleatorio
  p = t + rand()%(t-1);

  //Se tiver menos de 42 bits, obter outros valores de A para o algoritmo de MillerRabin
  if(p<4759123141){
    a[0]=2;
    a[1]=7;
    a[2]=61;
    j=3;
  }

  //Verifica se o numero gerado eh primo (testando para diferentes A's), caso nao seja eh gerado outro numero e verificado se eh primo
  n = p-1;
  for(i=0; i<j; i++){
    if(!MillerRabin(n,a[i],p)){
      if(trya<=20){
        p+=potencia(2,trya);
        trya++;
      }
      if(trya>20 || p>2*t-1){
        trya=1;
        p = t + rand()%(t-1);
        if(p<4759123141){
          a[0]=2;
          a[1]=7;
          a[2]=61;
          j=3;
        }
        else{    
          a[0]=2;
          a[1]=3;
          a[2]=5;
          j=13;
        }
      }
      n = p-1;
      i=-1;
    }
  }
  return p;
}

//Funcoes para o Algoritmo de euclides para inverso modular
//a variavel eucl sera 0 se o resto final no algoritmo de euclides for 0, ou seja, nao ha inverso modular
__int128_t Euclides(__int128_t e, __int128_t m)
{
  __int128_t r = m%e;
  __int128_t num=0;

  if(eucl == 1){
    if(r == 0)
      eucl = 0;
    else if(r == 1)
      num = -(m/e);
    else if(e%r == 0)
      eucl = 0;
    else if(e%r == 1)
      num = 1-(m/e)*Euclides(m%e,e);
    else if(r%(e%r) == 0)
      eucl = 0;
    else if(r%(e%r) == 1)
      num = -(m/e)*Euclides(r,e)+Euclides(e%r,r);
    else
      num = Euclides(e%r,r)-Euclides(r,e)*(m/e);
    if(eucl == 1)
      return num;
  }
  return 0;
}

//Gerador de chaves para o RSA
void gerar_chave(int t, Eleitores **eleitor, int i)
{
  int64_t P, Q, D;
  __int128_t D_aux, N;
  int E;
  //Gera o primeiro primo
  P = primo(t);
  do{
    //Gera o segundo primo
    Q = primo(t);
  }while(P==Q);
  N = P*Q;
  do{
    //Determina a chave publica e privada
    for(E = 1 + rand()%(N-2); E%2==0; E = 1 + rand()%(N-2));
    eucl = 1;
    D_aux = Euclides(E,(P-1)*(Q-1));
    D = D_aux;
    if(D<0)
      D=D+(P-1)*(Q-1);
  }while(D==0);
  //Cria o eleitor com suas chaves
  *eleitor = cria_eleitor(i,D,P,Q,N,E);
}

//Cifragem e decifragem com algoritmo RSA
__uint128_t RSA(__uint128_t M, __int128_t chave, __uint128_t N)
{
  return potenciamodular(M, chave, N);
}

//Funcao que une id do eleitor e seu voto, e sua inversa
__uint128_t function(int i, int v)
{
  return 10*i+v;
}

__uint128_t inversefunction(__uint128_t s, int a)
{
  if(a==1)
    return s%10;
  else
    return s/10;
}

//Operacoes com a lista linear de votos
void insere_voto(__uint128_t s, int v, Votos **votos)
{
  Votos *nova;

  nova = (Votos *) malloc(sizeof (Votos));
  nova->v = v;
  nova->s = s;
  nova->prox = *votos;
  *votos = nova;
}

void remove_votos(Votos **votos)
{
  Votos *lixo;

  while((*votos) != NULL){
    lixo = (*votos);
    (*votos) = (*votos)->prox;
    free(lixo);
  }
}

int busca_voto(Eleitores *eleitor, Votos *votos)
{
  Votos *p;
  p = votos;
  __uint128_t v, i, voto = RSA(eleitor->s,eleitor->D,eleitor->N);
  v = inversefunction(voto,1);
  i = inversefunction(voto,0);
  while (p != NULL && (p->s != eleitor->s || p->v != v)){
    p = p->prox;
  }
  if(p!=NULL && i == eleitor->id)
    return 1;
  return 0;
}

void imprimir_votos(Eleitores *eleitores)
{
  Eleitores *p;
  int64_t s;
  for(p=eleitores; p!=NULL; p=p->prox){
    s=p->s;
    printf("%ld ",s);
  }
  printf("\n");
}

//Operacoes com lista linear de eleitores
Eleitores * cria_eleitor(int i, __int128_t d, int64_t p, int64_t q, __int128_t  n, __int128_t e)
{
  Eleitores *nova;

  nova = (Eleitores *) malloc(sizeof (Eleitores));
  nova->id = i;
  nova->D = d;
  nova->P = p;
  nova->Q = q;
  nova->N = n;
  nova->E = e;
  nova->s = -1;
  return nova;
}

void insere_eleitor(int i, __uint128_t s, Eleitores **eleitores)
{
  Eleitores* nova;
  Eleitores* ant = NULL;
  Eleitores* p = *eleitores;

  while (p != NULL && p->s > s){ 
    ant = p;
    p = p->prox;
  }
  nova = (Eleitores*) malloc(sizeof(Eleitores));
  nova->id = i;
  nova->s = s;
  nova->prox = NULL;

  if (ant == NULL){
    nova->prox = *eleitores;
    *eleitores = nova;
  }
  else{
    nova->prox = ant->prox;
    ant->prox = nova;
  }
}

Eleitores* busca_eleitor(int i, Eleitores *eleitores)
{
  Eleitores *p;
  p = eleitores;

  while(p != NULL && p->id != i)
    p = p->prox;
  return p;
}

void remove_eleitor(Eleitores **eleitor)
{
  free(*eleitor);
  *eleitor = NULL;
}

int main(int argc, char *argv[])
{
  //Cria semente para o numero aleatorio
  srand(time(NULL));

  Eleitores *eleitor = NULL, *eleitores = NULL;
  Votos *votos = NULL;
  uint64_t T=0;
  int64_t P=0, Q=0;
  __int128_t E=0, D=0, N=0;
  char b;
  int i = 0, e = 0, c = 0, k = 0, j = 0, voto = 0, count = 0, count2 = 0, vt, ini = 0, *candidatos = NULL;
  __uint128_t s = 0, protesto = 0, v = 0, id = 0;
  uint64_t s_aux = 0, d_aux = 0, n_aux = 0, e_aux = 0, v_aux = 0;

  //Se a opcao for de Zona Eleitoral
  if(*argv[1]=='Z'){
    do{
      scanf(" %c", &b);
    switch (b){
        case 'I':
          scanf("%d %d %ld", &e, &c, &T);
          //Se ja nao tiver inicializado
          if(!ini){
            //Cria vetor para armazenar os votos para os candidatos
            candidatos = (int*) calloc((c+1),sizeof(int));
            ini=1;
            printf("C\n");
          }
          else
            printf("E\n");
          break;
        case 'R':
          scanf("%d %ld", &i, &s_aux);
          s = s_aux;
          //Se a zona tiver inicializado, o eleitor ja nao tiver votado e o eleitor for valido
          if(ini && busca_eleitor(i,eleitores) == NULL && i<e){
            count++;
            //Inserir o eleitor na lista de eleitores
            insere_eleitor(i,s,&eleitores);
            printf("C\n");
          }
          else
            printf("E\n");
          break;
        case 'L':
          //Imprime os eleitores
          printf("C %d ", count);
          imprimir_votos(eleitores);
          break;
        case 'A':
          scanf("%d %ld %ld", &i, &d_aux, &n_aux);
          D = d_aux;
          N = n_aux;
          eleitor = busca_eleitor(i,eleitores);
          //Se a zona tiver inicializado, o eleitor ja tiver votado, o voto nao tiver sido aberto e a chave estiver no limite
          if(ini && eleitor != NULL && eleitor->v==0 && N > potencia(T,2) && N < potencia(2*T-1,2)){
            voto = RSA(eleitor->s,D,N);
            v = inversefunction(voto,1);
            id = inversefunction(voto,0);
            //Se o voto do eleitor for dele mesmo
            if(id == eleitor->id && i == id){
              //Se o voto tiver relacionado a algum candidato
              if(v>=0 && v<c){
                count2++;
                candidatos[v+1]++;
                eleitor->v = v;
                eleitor->D = D;
                eleitor->N = N;
                printf("C\n");
              }
              else
                printf("E\n");
            }
            else{
              printf("E\n");
            }
          }
          else
            printf("E\n");
          break;
        case 'P':
          //Se a zona tiver inicializado
          if(ini)
            printf("C %d", count2);
          //Imprime os eletores e suas chaves
          for(eleitor = eleitores; eleitor!=NULL; eleitor=eleitor->prox){
            if(eleitor->v != 0){
              s_aux = eleitor->s;
              printf(" %d %ld", eleitor->v, s_aux);
            }
          }
          printf("\n");
          break;
        case 'Q':
          scanf("%d %ld %ld %ld", &i, &s_aux, &d_aux, &n_aux);
          s = s_aux;
          D = d_aux;
          N = n_aux;
          eleitor = busca_eleitor(i,eleitores);
          //Se a zona tiver inicializado, o eleitor ter votado, e a chave estiver dentro do limite
          if(ini && eleitor !=NULL && N > potencia(T,2) && N < potencia(2*T-1,2)){
            protesto = RSA(s,D,N);
            voto = RSA(eleitor->s,eleitor->D,eleitor->N);
            //Verificar se o protesto faz sentido
            if(inversefunction(voto,0) == inversefunction(protesto,0) && inversefunction(voto,1) == inversefunction(protesto,1) && i == inversefunction(protesto,0)){
              printf("E\n");
            }
            else
              printf("C\n");
          }
          else
            printf("E\n");
          break;
        case '!':
          scanf("%d %d %ld", &i, &vt, &s_aux);
          s = s_aux;
          eleitor = busca_eleitor(i,eleitores);
          //Se o eleitor existir
          if(eleitor!=NULL){
            //Sofrer ataque
            eleitor->v = v;
            eleitor->s = s;
            printf("C\n");
          }
          else
            printf("E\n");
          break;
    }}while(b!='T');
    printf("C %d", c);
    //Imprime o total de votos para cada candidato
    for(k=1; k<=c;k++){
      printf(" %d", candidatos[k]);
    }
    //else
    //  printf("E");
    printf("\n");
    remove_eleitor(&eleitores);
  }
  //Se a opcao for como Eleitor
  else if(*argv[1]=='E'){
    do{
      scanf(" %c", &b);
    switch (b){
        case 'I':
          scanf("%d %ld", &i, &T);
          //Verificar se o eleitor ja votou para um novo poder votar
          if(voto)
            remove_eleitor(&eleitor);
          //Se tiver vaga para votar
          if(eleitor==NULL){
            voto = 0;
            //Gera chaves para o eleitor
            gerar_chave(T,&eleitor,i);
            printf("C\n");
          }
          else
            printf("E\n");
          break;
        case 'P':
          scanf("%d %ld %ld %ld %ld", &i, &P, &Q, &e_aux, &d_aux);
          E = e_aux;
          D = d_aux;
          //Verifica se o eleitor ja votou para um novo poder votar
          if(voto)
            remove_eleitor(&eleitor);
          //Se tiver vaga para votar
          if(eleitor==NULL){
            voto = 0;
            //Criar o registro do eleitor com as chaves indicadas
            eleitor = cria_eleitor(i, D, P, Q, P*Q, E);
            printf("C\n");
          }
          else
            printf("E\n");
          break;
        case 'V':
          scanf("%ld", &v_aux);
          v = v_aux;
          //Se o eleitor tiver gerado chaves e nao tiver votado
          if(eleitor!=NULL && !voto){
            voto = 1;
            //Cifrar o seu voto
            s = RSA(function(eleitor->id,v),eleitor->E,eleitor->N);
            eleitor->s = s;
            s_aux = s;
            printf("C %ld\n", s_aux);
          }
          else
            printf("E\n");
          break;
        case 'R':
          //Se o eleitor tiver gerado chaves
          if(eleitor != NULL){
            d_aux = eleitor->D;
            n_aux = eleitor->N;
            //Revelar suas chaves
            printf("C %d %ld %ld\n", eleitor->id, d_aux, n_aux);
          }
          else
            printf("E\n");
          break;
        case 'C':
          scanf("%d", &k);
          for(j=0; j<k; j++){
            scanf("%ld %ld",&s_aux,&v_aux);
            v = v_aux;
            insere_voto(v, s_aux, &votos);
          }
          //Se o eleitor tiver gerado chaves e votado
          if(eleitor!=NULL && voto){
            //Verificar se seu voto se encontra na lista
            if(busca_voto(eleitor,votos))
                printf("C\n");
            else{
              s_aux = eleitor->s;
              d_aux = eleitor->D;
              n_aux = eleitor->N;
              printf("E %d %ld %ld %ld\n", eleitor->id, s_aux, d_aux, n_aux);
            }
          }
          else
            printf("E \n");
          remove_votos(&votos);
          break;
    }}while(b!='T');
    v_aux = eleitor->P;
    s_aux = eleitor->Q;
    e_aux = eleitor->E;
    d_aux = eleitor->D;
    //Imprimir as chaves do ultimo eleitor
    printf("C %ld %ld %ld %ld\n", v_aux, s_aux, e_aux, d_aux);
    remove_eleitor(&eleitor);
  }
  return 0;
}
