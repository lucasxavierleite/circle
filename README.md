# circle
IRC application for a Computer Networks assignment at ICMC - USP </br>

[Apresentação do trabalho](https://drive.google.com/file/d/1VmpJvFG03Py8MxPS-njv0ebPKdevGw_N/view?usp=sharing)

Altair Fernando Pereira Junior, 9391831 <br/>
Osni Brito de Jesus, 11857330 <br/>
Lucas Xavier Leite, 10783347 <br/>

### Projeto
Neste projeto para a disciplina de redes de computadores o grupo implementou um **Internet Relay Chat (IRC)** com algumas adaptações do protocolo **RFC 1459**. Onde é possivel conectar diversos usuarios (clientes) a um servidor e estabelecer uma troca de mensagens entre si. Algumas funcionalidades serão disponiveis atraves de comandos.



### Build

Primeiramente é necessario criar a pasta build, onde ficará os arquivos executáveis. Isso para o servidor e cliente.

```
mkdir build
```

Apos criar a pasta build, é possivel compilar o servidor e cliente com o comando:
```
make all
```
Ou pode ser compilado separadamente
```
make server
```
```
make client
```

Para executar servidor e cliente:
```
make run_server
```
```
make run_client
```

**Sistemas operacionais testados:** <br/>

Arch Linux 5.18.9-arch1-1 x86_64 GNU/Linux <br/>
5.17.15-1-MANJARO x86_64 GNU/Linux <br/>

**Compilador:** gcc version 12.1.0 <br/>

## Comandos
### Comandos gerais
- **/connect:** Conecta um novo cliente no servidor (que deve estar em execução)
- **/quit:** Encerra conexão com o servidor
- **/ping:** Recebe a mensagem 'pong' do servidor
- **/join _nomeCanal_:** Entra no canal 'nomeCanal' (se for inexistente é criado)
- **/nickname _apelido_:** Usuario é reconhecido pelo 'apelido'
### Comandos para admnistrador
- **/kick _usuario_:** Remove a conexao de 'usuario'
- **/mute _usuario_:** Impossibilita que o 'usuario' envie novas mensagens
- **/unmute _usuario_:** Remove 'mute' do usuario
- **/whois _usuario_:** Retorna o IP do usuario para o admnistrador

## Instruções:

Com o servidor rodando, um cliente pode se conectar ao servidor atraves do comando _/connect_. <br/>
Apos conectado o usuario pode entrar ou criar um canal atraves do comando _/join_ (com o protocolo adotado, o nome de um canal deve iniciar com '#' ou '&' sem espaços em branco).<br/>
O usuario que criar o canal se torna o admnistrador do canal, com privilégios de alguns comandos. <br/>
CTRL+D é equivalente ao /quit
