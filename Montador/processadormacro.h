//struct da tabela de macros
struct tabelamacro{
    std::string nomeMacro;
    std::string conteudomacro;
    std::string parametros;
    int nlinhas;
    int nparametros;
    tabelamacro (){};
    tabelamacro (std::string nome, std::string conteudo, std::string parame, int linhas, int nparame): nomeMacro(nome), conteudomacro(conteudo), parametros(parame), nlinhas(linhas), nparametros(nparame){};
};

int confereparametro(std::string rotulo);
void expandemacrosemparametro(std::vector<tabelamacro> &tmacros,std::string linhacod, int i, int quantmacro, std::ofstream &arquivosaida, int &lugar);
void expandemacro(std::vector<tabelamacro> &tmacros,std::string linhacod, int i, std::ofstream &arquivosaida, int &lugar);
void chamamacro(std::string linha, int quantmacro, std::vector<tabelamacro> &tmacros, std::ofstream &arquivosaida, int &lugar);
void corpomacro(std::string nome, std::string parametro, int numparam, std::ifstream &arquivo, std::vector<tabelamacro> &tmacros, int i, int &quantmacros, std::ofstream &arquivosaida, int &lugar);
void conferemacro(std::string linha, int &quantmacros, std::vector<tabelamacro>  &tmacros, std::ifstream &arquivo, std::ofstream &arquivosaida, int &lugar);

int confereparametro(std::string rotulo, int &lugar){
       //se o rotulo tem o nome de uma instrucao]
    if (rotulo.front() != '&') {
        std::cout << "Erro lexico, rotulo não comeca com &. Linha: " << lugar << std::endl;
        return 1;
    }

     if (rotulo == "ADD" || rotulo == "SUB" || rotulo == "MULT" || rotulo == "DIV" || rotulo == "JMP" ||
            rotulo == "JMPN" || rotulo == "JMPP" || rotulo == "JMPZ" || rotulo == "COPY" || rotulo == "LOAD" ||
            rotulo == "STORE" || rotulo == "INPUT" || rotulo == "OUTPUT" || rotulo == "STOP") {
            std::cout << "Erro semantico, rotulo com mesmo nome de instrucao. Linha: " << lugar << std::endl;
            return 1;
        }
        //se o rotulo tem o nome de uma diretiva
        if (rotulo == "CONST" || rotulo == "SPACE" || rotulo == "SECTION" || rotulo == "EQU" || rotulo == "MACRO" ||
            rotulo == "END") {
            std::cout << "Erro semantico, rotulo com mesmo nome de diretiva. Linha: " << lugar << std::endl;
            return 1;
        }
        //caracter invalido no rotulo
        for (int i = 1; i < rotulo.length(); i++) {
            if ((rotulo.at(i) > '9' || rotulo.at(i) < '0') && (rotulo.at(i) != '_') &&
                (rotulo.at(i) > 'Z' || rotulo.at(i) < 'A')&& (rotulo.at(i) != '+')) {
                std::cout << "Erro lexico, token nao permitido. Linha: " << lugar << std::endl;
                return 1;
            }
        }
        //se o rotulo comeca com numero
        if (rotulo.front() <= '9' && rotulo.front() >= '0') {
            std::cout << "Erro lexico, rotulo começa com número. Linha: " << lugar << std::endl;
            return 1;
        }
        //se o rotulo tem tamanho maior que 50
        if (rotulo.length() > 50) {
            std::cout << "Erro lexico, rotulo com tamanho maior que o permitido. Linha: " << lugar << std::endl;
            return 1;
        }

    return 0;
}

void expandemacrosemparametro(std::vector<tabelamacro> &tmacros,std::string linhacod, int i, int quantmacro, std::ofstream &arquivosaida, int &lugar){
    std::string instrucao, linhamacro, instcod, op1, op2, codigo, parametros, opmacro, inst, linhaexp;
    int k, nparametros, g, chamacro;

    //pega conteudo da macro na tabela
    linhamacro = tmacros[i].conteudomacro;
    //pega os parametros da macro
    parametros = tmacros[i].parametros;
    // pega o conteudodamacro
    std::stringstream palavramacro(linhamacro);
    nparametros = tmacros[i].nparametros;
    //percorre as linhas macro da tabela
    for (k = 1; k <= tmacros[i].nlinhas; k++){
        chamacro = 0;
        //linha do código do programa que chamou a macro
        std::stringstream palavracod(linhacod);
        //pega o nome da macro que foi chamada
        palavracod >> codigo;
        //instrucao da macro da tabela
        palavramacro >> instrucao;
        inst = instrucao;
        if(instrucao == "COPY"){
            palavramacro >> op1;
            palavramacro >> op2;
            arquivosaida << inst << " " << op1 << " " << op2 << std::endl;
        }
        else {//testar a instrução chamada
            //testa se a instrucao tem o nome da macro
            for (g = 1; g <= quantmacro; g++) {
                if (instrucao == tmacros[g].nomeMacro) {
                    chamacro = 1;
                }
            }//caso a instrucao seja a chamada de uma macro
            if (chamacro == 1) {
                do{
                    linhaexp.append(instrucao);
                    linhaexp.append(" ");
                    palavramacro >> instrucao;
                }while (instrucao != "|");

                chamamacro(linhaexp, quantmacro, tmacros, arquivosaida, lugar);
            }
            else{
                palavramacro >> op1;
                arquivosaida << inst << " " << op1 << std::endl;
                palavramacro >> inst;
            }
        }

    }
}

