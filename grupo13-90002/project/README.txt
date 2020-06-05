Grupo 13 - Alexandre Rodrigues, 90002

Para compilar o projeto:
    make
Para eliminar os objetos e executaveis:
    make clean

Para correr o servidor:
    ./server_app
Caso, por alguma razao, seja necessario alterar o port do servidor, este esta definido no topo de server_connection.clean

Para correr o cliente:
    ./client_app <ip>:25000 <cor>
A cor e lida no formato hexadecimal 0xRRGGBB. Vermelho sera entao ff0000, roxo ff00ff, etc.
Exemplo, com loopback protocol:
    ./client_app 127.0.0.1:25000 ff0000

Controlos:
    WASD - Monstro
    Rato - Pacman
Para mover o Pacman e necessario segurar o botao esquerdo do rato, e este move-se aproximadamente para a celula selecionada.