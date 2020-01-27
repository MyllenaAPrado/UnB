
struct Rotulo
{
    std::string nomeLabel;
    std::string  valorEQU;
    Rotulo (){};
    Rotulo(std::string nome, std::string equ): nomeLabel(nome), valorEQU(equ){};
};

int processaarquivo(std::ifstream &arquivo, std::ofstream &arquivosaida)
{   std::string arq_entr, arq_frase, linha, palavra, frase, linha_aux, Label, SegundoToken, ValorEQU, IF, descarte, novoToken, pre, novaFrase;
    char caracter;
    char *p;
    int tam, i, tamp, tam_frase, novo_tam_frase, j, pos, achou, verificador, contLinha, EquDef, tam_Rotulo, inicio, k, tamTabRotulo, encontrouEQU, encontrouIF;
    //unsigned int inicioRotulo;
    int inicioRotulo;
    std::vector<Rotulo> TabRotulo; // ONDE VOU GUARDAR AS LABELS QUE DEVEM SER SUBSTITUIDAS

    arq_entr = "arquivo11.txt";
    arq_frase = "frase.txt";
    contLinha = 0;
    //pre.append("\n");
   // std::ifstream arquivo(arq_entr);
    std::ofstream arquivoFrase(arq_frase);
    if(!arquivo){
        std::cout <<"Erro ao criar arquivo"<<std::endl;
    }
    TabRotulo.push_back(Rotulo());
    while(!arquivo.eof())
    {
        getline(arquivo, linha_aux);

        contLinha = contLinha + 1;
        achou = linha_aux.find(';');
        if(achou != -1)
        {
            linha = linha_aux.substr(0,achou);
        }
        else
        {
            linha = linha_aux;
        }

        std::transform (linha.begin(), linha.end(), linha.begin(), ::toupper); //transforma todos os caracteres em maisculo
        std::stringstream estrutura(linha); //pega a palavra até o espaco
        tam = 0;
        for (i = 0; i < linha.length(); i++) //PARA ELIMINAR OS ESPACOS EXTRAS NO ARQUIVO
        {
            if(!isspace(linha[i]))
            {
                tam++;
            }
        }

        while(tam > 0)
        {
            estrutura >> palavra;
            tamp = palavra.length();
            tam = tam - tamp;
            frase.append(palavra);
            frase.append(" ");
        }
        tam_frase = frase.length(); //vê o tamanho de uma linha do programa

        frase.at(tam_frase-1) = '\n';


    }
    arquivoFrase << frase;
    arquivoFrase.close();
    arquivo.close();
    std::ifstream arquivoNovo(arq_frase);
    while(!arquivoNovo.eof())
    {
        //ANALISE DE DIRETIVAS
        getline(arquivoNovo, novaFrase);
        if(novaFrase.empty())
        {
            break;
        }
        std::stringstream pegaToken (novaFrase);
        pegaToken >> Label;
        if(Label.back() == ':')
        {
            pegaToken >> SegundoToken;
            if(SegundoToken == "EQU")
            {
                Label.pop_back();
                pegaToken >> ValorEQU;
                pegaToken >> novoToken;
                if(ValorEQU.empty())
                {
                    std::cout << "ERRO SINTATICO! FALTA ARGUMENTO NA DIRETIVA EQU, LINHA: " << contLinha << std::endl;
                }
                if(!novoToken.empty())
                {
                    std::cout << "ERRO SINTATICO! EQU SO TEM UM ARGUMENTO, LINHA: " << contLinha << std::endl;
                }
                for(i = 0; i < TabRotulo.size(); i++)
                {
                    if(Label == TabRotulo[i].nomeLabel)
                    {
                        std::cout << "ERRO SEMANTICO! JA DEFINIDA COM  EQU, LINHA: " << contLinha << std::endl;
                    }
                }
                Rotulo adicionar (Label, ValorEQU); 
                TabRotulo.push_back(adicionar);
                
            }
        }

        if(Label == "IF")
        {
            pegaToken >> IF;
            if(IF.empty())
            {
                //ERRO, IF NAO TEM ARGUMENTO
                std::cout << "ERRO SINTATICO! IF NECESSITA DE ARGUMENTO NA LINHA: " << contLinha << std::endl;
            }
            else
            {
                pegaToken >> novoToken;
                if(!novoToken.empty())
                {
                    //ERRO SINTATICO, NAO DEVE TER OUTRO ARGUMENTO O IF
                    std::cout << "ERRO SINTATICO! LINHA: " << contLinha << std::endl;
                }
                else
                {
                    //SUBSTITUIR EQU NO PROGRAMA
                    EquDef = -1;
                    tamTabRotulo = TabRotulo.size();
                    for(i = 0; i < tamTabRotulo; i++)
                    {   
                        if(IF == TabRotulo[i].nomeLabel)
                        {
                            EquDef = i;
                            break;
                        }
                    }
                    if(EquDef == -1)
                    {
                        std::cout << "ERRO SEMANTICO! NAO DEFINIDO, LINHA: " << contLinha << std::endl;
                    }
                    else
                    {
                        //verificar o valor antes de fazer isso
                        verificador = 0;
                        verificador = strtol(TabRotulo[EquDef].valorEQU.c_str(), &p , 10); //TRANSFORMA EM INTEIRO
                        if (verificador != 1)
                        {
		                    getline(arquivoNovo, descarte); //pega a proxima linha e joga fora
                            contLinha = contLinha + 1;
	                    }
                    }
                }   
            }    
        }
        //EXPANDE EQU PARA OUTROS CASOS QUE NAO SEJA IF

        for(i = 0; i < TabRotulo.size(); i++)
        {
            novo_tam_frase = novaFrase.length();
            tam_Rotulo = TabRotulo[i].nomeLabel.size();
            inicio = 0;
            inicioRotulo = novaFrase.find(TabRotulo[i].nomeLabel, inicio);

            while(inicioRotulo != std::string::npos)
                {
                //O IF MAPEIA O ROTULO QUE QUEREMOS SUBSTITUIR INDICANDO O SEU TAMANHO
                if(novaFrase[tam_Rotulo+inicioRotulo] == ' ' || novaFrase[tam_Rotulo+inicioRotulo] == '\n' || inicioRotulo+tam_Rotulo >= novo_tam_frase) //VERIFICA SE O ROTULO ESTA NO MEIO OU NO FINAL DE UMA LINHA
                {
                    novaFrase.replace(inicioRotulo, tam_Rotulo, TabRotulo[i].valorEQU);
                }
                inicio = inicio + 1; // reduzir a busca em frase
                inicioRotulo = novaFrase.find(TabRotulo[i].nomeLabel, inicio);
            }
        }

        if(novaFrase != descarte)
        {
            encontrouIF = novaFrase.find("IF");
            encontrouEQU = novaFrase.find("EQU");
            if((encontrouIF == -1) && (encontrouEQU == -1))
            {
                pre.append(novaFrase);
                tam_frase = pre.length();
                pre.append("\n");
            }
        }
        //std::cout<<"pre"<<std::endl;
        //std::cout << pre<<std::endl;
    }
    //pos = pre.find("SECTION DATA");
   /* if(pos == -1)
    {
        std::cout << "SECTION DATA nao encontrada" << std::endl;
    }
    else
    {
        std::cout << "SECTION DATA OK" << std::endl;
    }
    
    pos = pre.find("SECTION TEXT");
    if(pos == -1)
    {
        std::cout << "SECTION TEXT nao encontrada" << std::endl;
    }
    else
    {
        std::cout << "SECTION TEXT OK" << std::endl;
    }*/

    arquivosaida << pre << std::endl;

    return 0;
}
