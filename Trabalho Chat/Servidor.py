import time
import thread
import socket
import pickle
from datetime import datetime

serverName = '127.0.0.1'
serverPort = 6001
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serverSocket.bind((serverName,serverPort))
serverSocket.listen(5)
cadastros={}
salas=[]
sala_participantes=[]
sala_user=[]
enviar = []

def log(logmsg,client): #funcao para fazer o arquvio log
    with open("log.txt","a") as log:
        log.write(str(datetime.now())+' ('+str(client)+'): '+logmsg+"\n") #armazena data, socket, requisicao e resposta nessa ordem

def envia_lista_usuarios(name, conn): # envia nomes do usuario da sala
    global enviar

    for i in range (0,len(sala_participantes),2): #percorre a lista de salas para saber quem esta na sala
        if sala_participantes[i] == name: # se pertence a sala
            data = cadastros[sala_participantes[i+1]] #Utiliza o dicionario para associar o socket ao nome usuario
            enviar.append(data)
    listaa = pickle.dumps(enviar)
    conn.send(listaa) #envia uma lista com os usuarios
    del enviar[:]

def envia_grupo(name, end, data): #manda mensagem para o grupo
    global sala_participantes

    for y in range (0, len(sala_participantes), 2):
        if sala_participantes[y] == name and sala_participantes[y+1]!=end : #verifica se o participante ta na sala e nao e a pessoa que ta fazendo a requisicao
            sala_participantes[y+1].send(name)
            time.sleep(1)
            sala_participantes[y + 1].send(data)


def modifica_salas(name, end, data): #modifica numero de usuarios
    global salas
    global sala_participantes
    i = 0
    sala_participantes.append(name) #add usuario
    sala_participantes.append(end) #add socket
    envia_grupo(name, end, data) #manda msg pro grupo que entrou alguem
    while True:
        if salas[i] == name: #procura o item na lista correspondente a sala
            salas [i+1] = salas[i+1] + 1 #aumenta um usuario
            break


def teste_sala(nome, end): #testa o nome da sala e adiciona os participantes
    global sala_participantes
    global salas
    flag = nome in salas #testa se ja tem sala com esse nome
    if flag == True: #ja tem o nome
        return '1'
    salas.append(nome) #add o nome da salae
    salas.append(0) # add um participante
    return '0'


def testa_nome(nome, end):
    flag = nome in cadastros # testa se ja existe o nome
    if flag == True: # caso ja tenha
        return '1'
    cadastros[end] = nome #associa nome ao endereco
    return '0'


def thread_cliente(conn, addr): #conexao thread
    global salas
    global sala_participantes

    print ' Conecction sucefful'
    while True:
        data = conn.recv(1024)  #dado da opcao do menu do cliente
        if not data: # se o cliente fechou a conexao
            break

        if data == "sair": #se o cliente deseja sair
            break

        if data == "0": #se o cliente acabou de entrar e vai se cadastrar

            while True: #enquanto nao aceita o nome
                menssagem = conn.recv(1024) # recebe nome usuario
                b = testa_nome(menssagem, conn) #testa a validade do nome
                conn.send(b) #envia o resultado do cadastro
                time.sleep(0.3)
                if b == '0':
                    log('Cadastro,  Aceito', conn)
                    #conn.send("funcoes")
                    break
                else:
                    log('Cadastro, Rejeitado', conn)

        if data == "1": #informacoes sobre as salas
            a = conn.recv(1024) # recebe flag de cliente se quer salas ou usuarios
            time.sleep(0.5)

            if a == "1": #informacao sobre sala e participante
                data = pickle.dumps(salas)
                conn.send(data) #envia lista de salas
                log('Informacao da sala, Lista das salas', conn)

            if a == "2": #informacoes sobre usuarios de uma sala
                name = conn.recv(1024) #recebe nome da sala desejada
                if name == '0':
                    break
                else:
                    envia_lista_usuarios(name, conn) #envia os nomes dos usuarios da sala
                    log('Informacao dos participantes', conn)

        if data == "2": #Entrar ou sair de uma sala
            f = conn.recv(1024) #recebe se a pessoa quer sair ou entrar da sala
            if f == '1': #entrar em uma sala
                t = conn.recv(1024)
                if t == '1':
                    break
                data = pickle.dumps(salas) #permite que a lista seja enviada pelo socket
                time.sleep(0.5)
                conn.send(data) #envia lista de salas
                time.sleep(0.2)
                sala_desejada = conn.recv(1024) #recebe o nome da sala selecionada
                a = str(cadastros[conn])
                b = " Entrou na Sala."
                bemvindo = a + b
                modifica_salas(sala_desejada, conn, bemvindo) #modifica banco de dados da sala
                log('Entrar na sala, Aceitado', conn)

            if f == '2': # sair de uma sala
                t = conn.recv(1024)
                if t == '0':
                    break
                sala = conn.recv(1024)
                i = sala_participantes.index(conn)
                del sala_participantes[i-1] #deleta nome da sala
                del sala_participantes[i-1] #deleta nome do usuario associado a sala
                i = salas.index(sala)
                n = salas[i + 1]
                salas[i + 1] = n-1 #diminui um participante da sala
                log('Sair da sala, Aceitado', conn)


        if data == "3":
            while True:
                sala = conn. recv(1024) #recebe o nome da sala
                f = teste_sala(sala, conn) #testa se ja tem sala e add um participante caso nao haja
                conn.send(f) #envia resultado do teste
                print(f)
                if f == '0': #caso deu certo
                    log('Criar Sala, Criado', conn)
                    break
                else:
                    log('Criar Sala, Sala ja existente', conn)


        if data == "4": #deletar sala
            time.sleep(0.2)
            lista = pickle.dumps(salas)
            conn.send(lista) #envia lista de salas existentes
            deletar = conn.recv(1024) #recebe sala a ser deletada
            i = salas.index(deletar)
            if salas[i+1] != 0: #se a sala tem usuario e nao pode ser deletado
                time.sleep(0.2)
                conn.send("0") # envia pro usuario que nao pode deltar
                log("Deletar sala, Rejeitado", conn)
            else:
                time.sleep(0.2)
                conn.send("1") # envia pro usuario que a sala foi deletada
                del salas[i] #deleta a sala
                del salas[i] #deleta o numero de participantes
                log('Deletar sala, Aceito', conn)

        if data == "5": #enviar mensagem para grupos
            sala = conn.recv(1024) #recebe a sala que tem a conversa
            a = str(cadastros[conn])
            c = " : "
            dado = conn.recv(1024) # recebe a mensagem
            msg = a + c + dado
            envia_grupo(sala, conn, msg) #envia msg para o grupo
            log('Enviar mensagem, Enviado', conn)
    conn.close()


while True:
    conn, addr = serverSocket.accept() #conectou com um cliente
    thread.start_new_thread(thread_cliente,(conn, addr)) #iniciar thread



serverSocket.close()
