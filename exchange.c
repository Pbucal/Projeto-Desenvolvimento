#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USU 50
#define ARQ_USU "usuarios.txt"
#define ARQ_COT "cotacoes.txt"

typedef struct {
    char cpf[12];
    char nome[50];   
    char senha[7];
    double reais, btc, eth, xrp;
} Usuario;

Usuario usu[MAX_USU];
int nUsu = 0;
double cot[3];                              
const char *NOME[3] = {"BTC", "ETH", "XRP"};
const double TX_COMPRA[3] = {0.02, 0.01, 0.01};
const double TX_VENDA[3]  = {0.03, 0.02, 0.01};

double *cripto(Usuario *u, int i) {
    if (i == 0) return &u->btc;
    if (i == 1) return &u->eth;
    return &u->xrp;
}

void lerLinha(char *buf, int tam) {
    if (!fgets(buf, tam, stdin)) { printf("\nEntrada encerrada.\n"); exit(0); }
    buf[strcspn(buf, "\n")] = '\0';
}

double lerDouble(const char *msg) {
    char buf[64], *fim;
    double v;
    while (1) {
        printf("%s", msg);
        lerLinha(buf, sizeof buf);
        for (fim = buf; *fim; fim++) if (*fim == ',') *fim = '.';
        v = strtod(buf, &fim);
        if (fim != buf && *fim == '\0' && v > -1e12 && v < 1e12) return v;
        printf("Valor invalido.\n");
    }
}

int lerInt(const char *msg) {
    char buf[32], *fim;
    long v;
    while (1) {
        printf("%s", msg);
        lerLinha(buf, sizeof buf);
        v = strtol(buf, &fim, 10);
        if (fim != buf && *fim == '\0') return (int)v;
        printf("Opcao invalida.\n");
    }
}

int senhaValida(const char *s) {
    int i;
    if (strlen(s) != 6) return 0;
    for (i = 0; i < 6; i++)
        if (s[i] < '0' || s[i] > '9') return 0;
    return 1;
}

void salvarUsuarios(void) {
    int i;
    FILE *f = fopen(ARQ_USU, "w");
    if (!f) { printf("Erro ao salvar usuarios.\n"); return; }
    for (i = 0; i < nUsu; i++)
        fprintf(f, "%s %s %s %.8f %.12f %.12f %.12f\n", usu[i].cpf, usu[i].nome,
                usu[i].senha, usu[i].reais, usu[i].btc, usu[i].eth, usu[i].xrp);
    fclose(f);
}

void salvarCotacoes(void) {
    FILE *f = fopen(ARQ_COT, "w");
    if (!f) { printf("Erro ao salvar cotacoes.\n"); return; }
    fprintf(f, "%.8f %.8f %.8f\n", cot[0], cot[1], cot[2]);
    fclose(f);
}

void criarPadrao(void) {
    FILE *f = fopen(ARQ_USU, "r");
    if (f) fclose(f);
    else {
        f = fopen(ARQ_USU, "w");
        fprintf(f, "12345678900 Gabriela_Biondi 123456 0 0 0 0\n");
        fprintf(f, "98765432100 Joao_Teste 654321 0 0 0 0\n");
        fclose(f);
    }
    f = fopen(ARQ_COT, "r");
    if (f) fclose(f);
    else {
        f = fopen(ARQ_COT, "w");
        fprintf(f, "350000.00 16000.00 2.57\n");
        fclose(f);
    }
}

void carregar(void) {
    FILE *f = fopen(ARQ_USU, "r");
    nUsu = 0;
    if (f) {
        while (nUsu < MAX_USU &&
               fscanf(f, "%11s %49s %6s %lf %lf %lf %lf", usu[nUsu].cpf,
                      usu[nUsu].nome, usu[nUsu].senha, &usu[nUsu].reais,
                      &usu[nUsu].btc, &usu[nUsu].eth, &usu[nUsu].xrp) == 7)
            nUsu++;
        fclose(f);
    }
    f = fopen(ARQ_COT, "r");
    if (f) {
        if (fscanf(f, "%lf %lf %lf", &cot[0], &cot[1], &cot[2]) != 3) {
            cot[0] = 350000; cot[1] = 16000; cot[2] = 2.57;
        }
        fclose(f);
    }
}

void registrar(Usuario *u, char sinal, double valor, const char *moeda,
               double ct, double tx) {
    char nomeArq[40], data[32];
    time_t t = time(NULL);
    strftime(data, sizeof data, "%d-%m-%Y %H:%M", localtime(&t));
    sprintf(nomeArq, "extrato_%s.txt", u->cpf);
    FILE *f = fopen(nomeArq, "a");
    if (!f) { printf("Erro ao gravar extrato.\n"); return; }
    fprintf(f, "%s %c %.2f %s CT: %.2f TX: %.2f REAL: %.2f BTC: %.4f ETH: %.4f XRP: %.4f\n",
            data, sinal, valor, moeda, ct, tx, u->reais, u->btc, u->eth, u->xrp);
    fclose(f);
}

void mostrarNome(Usuario *u) {
    char n[50];
    int i;
    strcpy(n, u->nome);
    for (i = 0; n[i]; i++) if (n[i] == '_') n[i] = ' ';
    printf("Nome: %s\n", n);
    printf("CPF: %.3s.%.3s.%.3s-%s\n", u->cpf, u->cpf + 3, u->cpf + 6, u->cpf + 9);
}