void expandemacro(std::vector<tabelamacro> &tmacros,std::string linhacod, int i, int quantmacro, std::ofstream &arquivosaida, int &lugar){
    std::string instrucao, linhamacro, instcod, op1, op2, codigo, parametros, opmacro, inst, linhaexp, instaux;
    int k, posicao, l, f, nparametros, g, chamamacro;

    //pega conteudo da macro na tabela
    linhamacro = tmacros[i].conteudomacro;
    //pega os parametros da macro
    parametros = tmacros[i].parametros;
    // pega o conteudodamacro
    std::stringstream palavramacro(linhamacro);
    nparametros = tmacros[i].nparametros;
    linhacod.append("|");
    //percorre as linhas macro da tabela
    for (k = 1; k <= tmacros[i].nlinhas; k++){
        chamamacro = 0;
        //parametros da tabela
        std::stringstream pegaparam(parametros);
        //linha do código do programa que chamou a macro
        std::stringstream palavracod(linhacod);


        //pega o nome da macro que foi chamada
        palavracod >> codigo;
        //instrucao da macro da tabela
        palavramacro >> instrucao;
        inst = instrucao;
        posicao = 0;
        if(instrucao == "COPY"){
            //pega o primeiro parametro da inst
            palavramacro >> instrucao;
            instrucao.pop_back();
            //pega o primeiro parametro da definiçao da macro
            pegaparam >> opmacro;
            if (instrucao == opmacro){
                //indica que e o primeiro argumento passado
                posicao = 1;
            }
            else{
                //pega o segundo parametro da definicao da macro
                pegaparam >> opmacro;
                if (instrucao == opmacro){
                    //indica que e o segundo argumento passado
                    posicao = 2;
                }
                else{
                    //indica que e o terceiro argumento passado
                    posicao =3;
                }
            }
            for (l = 1; l <= posicao; l++){//anda linha cod para pegar parametro

                while (codigo.back() != ',' && codigo.back() != '|') {

                    palavracod >> codigo;

                    instaux.append(codigo);
                    instaux.append (" ");
                }

                instaux.pop_back();
                if(instaux.back() == ',' || instaux.back() == '|'){
                    instaux.pop_back();
                }

                codigo = instaux;

                instaux.clear();
            }
            arquivosaida << inst << " " << codigo << "," ;

            //pega o segundo parametro da inst
            palavramacro >> instrucao;
            //volta a ler a linha toda
            std::stringstream pegaparam(parametros);
            //pega o primeiro parametro da definiçao da macro
            pegaparam >> opmacro;
            //linha do código do programa que chamou a macro
            std::stringstream palavracod(linhacod);
            //pega o nome da macro que foi chamada
            palavracod >> codigo;
            if (instrucao == opmacro){
                //indica que e o primeiro argumento passado
                posicao = 1;
            }
            else{
                //pega o segundo parametro da definicao da macro
                pegaparam >> opmacro;
                if (instrucao == opmacro){
                    //indica que e o segundo argumento passado
                    posicao = 2;
                }
                else{
                    //indica que e o terceiro argumento passado
                    posicao =3;
                }
            }
            for (l = 1; l <= posicao; l++){//anda linha cod para pegar parametro

                while (codigo.back() != ',' && codigo.back() != '|') {

                    palavracod >> codigo;

                    instaux.append(codigo);
                    instaux.append (" ");
                }

                instaux.pop_back();
                if(instaux.back() == ',' || instaux.back() == '|'){
                    instaux.pop_back();
                }

                codigo = instaux;

                instaux.clear();
            }
            if (tmacros[i].nparametros == 0){
                codigo = " ";
            }

            arquivosaida << " " << codigo << std::endl;

        }
        else {//testar a instrução chamada
            //testa se a instrucao tem o nome da macro
            for(g = 1; g <= quantmacro; g++) {
                if(instrucao == tmacros[g].nomeMacro){
                    chamamacro = 1;
                    f = g;
                }
            }//caso a instrucao seja a chamada de uma macro
            if( chamamacro == 1) {
                linhaexp.append(instrucao);
                linhaexp.append(" ");
                palavramacro >> instrucao;
                while (instrucao != "|"){
                    //parametros da tabela
                    std::stringstream pegaparam(parametros);
                    // trocar parametro pelo parametro de chamada
                    //tira virgula do parametro

                    if(instrucao.back() == ','){
                        instrucao.pop_back();
                    }
                    pegaparam >> opmacro;
                    if(instrucao == opmacro){
                        posicao = 1;
                    }
                    else{
                        //opmacro.pop_back();
                        pegaparam >> opmacro;
                        if(instrucao == opmacro){
                            posicao = 2;
                        }
                        else{
                            posicao = 3;
                        }
                    }
                    for (l = 1; l <= posicao; l++){//anda linha cod para pegar parametro

                        while (codigo.back() != ',' && codigo.back() != '|') {

                            palavracod >> codigo;

                            instaux.append(codigo);
                            instaux.append (" ");
                        }

                        instaux.pop_back();
                        if(instaux.back() == ',' || instaux.back() == '|'){
                            instaux.pop_back();
                        }

                        codigo = instaux;

                        instaux.clear();
                    }
                    instrucao = codigo;
                    linhaexp.append(" ");
                    linhaexp.append(instrucao);
                    linhaexp.append(",");
                    palavramacro >> instrucao;

                }
                linhaexp.pop_back();
                expandemacro(tmacros, linhaexp, f, quantmacro, arquivosaida, lugar);
            }
            else {
                //pega o primeiro parametro da inst
                palavramacro >> instrucao;
                //pega o primeiro parametro da definiçao da macro
                pegaparam >> opmacro;
                if (instrucao == opmacro) {
                    //indica que e o primeiro argumento passado
                    posicao = 1;
                } else {//recursividade da macro
                    //pega o segundo parametro da definicao da macro
                    pegaparam >> opmacro;
                    if (instrucao == opmacro) {
                        //indica que e o segundo argumento passado
                        posicao = 2;
                    } else {
                        //indica que e o terceiro argumento passado
                        posicao = 3;
                    }
                }

                for (l = 1; l <= posicao; l++){//anda linha cod para pegar parametro


                    while (codigo.back() != ',' && codigo.back() != '|') {

                        palavracod >> codigo;

                        instaux.append(codigo);
                        instaux.append (" ");
                    }

                    instaux.pop_back();
                    if(instaux.back() == ',' || instaux.back() == '|'){
                        instaux.pop_back();
                    }

                    codigo = instaux;

                    instaux.clear();

                }
                //caso a macro nao receba parametro
                if (tmacros[i].nparametros == 0){
                    codigo = " ";
                }

                arquivosaida << inst << " " << codigo << std::endl;
            }
        }
        if(chamamacro == 0){
            palavramacro >> instrucao;
        }
    }
}

