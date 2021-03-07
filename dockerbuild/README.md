# Instalare docker
Urmati instructiunile de mai jos pentru a instala docker pe:
- [windows](https://docs.docker.com/docker-for-windows/install/)
- [linux](https://docs.docker.com/engine/install/)
- [mac](https://docs.docker.com/docker-for-mac/install/)

# Cum construim imaginea?
Daca sunteti pe linux rulati `./build.sh`

Daca sunteti pe windows si folositi CMD, intrati in folderul `dockerbuild/` si rulati: `docker build -t "proiect_ip:1.0.0" .`

# Cum construim imaginea cu `docker-compose`?
Rulati `docker-compose build`

# Cum rulam containerele cu `docker-compose`?
Rulati `docker-compose up`

# Cum listam containerele lansate cu `docker-compose`?
In folderul unde este si fisierul `docker-compose.yml` rulati `docker-compose ps`

# Cum vedem ce imagini construite avem in docker?
Deschideti un terminal/consola si rulati `docker images`.

Obtinem un output de genul:
```
REPOSITORY    TAG       IMAGE ID       CREATED              SIZE
proiect_ip    1.0.0     e16a250c59dd   About a minute ago   1.14GB
ubuntu        latest    f63181f19b2f   5 weeks ago          72.9MB
```

# Cum lansam un container insotit de un terminal?
Containerele sunt in general rulate cu `docker run [OPTIONS] [IMAGE]`.

Daca dorim sa pornim un terminal si sa fim bagati direct in terminal folosim `docker run -it [IMAGE]` unde inlocuim `[IMAGE]` cu ID-ul obinut din lista de imagini.

De exemplu `docker run -it e16a250c59dd`

Ar trebui apoi sa aveti un terminal de genul:
```
root@63a76f4bd728:~# 
```

# Cum vedem ce containere ruleaza?
Folosim `docker ps -a` si obtinem ceva de genul:

```
CONTAINER ID   IMAGE          COMMAND       CREATED          STATUS                      PORTS                              NAMES
e1b9f61debca   e16a250c59dd   "-rm"         21 seconds ago   Created                     80/tcp, 80/udp, 443/tcp, 443/udp   pedantic_goodall
19718c520add   6d320579a4a8   "/bin/bash"   26 minutes ago   Exited (0) 24 minutes ago                                      keen_ride
```

# Cum inchidem un container?
Folosim `docker rm [CONTAINER ID]` de exemplu: `docker rm e1b9f61debca`

# Cum stergem o imagine de care nu mai avem nevoie?
Folosim `docker image rm [IMAGE ID]` de exemplu `docker image rm e16a250c59dd`

Observatie: Este recomandat (uneori chiar necesar) ca niciun container sa nu ruleze pe imaginea pe care dorim sa o stergem.
