import time
import threading
import socket
import pickle
import thread


clientName = '127.0.0.1'
clientPort = 6001
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
clientSocket.connect((clientName, clientPort))


def postar_usuarios(): #postar usuarios da sala
    print("Participantes:")
    user = clientSocket.recv(1024) #recebe lista de usuarios
    usua = pickle.loads(user)
    print(usua)#posta lista usuarios

def postar_salas(lista): # posta sala e participantes
    print("Salas, participantes:")
    for i in range (0, len(lista), 2): #percorre a lista de dois em dois elementos
        print (lista[i], lista[i+1]) # posta sala e numero de participante

def funcoes(opt): #funcao das opcoes do chat

    global sala_desejada
    global estar
    global sair

    if opt == '1': # opcao buscar informacoes sobre salas

        print("Digite 1 para saber as salas existente e o numero de participante\n"
              "Digite 2 para saber os usuarios de sua sala atual")

        flag = raw_input("opcao:") # escolhe entre ver as salas e os usuarios da sala
        time.sleep(0.2)
        clientSocket.send(flag) # envia essa opcao pro servidor

        if flag == '1': # se for pra ver as salas e os participantes
            salotas = clientSocket.recv(1024) #recebe a lista do servidor
            lista = pickle.loads(salotas)
            if not lista:
                print("Sem salas")
            else:
                print 'Salas publicas:'
                postar_salas(lista) # chama funcao que posta as salas

        if flag == '2': #escolhe ver os usuarios da sala atual
            if estar == 1 :
                clientSocket.send(sala_desejada) #envia sala atual
                postar_usuarios() #posta os usuarios
            else:
                clientSocket.send('0')
                print("voce nao esta em uma sala ou nao ha mais participantes na atual")

    if opt == '2': #entrar ou sair de uma sala publica

        print("Digite 1 para entrar em uma sala e 2 para sair de uma sala:")
        digitado = raw_input("opcao:")
        time.sleep(0.2)
        clientSocket.send(digitado)

        if digitado == '1': #usuario deseja entrar em uma sala
            r = str(estar)
            clientSocket.send(r)
            time.sleep(0.2)
            if estar == 1: #testa se o usuario ja esta em uma lista
                print("Voce ja esta em uma sala, opcao nao disponivel!")

            else:
                data = clientSocket.recv(1024) #recebe lista de salas
                salas = pickle.loads(data) # processa o dado recebido
                postar_salas(salas) # manda pra funcao informar ao usuario as salas e os participantes
                print("Digite o nome da sala desejada:")
                sala_desejada = raw_input() #escolhe sala
                clientSocket.send(sala_desejada) # manda para o servidor
                print("Bem vindo a sala: ")
                print(sala_desejada)
                estar = 1


        if digitado == '2': # se o usuario quer sair de uma sala
            r = str(estar)
            time.sleep(0.5)
            clientSocket.send(r) #envia se o usuario encontra em uma sala
            time.sleep(0.5)
            if estar == 1: # se o usuario esta em uma sala
                clientSocket.send(sala_desejada) #nome da sala para o servidor
                print ("Voce saiu da sala")
                estar = 0
            else:
                print ("voce nao esta em uma sala, entre em uma antes de sair dela")


    if opt == '3': # cria uma sala publica
        print("Digite um nome para a sala:")
        while True:
            sala = raw_input("Sala:")
            clientSocket.send(sala) #envia nome da sala
            time.sleep(0.5)
            f = clientSocket.recv(1024) #recebe se o nome foi aceito ou nao
            if f == "0":
                break
            print("Nome ja existente. Escolha outro.")

    if opt == '4':
        data = clientSocket.recv(1024)  # recebe a lista do servidor
        lista = pickle.loads(data)
        postar_salas(lista)  # chama funcao que posta as salas
        print("Apenas salas sem participantes podem ser deletadas, escolha uma acima vazia:")
        sala = raw_input("sala:")
        clientSocket.send(sala) #envia o nome da sala a ser deletada
        rt = clientSocket.recv(1024)
        if rt == '1':
            print("Sala deletada")

        else:
            print("Sala com participantes nao pode ser deletada")


    if opt == '5': #enivar mensagem pro grupo
        clientSocket.send(sala_desejada) #eniva sala atual
        print("Digite a mensagem a ser enviada:")
        ms = raw_input()
        clientSocket.send(ms) #envia mensagem


def menu(): #opcao de menu em segunda chamada

    print ("\n ---------------------------------------------------------------------"
           "\nEscolha o numero de uma opcao abaixo:\n"
           "1- Buscar informacoes sobre as salas\n"
           "2- Entrar em e sair de uma sala existente\n"
           "3- Criacao de sala publicas\n"
           "4- Apagar salas \n"
           "5- Envio de arquivos e de mensagens de texto\n")

    while True: #testa valida da opcao
        opcao = raw_input("Opcao:") #reconhece opcao do menu
        if opcao == '1' or opcao == '2' or opcao == '3' or opcao == '4' or opcao == '5':
            break
        else:
            print("Opcao nao exitente, escolha entre as opcoes acima")
    time.sleep(0.1)
    clientSocket.send(opcao) #envia opcao pro servidor
    funcoes(opcao) #chama funcao que executa as opcoes do chat

def usuario():
    while True:
        menu()


print ("Bem vindo! Para se cadastrar no chat escolha um nome") #frase inicial
clientSocket.send("0") #escolha de cadastramento automatica

while True: #testar nome de usuario
    userName = raw_input('Nome: ') #le o nickname
    clientSocket.send(userName) #envia para servidor
    msg = clientSocket.recv(1024) # recebe a respota se foi aceito ou nao
    if msg == '0': # nome aceito
        break
    if msg == '1': #nome invalido
        print "Nome invalido, tente outro."

print ("\n ---------------------------------------------------------------------"
               "\nEscolha o numero de uma opcao abaixo:\n"
               "1- Buscar informacoes sobre as salas\n"
               "2- Entrar em e sair de uma sala existente\n"
               "3- Criacao de sala publicas\n"
               "4- Apagar salas \n")

sala_desejada = "11111111111111111111111111111" #primeiro nome de sala para o cliente comparar
estar = 0
sair = 0

while True:  # testa valida da opcao
    opcao = raw_input("Opcao:")  # reconhece opcao do menu
    if opcao == '1' or opcao == '2' or opcao == '3' or opcao == '4':
        break
    else:
        print("Opcao nao exitente, escolha entre as opcoes acima")

clientSocket.send(opcao)  # envia opcao pro servidor
funcoes(opcao)  # chama funcao que executa as opcoes do chat



def grupo(): #thread para receber mensagem do grupo
    global sala_desejada

    while True:
        data = clientSocket.recv(1024) #fica escutando
        if data == sala_desejada : #verifica se a mensagem e da sala
            mens = clientSocket.recv(1024) #recebe a mensagem
            print (mens)

client_threading = threading.Thread(target=usuario) #thread do usuario
client_threading.start()

receive_threading = threading.Thread(target=grupo)#thread para escutar as mensagens
receive_threading.start()



while True:
    if sair == 3:
        clientSocket.close()
        clientSocket.close()
        break







