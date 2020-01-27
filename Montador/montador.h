
struct Rotulos{
    std::string rotulo;
    int numdalinha;
    std::string tipo;
    int valorconst;
    int valorspace;
    Rotulos (){};
    Rotulos (std::string nome, int linhas, int tamanho, std::string diretiva, int constante): rotulo(nome), numdalinha(linhas), valorspace(tamanho), tipo(diretiva), valorconst(constante){};
};


void SegundaPassada(std::vector<Rotulos> &tabelasimbolos, std::ifstream &arquivomontador, std::ofstream &arquivosaida){
    std::string arq_entr, linha, token, arg1, arg2, arg3, tokenNaoLabel, stop;
    std::string Instrucoes[] = {"ADD", "SUB", "MULT", "DIV", "JMP", "JMPN", "JMPP", "JMPZ", "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP"};
    std::string Diretivas[] = {"CONST", "SPACE"};
    int opcode[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    int numArgumento[] = {1,1,1,1,1,1,1,1,2,1,1,1,1,0};
    int diretiva, instrucao, i, j, k, constante, ConstInt, espaco, quantEspaco, rotDefinido, lugar;
    unsigned int virgula;
    int data;
    char *p, *c, *b, *d; // pra ajudar a transformar de char para inteiro
    std::vector<int> obj; //VETOR COM OS OPCODES E ENDERECOS PARA O ARQUIVO OBJETO

    //arq_entr = "arquivo11.txt";
    data = 0;
    lugar = 0;
    //getline(arquivomontador, linha);

    //std::ifstream arquivo(arq_entr);
    while(!arquivomontador.eof()) {
        getline(arquivomontador, linha);
        if (linha == "SECTION TEXT"){
            getline(arquivomontador, linha);
            lugar = lugar + 1;
        }
        lugar = lugar + 1;
        if (linha.empty()) {
            break;
        }
        std::stringstream pegaToken(linha);
        pegaToken >> token;
        if (token.back() == ':') //VERIFICA SE O PRIMEIRO TOKEN PEGO EH LABEL, SE FOR DEVE-SE PEGAR O PROXIMO PARA VER SE EH DIRETIVA
        {
            pegaToken >> tokenNaoLabel;
            token = tokenNaoLabel;
        }
        diretiva = -1;
        instrucao = -1;
        for (j = 0; j < 2; j++) {
            if (token == Diretivas[j]) {
                diretiva = j;
                break;
            }
        }
        for (i = 0; i < 14; i++) {
            if (token == Instrucoes[i]) {
                instrucao = i;
                break;
            }
        }

        //ANALISE DE DIRETIVAS
        if (diretiva != -1) {
            if (token == "CONST") {
                pegaToken >> arg1;
                if (arg1.empty()) {
                    //ERRO!!!
                    std::cout << "ERRO SINTATICO! FALTA DE ARGUMENTO NA LINHA " << lugar << std::endl;
                } else {
                    if ((arg1.substr(0,2) == "0X") || (arg1.substr(0,3) == "-0X")) {
                        ConstInt = strtol(arg1.c_str(), &p, 16); //CASO HEXADECIMAL
                    } else {
                        ConstInt = strtol(arg1.c_str(), &p, 10); //CASO DECIMAL
                    }
                    arg2.clear();
                    pegaToken >> arg2;
                    //std::cout<<"ARG2 RECEBIDO DO SUB: "<< arg2<< std::endl;
                    if (!arg2.empty()) {
                        std::cout << "ERRO SINTATICO! INTRUCAO ACEITA SOMENTE UM ARGUMENTO, LINHA: " << lugar<< std::endl;
                       // std::cout <<"PASSEI AQUI"<<std::endl;
                    } else {
                        obj.push_back(ConstInt);
                        arquivosaida << ConstInt << " ";
                    }
                }
            } else {
                if (token == "SPACE") {
                    espaco = 0;
                    arg1.clear();
                    pegaToken >> arg2;
                    if (arg2.empty()) {
                        espaco = 1;
                    } else {

                        quantEspaco = strtol(arg2.c_str(), &c, 10);
                        if (!*c || (*c == 1 && quantEspaco > 1))
                            espaco = quantEspaco + 1;
                        else {
                            //ERRO SINTATICO
                            std::cout << "ERRO SINTATICO NA LINHA: " << lugar << std::endl;
                        }
                    }
                    k = 0;
                    while (k < espaco) {
                        obj.push_back(0);
                        arquivosaida << 0 << " ";
                        k++;
                    }

                }
            }
        }
        
        //ANALISE DE INSTRUCOES
        if (instrucao != -1) {
            obj.push_back(opcode[i]);
            arquivosaida << opcode[i] << " ";
            if (token == "STOP") {
                pegaToken >> stop;
                if (!stop.empty()) {
                    //erro, stop nao tem argumento
                    std::cout << "ERRO SINTATICO! STOP NAO TEM ARGUMENTO! LINHA: " << lugar << std::endl;
                }
            } else {
                if (token == "COPY") {
                    arg1.clear();
                    pegaToken >> arg1;
                    if (arg1.empty()) {
                        //ERRO SINTATICO!! PRECISA DE DOIS ARGUMENTOS
                        std::cout << "ERRO SINTATICO! INTRUCAO PRECISA DE ARGUMENTOS NA LINHA: " << lugar << std::endl;
                    } else {
                        arg2.clear();
                        pegaToken >> arg2;
                        if (arg2.empty()) {
                            //ERRO SINTATICO!! PRECISA DE DOIS ARGUMENTOS
                            std::cout << "ERRO SINTATICO! INTRUCAO PRECISA DE ARGUMENTOS NA LINHA: " << lugar<< std::endl;
                        } else {
                            virgula = 0;
                            virgula = arg1.find(',');
                            if (virgula == 0) {
                                //ERRO LEXICO !! TEM QUE TER VIRGULA
                                std::cout << "ERRO LEXICO! DEVE TER VIRGULA NO FINAL DO PRIMEIRO ARGUMENTO, LINHA: "
                                          << lugar << std::endl;
                            } else {
                                if (arg1.length() > 50) {
                                    //ERRO LEXICO! MUITO GRANDE
                                    std::cout << "ERRO LEXICO! ARGUMENTO MUITO GRANDE, LINHA: " << lugar << std::endl;
                                } else {
                                    arg1 = arg1.substr(0, virgula);
                                    if (arg1.empty()) {
                                        //ERRO SINTATICO
                                        std::cout << "ERRO SINTATICO! INTRUCAO SEM ARGUMENTO NA LINHA: " << lugar
                                                  << std::endl;
                                    } else {
                                        //LUGAR PARA TESTAR VETOR
                                        if(arg2 == "+")
                                        {
                                            rotDefinido = -1;
                                            for (k = 1; k < tabelasimbolos.size(); k++) {
                                                if (arg1 == tabelasimbolos[k].rotulo) {
                                                    rotDefinido = k;
                                                    break;
                                                }
                                            }
                                            if (rotDefinido == -1) {
                                                if (arg1.front() <= '9' && arg1.front() >= '0') {
                                                    //ERRO LEXICO
                                                    std::cout << "ERRO LEXICO! FORMATO INVALIDO NA LINHA: " << lugar
                                                          << std::endl;
                                                } else {
                                                    //ERRO ROTULO NAO DEFINIDO
                                                    std::cout << "ERRO SEMANTICO! ROTULO NAO DEFINIDO NA LINHA: " << lugar
                                                          << std::endl;
                                                }
                                            } else {
                                                obj.push_back(tabelasimbolos[rotDefinido].numdalinha);
                                                arquivosaida << tabelasimbolos[rotDefinido].numdalinha << " ";
                                                pegaToken >> arg3; //NUMERO DO VETOR
                                                if(arg3.empty())
                                                {
                                                    std::cout<<"ERRO SINTATICO! A INSTRUCAO PRECISA DE MAIS UM ARGUMENTO, LINHA: " << lugar<<std::endl;
                                                }
                                                else
                                                {
                                                    int numIndice;
                                                    numIndice = strtol(arg3.c_str(), &b, 10);
                                                    if(/*!*b || */(*b == 1 && numIndice < 1))
                                                    {
                                                        std::cout<<"ERRO SINTATICO! ARGUMENTO INVALIDO NA LINHA: "<< lugar <<std::endl;
                                                    }
                                                    else
                                                    {
                                                        if(tabelasimbolos[rotDefinido].valorspace == 0)
                                                        {
                                                            std::cout<<"ERRO SEMANTICO! ROTULO NAO DEFINIDO COMO VETOR NA LINHA: " << lugar<< std::endl;
                                                        }
                                                        else
                                                        {
                                                            if(tabelasimbolos[rotDefinido].valorspace-1 < numIndice && tabelasimbolos[rotDefinido].valorspace > 0)
                                                            {
                                                                std::cout<< "ERRO SEMANTICO! TAMANHO EXCEDIDO, LINHA: "<<lugar<<std::endl;
                                                            }
                                                            else
                                                            {
                                                                obj.push_back(tabelasimbolos[rotDefinido].numdalinha);
                                                                arquivosaida<<tabelasimbolos[rotDefinido].numdalinha<<" ";
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else{
                        
                                            rotDefinido = -1;
                                            for (k = 1; k < tabelasimbolos.size(); k++) {
                                                if (arg1 == tabelasimbolos[k].rotulo) {
                                                    rotDefinido = k;
                                                    break;
                                                }
                                            }
                                            if (rotDefinido == -1) {
                                                if (arg1.front() <= '9' && arg1.front() >= '0') {
                                                    //ERRO LEXICO
                                                    std::cout << "ERRO LEXICO! FORMATO INVALIDO NA LINHA: " << lugar
                                                          << std::endl;
                                                } else {
                                                    //ERRO ROTULO NAO DEFINIDO
                                                    std::cout << "ERRO SEMANTICO! ROTULO NAO DEFINIDO NA LINHA: " << lugar
                                                          << std::endl;
                                                }
                                            } else {
                                                obj.push_back(tabelasimbolos[rotDefinido].numdalinha);
                                                arquivosaida << tabelasimbolos[rotDefinido].numdalinha << " ";

                                                //ANALISE PARA O SEGUNDO ARGUMENTO
                                                rotDefinido = -1;
                                                for (k = 1; k < tabelasimbolos.size(); k++) {
                                                    if (arg2 == tabelasimbolos[k].rotulo) {
                                                        rotDefinido = k;
                                                        break;
                                                    }
                                                }
                                                if (rotDefinido == -1) {
                                                    if (arg2.front() <= '9' && arg2.front() >= '0') {
                                                        //ERRO LEXICO
                                                        std::cout << "ERRO LEXICO! FORMATO INVALIDO NA LINHA: " << lugar<< std::endl;
                                                    } else {
                                                        //ERRO ROTULO NAO DEFINIDO
                                                        std::cout << "ERRO SEMANTICO! ROTULO NAO DEFINIDO NA LINHA: "<< lugar << std::endl;
                                                    }
                                                } else {
                                                    obj.push_back(tabelasimbolos[rotDefinido].numdalinha);
                                                    arquivosaida << tabelasimbolos[rotDefinido].numdalinha << " ";
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    arg1.clear();
                    pegaToken >> arg1;
                    //std::cout<<"ARG1 RECEBIDO DO SUB: "<< arg1<< std::endl;
                    if (arg1.empty()) {
                        //ERRO SINTATICO!!
                        std::cout << "ERRO SINTATICO! NAO FOI APRESENTADO ARGUMENTO NA LINHA: " << lugar << std::endl;
                    } else {
                        arg2.clear();
                        pegaToken >> arg2;
                      //  std::cout<<"ARG2 RECEBIDO DO SUB: "<< arg2<< std::endl;
                        if (!arg2.empty()) {
                            if(arg2 == "+")
                            {
                                rotDefinido = -1;
                                for (k = 1; k < tabelasimbolos.size(); k++) {
                                    if (arg1 == tabelasimbolos[k].rotulo) {
                                        rotDefinido = k;
                                        break;
                                    }
                                }
                                if (rotDefinido == -1) {
                                        if (arg1.front() <= '9' && arg1.front() >= '0') {
                                        //ERRO LEXICO
                                        std::cout << "ERRO LEXICO! FORMATO INVALIDO NA LINHA: " << lugar<< std::endl;
                                        } else {
                                        //ERRO ROTULO NAO DEFINIDO
                                        std::cout << "ERRO SEMANTICO! ROTULO NAO DEFINIDO NA LINHA: " << lugar
                                              << std::endl;
                                    }
                                } else {
                                    obj.push_back(tabelasimbolos[rotDefinido].numdalinha);
                                    arquivosaida << tabelasimbolos[rotDefinido].numdalinha << " ";
                                    arg3.clear();
                                    pegaToken >> arg3; //NUMERO DO VETOR
                                    if(arg3.empty())
                                    {
                                        std::cout<<"ERRO SINTATICO! A INSTRUCAO PRECISA DE MAIS UM ARGUMENTO, LINHA: " << lugar<<std::endl;
                                    }
                                    else
                                    {
                                        int numIndice;
                                        numIndice = strtol(arg3.c_str(), &d, 10);
                                       // std::cout<< "numIndice: "<< numIndice<< std::endl;
                                        //std::cout<<"*d: " << *d<<std::endl;
                                        if(/*!*d || */(*d == 1 && numIndice < 1))
                                        {
                                            std::cout<<"ERRO SINTATICO! ARGUMENTO INVALIDO NA LINHA: "<< lugar <<std::endl;
                                        }
                                        else
                                        {
                                            if(tabelasimbolos[rotDefinido].valorspace == 0)
                                            {
                                                std::cout<<"ERRO SEMANTICO! ROTULO NAO DEFINIDO COMO VETOR NA LINHA: " << lugar<< std::endl;
                                            }
                                            else
                                            {
                                                if(tabelasimbolos[rotDefinido].valorspace-1 < numIndice && tabelasimbolos[rotDefinido].valorspace > 0)
                                                {
                                                    std::cout<< "ERRO SEMANTICO! TAMANHO EXCEDIDO, LINHA: "<<lugar<<std::endl;
                                                }
                                                else
                                                {
                                                    obj.push_back(tabelasimbolos[rotDefinido].numdalinha);
                                                    arquivosaida<<tabelasimbolos[rotDefinido].numdalinha<<" ";
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else{
                                //ERRO SINTATICO
                                std::cout << "ERRO SINTATICO! INSTRUCAO SO PRECISA DE UM ARGUMENTO NA LINHA:" << lugar<< std::endl;
                            }
                        } else {
                            virgula = -1;
                            virgula = arg1.find(',');
                            if (virgula != -1) {
                                //ERRO LEXICO ARG NAO DEVE TER VIRGULA
                                std::cout << "ERRO LEXICO! NAO DEVE TER VIRGULA JUNTO DO PRIMEIRO ARGUMENTO NA LINHA: "
                                          << lugar << std::endl;
                            } else {
                                rotDefinido = -1; // VERIFICAR SE O ARGUMENTO ESTA DEFINIDO NA TABELA DE SIMBOLOS
                                for (k = 1; k < tabelasimbolos.size(); k++) {
                                    if (arg1 == tabelasimbolos[k].rotulo) {
                                        rotDefinido = k;
                                    }
                                }
                                if (rotDefinido == -1) //CASO NAO ENCONTRE NA TABELA DE SIMBOLOS
                                {
                                    if (arg1.front() <= '9' && arg1.front() >= '0') {
                                        //ERRO LEXICO
                                        std::cout << "ERRO LEXICO! FORMATO INVALIDO NA LINHA: " << lugar << std::endl;
                                    } else {
                                        if (token == "JMP" || token == "JMPN" || token == "JMPP" || token == "JMPZ") {
                                            //ERRO SEMANTICO PULO para lugar inexistente
                                            std::cout << "ERRO SEMANTICO! PULO PARA LUGAR INEXISTENTE NA LINHA: "
                                                      << lugar << std::endl;
                                        } else {
                                            //ERRO SINTATICO???
                                            std::cout << "ERRO SEMANTICO! ROTULO INEXISTENTE! LINHA: " << lugar
                                                      << std::endl;
                                        }
                                    }
                                } else {
                                    if (token == "DIV") {
                                        //VERIFICAR SE HA DIVISAO POR ZERO
                                        if (tabelasimbolos[rotDefinido].valorconst == 0) {
                                            //ERRO SEMANTICO, DIVISAO POR ZERO
                                            std::cout << "ERRO SEMANTICO! DIVISAO POR ZERO NA LINHA: " << lugar
                                                      << std::endl;
                                        }
                                    } else {
                                        if (token == "INPUT" || token == "STORE") {
                                            //VERIFICAR SE O ARGUMENTO NAO EH CONSTANTE, ERRO SEMANTICO
                                            if (tabelasimbolos[rotDefinido].tipo == "CONST") {
                                                //ERRO SEMANTICO
                                                std::cout << "ERRO SEMANTICO! INPUT OU STORE DE CONSTANTANTE NA LINHA: "
                                                          << lugar << std::endl;
                                            }
                                        } else {
                                            if (token == "JMP" || token == "JMPN" || token == "JMPP" ||
                                                token == "JMPZ") {
                                                //VERIFICA SE NAO ESTA PULANDO PARA UMA CONSTANTE, ERRO SEMANTICO
                                                if (tabelasimbolos[rotDefinido].tipo == "CONST") {
                                                    //ERRO SEMANTICO
                                                    std::cout << "ERRO SEMANTICO! PULO PARA UMA CONSTANTE NA LINHA: "
                                                              << lugar << std::endl;
                                                }else{
                                                    if(tabelasimbolos[rotDefinido].tipo == "SPACE") {
                                                        //ERRO SEMANTICO
                                                        std::cout
                                                                << "ERRO SEMANTICO! PULO PARA UMA VARIAVEL SPACE NA LINHA: "
                                                                << lugar << std::endl;
                                                    }
                                                }

                                            } else {
                                                obj.push_back(tabelasimbolos[rotDefinido].numdalinha);
                                                arquivosaida << tabelasimbolos[rotDefinido].numdalinha << " ";
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            if ( linha == "SECTION DATA") {
                data = 1;
            }else {
                if (data == 1) {
                } else {
                    // ERRO INSTRUCAO INVALIDA
                    std::cout << "ERRO LEXICO! INSTRUCAO NAO DEFINIDA. NA LINHA: " << lugar << std::endl;
                }
            }
        }
    }
}

//executa const na primeira passagem
int const1(std::stringstream &pegapalavra, int &constante, int lugar){
    std::string palavra;
    int converter;
    char *c;

    pegapalavra >> palavra;
    //testa se tem valor
    if (palavra.empty()){
        std::cout << "Erro sintático. Valor da constante não identificado. Linha: " << lugar<< std::endl;
    }

    //testa se é decimal
    constante = strtol(palavra.c_str(), &c, 10);
    converter = *c;
    if (!converter || (converter == 1 && constante < 1)){
        return 0;
    }
    else{//testa se e hex positivo
        if(palavra.substr(0,2) == "0X"){
            constante = strtol(palavra.c_str(), &c, 16);
        }else{
            std::cout << "Erro lexico. Token nao permitido. Linha: " << lugar<< std::endl;
            return 1;
        }
        if (!converter || (converter == 1 && constante < 1)){
            std::cout << "Erro lexico. Token nao permitido. Linha: " << lugar<< std::endl;
            return 1;
        }
        else{
            return 0;

        }
    }
}

//executa space na primeira passagem
int space1(std::stringstream &pegapalavra, int lugar, int &reservado){
    std::string palavra;
    int converter, numero, erro;
    char *c;
    reservado = 0;
    erro = 0;
    //pega proxima palavra
    pegapalavra >> palavra;
    //space normal
    if (palavra.empty()){
        reservado = 1;
    }
    else {
        pegapalavra >> palavra;
        //confere se é numero
        numero = strtol(palavra.c_str(), &c, 10);
        converter = *c;
        if (!converter || (converter == 1 && numero < 1)) {
            reservado = 1 + numero;
        } else {
            std::cout << "Erro lexico, caracter nao valido. Linha: " << lugar << std::endl;
            erro = 1;
        }


    }
    return erro;
}

void primeirapassagem(std::ifstream &arquivomontador, std::vector<Rotulos> &tabelasimbolos, std::vector<int> &linhaerro){
    std::string linha, palavra, rotulo, diretiva;
    int section, errorot, quantrotulo, lugar, reservado, local, i, label, constante, v;

    section = 0; // 1 = Text  e  2 = Data

    lugar = 0;
    local = 0;
    quantrotulo = 0;
    i = 0;
    v = 0;

    while (!arquivomontador.eof()) {
        //lê do arquivo
        getline(arquivomontador, linha);
        if (linha.empty()){
            break;
        }
        lugar = lugar + 1;
        local = local + 1;
        if (linha == "SECTION TEXT") {
            section = 1;
        }
        if (linha == "SECTION DATA") {
            section = 2;
        }
        label = 0;
        //pega palavaras da linhas lida


        palavra.clear();
        std::stringstream pegapalavra(linha);
        pegapalavra >> palavra;
        //pegapalavra >> diretiva;

        //erro rotulo
        errorot = 0;
        constante = 0;
        //se encontrou rotulo
        if (palavra.back() == ':') {
            rotulo = palavra;

            //analisar se tem outro rotulo
            while (pegapalavra >> palavra) {
                //se achou rotulo
                if (palavra.back() == ':') {
                    errorot =1;
                    std::cout << "Erro sintatico, rotulo repetido. Linha: " << lugar << std::endl;
                }
            }
            std::stringstream pegapalavra(linha);
            pegapalavra >> palavra;
            pegapalavra >> diretiva;

            //se nao tem o rotulo
            rotulo.pop_back();
            if (rotulo.empty()) {
                errorot =1;
                std::cout << "Erro sintatico, rotulo ausente. Linha: " << lugar << std::endl;
            }
            else{

                //se o rotulo tem o nome de uma instrucao
                if(rotulo == "ADD" || rotulo == "SUB" || rotulo == "MULT" || rotulo == "DIV" || rotulo == "JMP" || rotulo == "JMPN" || rotulo == "JMPP" || rotulo == "JMPZ" || rotulo == "COPY" || rotulo == "LOAD" || rotulo == "STORE" || rotulo == "INPUT" || rotulo == "OUTPUT" || rotulo == "STOP"){
                    std::cout << "Erro semantico, rotulo com mesmo nome de instrucao. Linha: " << lugar << std::endl;
                    errorot =1;
                }
                //se o rotulo tem o nome de uma diretiva
                if(rotulo == "CONST" || rotulo == "SPACE" || rotulo == "SECTION" || rotulo == "EQU" || rotulo == "MACRO" || rotulo == "END"){
                    std::cout << "Erro semantico, rotulo com mesmo nome de diretiva. Linha: " << lugar << std::endl;
                    errorot =1;
                }

                //caracter invalido no rotulo
                for (int i = 0; i < rotulo.length(); i++){
                    if((rotulo.at(i) > '9' || rotulo.at(i) < '0') && (rotulo.at(i) != '_') && (rotulo.at(i) > 'Z' || rotulo.at(i) < 'A')){
                        std::cout << "Erro lexico, token nao permitido. Linha: " << lugar << std::endl;
                        errorot =1;
                    }
                }

                //se o rotulo comeca com numero
                if(rotulo.front() <= '9' && rotulo.front() >= '0' ){
                    std::cout << "Erro lexico, rotulo começa com número. Linha: " << lugar << std::endl;
                    errorot =1;
                }

                //se o rotulo tem tamanho maior que 50
                if(rotulo.length() > 50){
                    std::cout << "Erro lexico, rotulo com tamanho maior que o permitido. Linha: " << lugar << std::endl;
                    errorot =1;
                }


                //se o rotulo é repetido
                for (int j = 0; j <= quantrotulo; ++j){
                    if (rotulo == tabelasimbolos[j].rotulo){
                        errorot = 2;
                    }
                    if (errorot == 2){
                        std::cout << "Erro semantico, rotulo repetido. Linha: " << lugar << std::endl;
                        errorot = 1;
                    }
                }


                //conferir aqui se é space ou const
                std::stringstream pegapalavra(linha);
                pegapalavra >> palavra;
                pegapalavra >> palavra;

                //confere se é space
                if (diretiva == "CONST"){
                    if (errorot == 0) {
                        errorot = const1(pegapalavra, constante, lugar);
                    }
                }else {
                    if (diretiva == "SPACE") {
                        if (errorot == 0){
                            errorot = space1(pegapalavra, lugar, reservado);
                        }
                    }else{
                        diretiva = "label";
                        label = 1;
                    }
                }

                if (label == 0 && (diretiva == "CONST" || diretiva == "SPACE") && section == 1 ){
                    errorot = 1;
                    std::cout << "Erro semantico, dado fora da SECTION DATA. Linha: " << lugar << std::endl;

                }
                //add rotulo na tabela de simbolos
                if (errorot == 0){
                    if (label == 0 && (diretiva == "CONST" || diretiva == "SPACE") ){
                        local = local + tabelasimbolos[i].valorspace - 2 ;
                    }else{
                        if (section == 2){
                            std::cout << "Erro semantico, label fora da SECTION TEXT. Linha: " << lugar << std::endl;
                        }
                    }

                    Rotulos adiciona(rotulo, local, reservado, diretiva, constante);
                    tabelasimbolos.push_back(adiciona);
                    quantrotulo = quantrotulo + 1;
                    i = i + 1;
                }
            }
            //std::cout << tabelasimbolos[i].rotulo << std::endl;
            //std::cout << tabelasimbolos[i].tipo << std::endl;
            //std::cout << tabelasimbolos[i].valorspace << std::endl;
            //std::cout << tabelasimbolos[i].numdalinha << std::endl;

        }
        //CASO NAO É ROTULO
        else{
            //confere se é stop
            if (palavra == "STOP"){
                reservado = 0;
                diretiva = "STOP";
                reservado = 2;
                Rotulos adiciona(palavra, local, reservado, diretiva, constante);
                tabelasimbolos.push_back(adiciona);
                quantrotulo = quantrotulo + 1;
                i = i + 1;

            }
            if(section == 2 && palavra != "STOP" && palavra!= "SECTION"){
                std::cout << "Erro semantico, instrucao fora da SECTION TEXT" << lugar << std::endl;
                errorot = 1;
            }

            //conferir secao
        }

        if (errorot == 1){
            linhaerro[v] = lugar;
            linhaerro.push_back(lugar);
            v++;
        }
    }
}

void montar(std::ifstream &arquivomontador, std::string codigo, std::ofstream &arquivosaida){
    int lugar, i;
    lugar = 0;
    std::vector<int> linhaerro;
    linhaerro.push_back(lugar);

    //tabela de simbolos
    std::vector<Rotulos> tabelasimbolos;
    tabelasimbolos.push_back(Rotulos());;
    primeirapassagem(arquivomontador, tabelasimbolos, linhaerro);

    std::ifstream arquivomontador1(codigo); //modo leitura
    if(!arquivomontador1){
        std::cout << "Ocorreu um erro na abertura do arquivo" << std::endl;
    }
    SegundaPassada(tabelasimbolos, arquivomontador1, arquivosaida);

}