void mostrarSaldo(Usuario *u) {
    mostrarNome(u);
    printf("Reais: %.2f\n", u->reais);
    printf("Bitcoin: %.8f\n", u->btc);
    printf("Ethereum: %.8f\n", u->eth);
    printf("Ripple: %.8f\n", u->xrp);
}

int confirmarSenha(Usuario *u) {
    char s[32];
    printf("Senha: ");
    lerLinha(s, sizeof s);
    if (strcmp(s, u->senha) != 0) { printf("Senha incorreta.\n"); return 0; }
    return 1;
}

void mostrarCotacoes(void) {
    printf("Cotacoes:\n1. Bitcoin : R$ %.2f\n2. Ethereum: R$ %.2f\n3. Ripple  : R$ %.2f\n",
           cot[0], cot[1], cot[2]);
}

void extrato(Usuario *u) {
    char nomeArq[40], linha[256];
    if (!confirmarSenha(u)) return;
    mostrarNome(u);
    sprintf(nomeArq, "extrato_%s.txt", u->cpf);
    FILE *f = fopen(nomeArq, "r");
    if (!f) { printf("Sem operacoes.\n"); return; }
    while (fgets(linha, sizeof linha, f)) printf("%s", linha);
    fclose(f);
}

void depositar(Usuario *u) {
    double v = lerDouble("Valor do deposito: ");
    if (v <= 0) { printf("Valor deve ser positivo.\n"); return; }
    u->reais += v;
    salvarUsuarios();
    registrar(u, '+', v, "REAL", 0.0, 0.0);
    mostrarSaldo(u);
}

void sacar(Usuario *u) {
    double v = lerDouble("Valor do saque: ");
    if (v <= 0) { printf("Valor deve ser positivo.\n"); return; }
    if (v > u->reais + 1e-9) { printf("Saldo insuficiente.\n"); return; }
    u->reais -= v;
    if (u->reais < 0) u->reais = 0;
    salvarUsuarios();
    registrar(u, '-', v, "REAL", 0.0, 0.0);
    mostrarSaldo(u);
}

void comprar(Usuario *u) {
    if (!confirmarSenha(u)) return;
    mostrarCotacoes();
    int op = lerInt("Qual comprar (1-3): ");
    if (op < 1 || op > 3) { printf("Opcao invalida.\n"); return; }
    int i = op - 1;
    double v = lerDouble("Quanto gastar em reais (sem a taxa): ");
    if (v <= 0) { printf("Valor deve ser positivo.\n"); return; }
    double total = v * (1 + TX_COMPRA[i]);
    if (total > u->reais + 1e-9) {
        printf("Saldo insuficiente (necessario R$ %.2f com taxa).\n", total);
        return;
    }
    u->reais -= total;
    if (u->reais < 0) u->reais = 0;
    *cripto(u, i) += v / cot[i];
    salvarUsuarios();
    registrar(u, '+', v, NOME[i], cot[i], TX_COMPRA[i]);
    mostrarSaldo(u);
}

void vender(Usuario *u) {
    if (!confirmarSenha(u)) return;
    mostrarCotacoes();
    int op = lerInt("Qual vender (1-3): ");
    if (op < 1 || op > 3) { printf("Opcao invalida.\n"); return; }
    int i = op - 1;
    double v = lerDouble("Quanto vender em reais (antes da taxa): ");
    if (v <= 0) { printf("Valor deve ser positivo.\n"); return; }
    double qtd = v / cot[i];
    if (qtd > *cripto(u, i) + 1e-8) { printf("Saldo de %s insuficiente.\n", NOME[i]); return; }
    if (qtd > *cripto(u, i)) qtd = *cripto(u, i);  
    *cripto(u, i) -= qtd;
    u->reais += v * (1 - TX_VENDA[i]);
    salvarUsuarios();
    registrar(u, '-', v, NOME[i], cot[i], TX_VENDA[i]);
    mostrarSaldo(u);
}

void atualizarCotacao(void) {
    int i;
    for (i = 0; i < 3; i++) {
        double var = ((double)rand() / RAND_MAX) * 0.10 - 0.05; 
        cot[i] *= 1 + var;
    }
    salvarCotacoes();
    mostrarCotacoes();
}

Usuario *login(void) {
    char cpf[32], senha[32];
    int t, i;
    for (t = 0; t < 3; t++) {
        printf("CPF (so numeros): ");
        lerLinha(cpf, sizeof cpf);
        printf("Senha (6 digitos): ");
        lerLinha(senha, sizeof senha);
        if (senhaValida(senha))
            for (i = 0; i < nUsu; i++)
                if (!strcmp(usu[i].cpf, cpf) && !strcmp(usu[i].senha, senha))
                    return &usu[i];
        printf("CPF ou senha invalidos.\n");
    }
    return NULL;
}

int main(void) {
    srand((unsigned)time(NULL));
    criarPadrao();
    carregar();

    Usuario *u = login();
    if (!u) { printf("Acesso negado.\n"); return 1; }

    int op;
    do {
        printf("\n1. Consultar saldo\n2. Consultar extrato\n3. Depositar\n"
               "4. Sacar\n5. Comprar criptomoedas\n6. Vender criptomoedas\n"
               "7. Atualizar cotacao\n8. Sair\n");
        op = lerInt("Opcao: ");
        switch (op) {
            case 1: if (confirmarSenha(u)) mostrarSaldo(u); break;
            case 2: extrato(u); break;
            case 3: depositar(u); break;
            case 4: sacar(u); break;
            case 5: comprar(u); break;
            case 6: vender(u); break;
            case 7: atualizarCotacao(); break;
            case 8: printf("Ate logo.\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 8);
    return 0;
}
