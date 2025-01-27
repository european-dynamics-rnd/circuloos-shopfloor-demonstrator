# Orion, mintaka restart and mongodb is not live

```console
cd /circuloos-data-platform
./service stop
docker run --privileged --rm tonistiigi/binfmt --install all
./service start
 ```
 