void chamamacro(std::string linha, int quantmacro, std::vector<tabelamacro> &tmacros, std::ofstream &arquivosaida, int &lugar){
    std::string token, auxiliar, codigo, instaux;
    int i, achoumacro, contar;
    i = 1;
    contar = 0;
    achoumacro = 0;

    auxiliar = linha;
    auxiliar.append("|");

    //std::cout << auxiliar << "     linha de chamada" << std::endl;
    std::stringstream palavracod(auxiliar);
    std::stringstream palavra(linha);
    palavra >> token;
    do{
        if (token == tmacros[i].nomeMacro){
            palavracod >> codigo;
            contar = contar + 1;

            if(codigo.back() == '|'){
                contar  = 0;
            }
            while (codigo.back() != '|' && contar != 0) {
                palavracod >> codigo;
                instaux.append(codigo);
                instaux.append (" ");
                if(codigo.back() == ','){
                    contar = contar + 1;
                }
            }

            instaux.clear();
            if (tmacros[i].nparametros == 0 && contar == tmacros[i].nparametros){

                expandemacrosemparametro(tmacros, linha, i, quantmacro, arquivosaida, lugar);
                achoumacro = 1;
            }
            else {
                if (contar == tmacros[i].nparametros ) {
                    expandemacro(tmacros, linha, i, quantmacro, arquivosaida, lugar);
                    achoumacro = 1;
                }
                else{
                    std::cout << "ERRO SINTATICO. Numero de argumentos errados. linha: " << lugar << std::endl;
                }
            }
        }
        i++;
    }while (achoumacro == 0 && i <= quantmacro);

     if(achoumacro == 0){
         //escrever no arquivo
         arquivosaida << linha << std::endl;
     }
}

