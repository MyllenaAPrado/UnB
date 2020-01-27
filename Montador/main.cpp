#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <algorithm>
#include "processadormacro.h"
#include "preprocessador.h"
#include "montador.h"


int main(int argc, char** argv) {

    std::string nomeArquivo, nomearq, nomearqsaida, escrever, arq_entr, codigo, arq, final, auxiliar;
    if (argc != 2) {
        std::cout << "\n*** Numero de argumentos na linha de comando invalido. Exemplo: ./tradutor nomedoarquivo.asm***"
                  << "\n\n";
        exit(1);
    }
    for (int i = 0; i < argc; i++) {

        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    //abrir arquivo no modo de leitura
    arq_entr = argv[1];
    nomearq = argv[1];
    nomearq.append(".pre");
    codigo = nomearq;
    arq = argv[1];
    arq.append(".obj");


    //arq_entr = "arquivo11.txt";
    std::ifstream arquivo_ent(arq_entr);
    if (!arquivo_ent) {
        std::cout << "Ocorreu um erro na abertura do arquivo" << std::endl;
    }
    nomearqsaida = "macro.txt";
    std::ofstream arquivosaidap(nomearqsaida);
    if (!arquivosaidap) {
        std::cout << "Ocorreu um erro na criacao do arquivo de saida" << std::endl;
    }
    std::cout << "*** Preprocessamento ***" << std::endl;

    processaarquivo(arquivo_ent, arquivosaidap);

    nomeArquivo = "macro.txt";
    std::ifstream arquivo(nomeArquivo); //modo leitura
    if (!arquivo) {
        std::cout << "Ocorreu um erro na abertura do arquivo" << std::endl;
    }
    //nomearq = "preprocessado.pre";
    std::ofstream arquivosaida(nomearq);
    if (!arquivo) {
        std::cout << "Ocorreu um erro na criacao do arquivo de saida" << std::endl;
    }
    std::cout << std::endl << "*** Expande macro ***" << std::endl;

    expandirmacro(arquivo, arquivosaida);

    //codigo = "preprocessado.pre";
    //codigo = "monta";
    std::ifstream arquivomontador(codigo); //modo leitura
    if (!arquivomontador) {
        std::cout << "Ocorreu um erro na abertura do arquivo" << std::endl;
    }
    //arq = "objeto.obj.txt";
    std::ofstream codigoobjeto(arq);
    if (!codigoobjeto) {
        std::cout << "Ocorreu um erro na criacao do arquivo de saida" << std::endl;
    }

    std::cout << std::endl << "*** Montador ***" << std::endl;
    montar(arquivomontador, codigo, codigoobjeto);


    return 0;

}