//coloca na tabela o conteudo da macro e todos os parametros
void corpomacro(std::string nome, std::string parametro, int numparam, std::ifstream &arquivo, std::vector<tabelamacro> &tmacros, int i, int &quantmacros, std::ofstream &arquivosaida, int erroparametro, int &lugar){
    std::string conteudodamacro, linha, token1;
    int numlinha, achoumacro;
    numlinha = 0;
    achoumacro = 0;

    do{
        //pega linha do arquivo
        getline(arquivo, linha);
        lugar = lugar + 1;
        std::stringstream palavra(linha);
        //pega na instrucao do corpo da macro
        palavra >> token1;
        //verificar se aqui tem rotulo antes ainda( FAZZEERRRRRR)
        //confere se achou uma macro sem ter encontrado o end anterior
        if(token1.back()== ':') {
            achoumacro = 1;
            std::cout << "Erro semantico, macro sem END. Linha: " << lugar << std::endl;
            quantmacros = quantmacros - 1;
            conferemacro(linha, quantmacros, tmacros, arquivo, arquivosaida, lugar);
        }
        else{
            if (token1 != "END") {
                //verificar se aqui tem section antes ainda( FAZZEERRRRRR)

                if (token1 == "COPY") {
                    conteudodamacro.append(linha);
                    conteudodamacro.append(" ");
                    conteudodamacro.append("|");
                    conteudodamacro.append(" ");
                    numlinha = numlinha + 1;

                } else {
                    conteudodamacro.append(linha);
                    conteudodamacro.append(" ");
                    conteudodamacro.append("|");
                    conteudodamacro.append(" ");
                    numlinha = numlinha + 1;
                }
            }
        }
    }while((token1 != "END" && achoumacro == 0) && token1 != "STOP" && token1 != "SECTION");
    if(erroparametro == 1){
        quantmacros = quantmacros - 1;
    }
    if (achoumacro == 0 && erroparametro == 0) {
        tabelamacro tabelaaux(nome, conteudodamacro, parametro, numlinha, numparam);
        tmacros.push_back(tabelaaux);
       // std::cout << tmacros[i].nomeMacro << std::endl;
       // std::cout << tmacros[i].conteudomacro << std::endl;
       // std::cout << tmacros[i].parametros << std::endl;
       // std::cout << tmacros[i].nparametros << std::endl;
       // std::cout << tmacros[i].nlinhas << std::endl;
    }

}

//confere se na linha há uma declaração de macro
void conferemacro(std::string linha, int &quantmacros, std::vector<tabelamacro>  &tmacros, std::ifstream &arquivo, std::ofstream &arquivosaida, int &lugar){
    int detectaerro, i, numparam, flag, n, repetido, auxiliar, erroparametro, erro;
    detectaerro = 0;
    erro = 0;
    std::string parametro, vetorparametros, variavel;
    std::vector<char> vetorparametro;
    std::string conteudodamacro;
    std::string token1, token2;
    std::stringstream palavra(linha);
    palavra >> token1;
    palavra >> token2;
    numparam = 0;
    if(token2 == "MACRO"){
        //confere se não é um nome proibido
        token1.pop_back();
        if(token1 == "CONST" || token1 == "SPACE" || token1 == "SECTION" || token1 == "EQU" || token1 == "MACRO" || token1 == "END"){
            detectaerro = 1;
            std::cout << "Erro semantico. Linha: " << lugar << std::endl;
        }
        //confere se possui nome proibido de instrução
        if(token1 == "ADD" || token1 == "SUB" || token1 == "MULT" || token1 == "DIV" || token1 == "JMP" || token1 == "JMPN" || token1 == "JMPP" || token1 == "JMPZ" || token1 == "COPY" || token1 == "LOAD" || token1 == "STORE" || token1 == "INPUT" || token1 == "OUTPUT" || token1 == "STOP"){
            detectaerro = 1;
            std::cout << "Erro semantico. Linha: " << lugar << std::endl;
        }

        //confere se o rotulo possui o mesmo nome que outra macro ja declarada
        for(i=0; i <= quantmacros; i++){
            //compara com o nome da macro já armazenado
            if(token1 == tmacros[i].nomeMacro && quantmacros >= 1){
                std::cout << "Erro semantico, macro já definida. Linha: " << lugar << std::endl;
                detectaerro = 1;
            }
        }
        //confere parametro
        repetido =0;
        do{
            flag = 1;
            palavra >> parametro;
            erroparametro = 0;

            //add caso tenha mais que um parametro
            if(parametro.back() == ','){
                flag = 0;
                parametro.pop_back();
                //se nao tem o rotulo
                erroparametro = confereparametro(parametro, lugar);
                if (erro == 0 ){
                    erro = erroparametro;
                }
                //compara com outros parametors já apresentados
                std::stringstream pegarparamen(vetorparametros);

                if (erroparametro == 0) {
                    for (n = 0; n < numparam; n++) {
                        pegarparamen >> variavel;
                        if (parametro == variavel) {
                            std::cout << "Erro semantico, parametro da macro repetido. Linha: " << lugar << std::endl;
                            repetido = 1;
                        }
                    }
                    if (numparam > 3) {
                        std::cout << "Erro sintático mais de 3 argumentos. Linha: " << lugar << std::endl;
                        repetido = 1;
                    }
                }
                if (repetido == 0 && erroparametro == 0){
                    vetorparametros.append(parametro);
                    vetorparametros.append(" ");
                    numparam = numparam +1;
                }

            }

            //add caso seja o ultimo parametro
            else{
                std::stringstream pegarparamen(vetorparametros);

                if (parametro.empty()) {
                    numparam = 0;
                    break;
                }
                erroparametro = confereparametro(parametro, lugar);
                if (erro == 0){
                    erro = erroparametro;
                }
                if (erroparametro == 0) {
                    for (n = 0; n < numparam; n++) {
                        pegarparamen >> variavel;
                        if (parametro == variavel) {
                            std::cout << "Erro semantico, parametro da macro repetido. Linha: " << lugar << std::endl;
                            repetido = 1;
                        }
                    }
                    if (numparam > 2) {
                        std::cout << "Erro sintático mais de 3 argumentos. Linha: " << lugar << std::endl;
                        repetido = 1;
                    }
                }
                if (repetido == 0 && erroparametro == 0){
                    vetorparametros.append(parametro);
                    numparam = numparam + 1;
                }
            }

        }while(flag == 0);

        if(erro == 0 && repetido == 1){
            erro = 1;
        }

        if (detectaerro == 0 ){
            quantmacros = quantmacros + 1;
            auxiliar = quantmacros;
            corpomacro(token1, vetorparametros, numparam, arquivo, tmacros, auxiliar, quantmacros, arquivosaida, erro, lugar);
        }

    }
    else{
    }
}

//Lê linha do arquivo e chama a funcao conferemacro
void expandirmacro(std::ifstream &arquivo, std::ofstream &arquivosaida){
    std::string linha, testeparada;
    std::string nomeArquivo;
    int quantdemacros, lugar;
    lugar = 0;

    std::vector<tabelamacro> tmacros;

    //armazena espaço para um elemento no vector de struct é necessário sempre pra colocar um novo elemento
    tmacros.push_back(tabelamacro());

    //quant de macros encontradas
    quantdemacros = 0;
    //ler do arquivo parte de definição de macros
    while(!arquivo.eof()){ //testa se o tem algo para ler
        getline(arquivo,linha); //lê uma linha do arquivo e armazena na variavel linha
        lugar = lugar + 1;
        std::stringstream definicao(linha);
        definicao >> testeparada;
        definicao >> testeparada;
        if((testeparada != "MACRO" || linha == "SECTION DATA") && linha != "SECTION TEXT"  ){
            break;
        }
        if(linha == "SECTION TEXT"){
            arquivosaida << "SECTION TEXT" << std::endl;
        }
        else{
            conferemacro(linha, quantdemacros, tmacros, arquivo, arquivosaida, lugar); //função de conferir o rotulo da macro
        }
    }
    //ler do arquivo a parte de section text
    while(!arquivo.eof()){

        if (linha == "SECTION DATA"){
            break;
        }
        //percorre código atrás da chamada da macro
        chamamacro(linha, quantdemacros, tmacros, arquivosaida, lugar);
        getline(arquivo,linha);
        lugar = lugar + 1;
    }
    //ler do arquivo a parte de section data
    arquivosaida << "SECTION DATA" << std::endl;
    while(!arquivo.eof()) {
        getline(arquivo,linha); //lê uma linha do arquivo e armazena na variavel linha
        lugar = lugar + 1;
        arquivosaida << linha << std::endl;
    }

}